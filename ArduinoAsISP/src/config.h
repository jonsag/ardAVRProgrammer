//#define LOW_SPEED
#ifdef LOW_SPEED
#define EXTRA_SPI_DELAY 125
#else
#define EXTRA_SPI_DELAY 0
#endif

#include "pins_arduino.h" // defines SS,MOSI,MISO,SCK
#define RESET SS

#define LED_ERR 7
#define LED_PMODE 8
#define LED_HB 6
//#define PIEZO A3

#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
const byte STK_OK = 0x10;
const byte STK_FAILED = 0x11;
const byte STK_UNKNOWN = 0x12;
const byte STK_INSYNC = 0x14;
const byte STK_NOSYNC = 0x15;
const byte CRC_EOP = 0x20; //ok it is a space...

const byte STK_GET_SYNC = 0x30;
const byte STK_GET_SIGNON = 0x31;
const byte STK_GET_PARM = 0x41;
const byte STK_SET_PARM = 0x42;
const byte STK_SET_PARM_EXT = 0x45;
const byte STK_PMODE_START = 0x50;
const byte STK_PMODE_END = 0x51;
const byte STK_SET_ADDR = 0x55;
const byte STK_UNIVERSAL = 0x56;
const byte STK_PROG_FLASH = 0x60;
const byte STK_PROG_DATA = 0x61;
const byte STK_PROG_PAGE = 0x64;
const byte STK_READ_PAGE = 0x74;
const byte STK_READ_SIGN = 0x75;

//// TONES  ==========================================
//// Start by defining the relationship between
////       note, period, &  frequency.
//#define  c     3830    // 261 Hz
//#define  d     3400    // 294 Hz
//#define  e     3038    // 329 Hz
//#define  f     2864    // 349 Hz
//#define  g     2550    // 392 Hz
//#define  a     2272    // 440 Hz
//#define  b     2028    // 493 Hz
//#define  C     1912    // 523 Hz

//void pulse(int pin, int times);

int error = 0;
int pmode = 0;
// address for reading and writing, set by STK_SET_ADDR command
int _addr;
byte _buffer[256]; // serial port buffer
int pBuffer = 0;   // buffer pointer
int iBuffer = 0;   // buffer index
byte buff[256];    // temporary buffer
boolean EOP_SEEN = false;

#define beget16(addr) (*addr * 256 + *(addr + 1))
typedef struct param
{
  byte devicecode;
  byte revision;
  byte progtype;
  byte parmode;
  byte polling;
  byte selftimed;
  byte lockbytes;
  byte fusebytes;
  int flashpoll;
  int eeprompoll;
  int pagesize;
  int eepromsize;
  int flashsize;
} parameter;

parameter param;

// this provides a heartbeat on pin 6, so you can tell the software is running.
byte hbval = 128;
int8_t hbdelta = 4;
void heartbeat()
{
  if (hbval > 192)
    hbdelta = -hbdelta;
  if (hbval < 32)
    hbdelta = -hbdelta;
  if (hbval > 250)
    hbdelta = -hbdelta;
  if (hbval < 10)
    hbdelta = -hbdelta;
  hbval += hbdelta;
  analogWrite(LED_HB, hbval);
  delay(20);
}
