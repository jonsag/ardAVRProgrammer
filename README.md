# ardATProgrammer
Some code and a shield for programming ATMega328 and ATtiny85

## Additional Boards Managers URL
Insert these URLs in File -> Preferences -> Settings tab  

>https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json  
>http://www.leonardomiliani.com/repository/package_leonardomiliani.com_index.json  
>https://raw.githubusercontent.com/carlosefr/atmega/master/package_carlosefr_atmega_index.json  

Then install boards in Tools -> Board -> Boards manager...  

>attiny  
>ATtiny extra Boards  
>ATmega 168P/328P Boards  
>Barebones ATmega Chips (no bootloader)  


## Burn bootloader, settings

### ATtiny85 
Board -> ATtiny Microcontrollers -> ATtiny 25/45/85  
Processor -> ATtiny85  
Clock -> ... 
Programmer -> Arduino as ISP  

### ATtiny2313
Board -> ATtiny MCUs -> ATtiny2313/4313  
Micro -> ATtiny2313 @ ...  
Programmer -> Arduino as ISP
### ATmega328P
Board -> Atmega168/328 -> Atmega168/328  
Micro -> ATmega328P @ ... w/Arduino as ISP

### ATmega328
No solution to burn bootloader at the moment  

## Program AVR, setttings

### ATtiny85 
Board -> ATtiny Microcontrollers -> ATtiny 25/45/85  
Processor -> ATtiny85  
Clock -> ... 
Programmer -> AVRISP mkII

### ATtiny2313
Board -> ATtiny MCUs -> ATtiny2313/4313  
Micro -> ATtiny2313 @ ...  
Programmer -> AVRISP mkII
### ATmega328P
Board -> Atmega168/328 -> Atmega168/328  
Micro -> ATmega328P @ ... w/Arduino as ISP

### ATmega328
Board -> ATmega Microcontrollers -> ATmega328/328P  
Processor -> ATmega328  
Clock -> ... 
Programmer -> AVRISP mkII

## Pin Mapping, Arduino IDE -> AVR

### Arduino code -> ATtiny85 pin number -> Pin	name ->	Function
0 -> 5 -> PB0 ->  
1 -> 6 -> PB1 ->  
2 -> 7 -> PB2 ->  
3 -> 2 -> PB3 ->  
4 -> 3 -> PB4 ->  
5 -> 1 -> PB5 -> RESET  

### Arduino code -> ATmega328 pin number -> Pin name -> Function
  -> 1 -> PC6 -> RESET  
0 -> 2 -> PD0 -> Digital pin 0 (Rx)  
1 -> 3 -> PD1 -> Digital pin 1 (Tx)  
2 -> 4 -> PD2 -> Digital pin 2  
3 -> 5 -> PD3 -> Digital pin 3 (PWM)  
4 -> 6 -> PD4 -> Digital pin 4  
5 -> 11 -> PD5 -> Digital pin 5  
6 -> 12 -> PD6 -> Digital pin 6  
7 -> 13 -> PD7 -> Digital pin 7  
8 -> 14 -> PB0 -> Digital pin 8  
9 -> 15 -> PB1 -> Digital pin 9 (PWM)  
10 -> 16 -> PB2 -> Digital pin 10 (PWM)  
11 -> 17 -> PB3 -> Digital pin 11 (PWM)  
12 -> 18 -> PB4 -> Digital pin 12  
13 -> 19 -> PB5 -> Digital pin 13  
14 -> 23 -> PC0 -> Analog input 0  
15 -> 24 -> PC1 -> Analog input 1  
16 -> 25 -> PC2 -> Analog input 2  
17 -> 26 -> PC3 -> Analog input 3  
18 -> 27 -> PC4 -> Analog input 4  
19 -> 28 -> PC5 -> Analog input 5

## Resources
[Breadboard @gammon.com](http://www.gammon.com.au/breadboard) (excellent!)  
[nickgammon arduino sketches @github](https://github.com/nickgammon/arduino_sketches)  
[Alternative Atmega bootloader programmer @gammon.com](http://www.gammon.com.au/forum/?id=11635)  
