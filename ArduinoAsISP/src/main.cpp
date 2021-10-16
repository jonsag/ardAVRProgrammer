/* ardAVRProgrammer
 *  
 * by phenoptix
 * code from https://www.instructables.com/Building-an-ISP-Shield-for-Arduino/ 
 * 
 * /

/* Making a barebones arduino
 *  
 *  instructions from https://www.instructables.com/Programming-Chips-With-Your-Arduino-AVR-ISP-Coveri/
 *  
 *  Go to > Tools on your IDE, 
 *  Now > Programmer > Arduino as ISP. 
 *  With that checked we can then go back to > Tools > Burn Bootloader. 
 *  Now we're going to see some flashing LEDs and wait a while. 
 *  The message at the bottom of your IDE should eventually turn to Bootloading complete. 
 *  Hey presto, that chip on the breadboard can now be turned into a barebones Arduino!
 *  such as on http://start.shrimping.it/kit/shrimp.html
 *  
 */

// this sketch turns the Arduino into a AVRISP
// using the following pins:
// 10: slave reset
// 11: MOSI
// 12: MISO
// 13: SCK

// Put an LED (with resistor) on the following pins:
//  7: Error - Lights up if something goes wrong (use red if that makes sense)
//  8: Programming - In communication with the slave
//  6: Heartbeat - shows the programmer is running (removed, see notes below)
// Optional - Piezo speaker on pin A3
//
// October 2009 by David A. Mellis
// - Added support for the read signature command
//
// February 2009 by Randall Bohn
// - Added support for writing to EEPROM (what took so long?)
// Windows users should consider WinAVR's avrdude instead of the
// avrdude included with Arduino software.
//
// January 2008 by Randall Bohn
// - Thanks to Amplificar for helping me with the STK500 protocol
// - The AVRISP/STK500 (mk I) protocol is used in the arduino bootloader
// - The SPI functions herein were developed for the AVR910_ARD programmer
// - More information at http://code.google.com/p/mega-isp
//
// March 2012 - William Phelps
// modify to work with Arduino IDE 1.0 which has a shorter serial port receive buffer
// getEOP() now gets entire request before avrisp() is called to process it
// Serial.print((char) xxx) changed to Serial.write(xxx)
// uint8_t changed to byte
// added support for Piezo speaker
// moved Pmode LED to A0
// removed "heartbeat" on pin 6, added short blip of ERROR LED instead
// Why is it that PROG_FLASH and PROG_DATA don't actually do anything???
// Tested with Arduino IDE 22 and 1.0
// IDE 22 - 5148 bytes
// IDE 1.0 - 5524 bytes!

// January 2014 - Ben Gray
// Put the heartbeat LED back and switched around the pins a little for the LEDs.

// SLOW SPEED CHIP ERASE AND FUSE BURNING
//
// Enable LOW_SPEED to allow you to erase chips that would fail otherwise,
// for being running with a clock too slow for the programmer.
//
// This allowed me to recover several ATMega328 that had no boot loader and the
// first instruction was to set the clock to the slowest speed. Usually this
// kind of recovery requires high voltage programming, but this trick will do
// just fine.
//
// How to proceed:
// 1. Enable LOW_SPEED, and load it to the programmer.
// 2. Erase and burn the fuses on the target uC. Example for ATMega328:
//   arduino-1.0.1/hardware/tools/avrdude -Carduino-1.0.1/hardware/tools/avrdude.conf -patmega328p -cstk500v1 -P /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A900cf1Q-if00-port0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0x05:m -Uhfuse:w:0xDA:m -Ulfuse:w:0xF7:m
// 3. Comment LOW_SPEED and load it back to the programmer.
// 4. Program the target uC as usual. Example:
//  arduino-1.0.1/hardware/tools/avrdude -Carduino-1.0.1/hardware/tools/avrdude.conf -patmega328p -cstk500v1 -P /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A900cf1Q-if00-port0 -b19200 -Uflash:w:firmware.hex:i
//
// Note 1: EXTRA_SPI_DELAY was added to let you slow down SPI even more. You can
// play with the value if it does not work with the default.
// Note 2: LOW_SPEED will alow you only to erase the chip and burn the fuses! It
// will fail if you try to program the target uC this way!

#include <Arduino.h>

#include "config.h"
#include "modules.h"

void setup()
{

  Serial.begin(19200);
  //  pinMode(PIEZO, OUTPUT);
  //  beep(1700, 40);
  EOP_SEEN = false;
  iBuffer = pBuffer = 0;

  pinMode(LED_PMODE, OUTPUT);
  pulse(LED_PMODE, 2);
  pinMode(LED_ERR, OUTPUT);
  pulse(LED_ERR, 2);
  pinMode(LED_HB, OUTPUT);
  pulse(LED_HB, 2);

  pinMode(9, OUTPUT);
  // setup high freq PWM on pin 9 (timer 1)
  // 50% duty cycle -> 8 MHz
  OCR1A = 0;
  ICR1 = 1;
  // OC1A output, fast PWM
  TCCR1A = _BV(WGM11) | _BV(COM1A1);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // no clock prescale
}

void loop(void)
{
  // is pmode active?
  //  if (pmode) digitalWrite(LED_PMODE, HIGH);
  //  else digitalWrite(LED_PMODE, LOW);
  digitalWrite(LED_PMODE, LOW);
  // is there an error?
  if (error)
    digitalWrite(LED_ERR, HIGH);
  else
    digitalWrite(LED_ERR, LOW);

  getEOP();

  // have we received a complete request?  (ends with CRC_EOP)
  if (EOP_SEEN)
  {
    digitalWrite(LED_PMODE, HIGH);
    EOP_SEEN = false;
    avrisp();
    iBuffer = pBuffer = 0; // restart buffer
  }
}
