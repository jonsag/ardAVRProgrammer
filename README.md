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


