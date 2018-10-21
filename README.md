# iAQcore
Arduino library for the iAQ-core indoor air quality sensor module with I2C interface from ams.

## Introduction
This project is an Arduino *library*. It implements a driver for the iAQcore.
This chip is a indoor air quality sensor module with an I2C interface.

The code has been tested with
 - [NodeMCU (ESP8266)](https://www.aliexpress.com/item/NodeMCU-V3-Lua-WIFI-module-integration-of-ESP8266-extra-memory-32M-flash-USB-serial-CP2102/32779738528.html)
 - [Arduino pro mini](https://www.aliexpress.com/item/ProMini-ATmega328P-3-3V-Compatible-for-Arduino-Pro-Mini/32525927539.html)
 - [Arduino nano](https://www.aliexpress.com/item/Nano-CH340-ATmega328P-MicroUSB-Compatible-for-Arduino-Nano-V3/32572612009.html)
 - [anwofis](https://github.com/anwofis) has tested the library with STM32duino and a "Blue Pill" STM32F103C8 with
   I2C clock speed at 100kHz and 0.1 µF capacitor between GND and VCC near the sensor.

Note that the iAQcore requires a supply voltage of 3.3V ± 0.1V.
So, 3.3V is ok, but *do not use a 5V board*.
The Nano has 3v3 supply, but runs I2C on 5V. This does seem to work.

## Links
The iAQcore is made by [ams](http://www.ams.com). This library is compatible with the following variants.
 - Find the datasheet of the iAQcore continuous on the
   [product page](http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C).
 - Find the datasheet of the iAQcore pulsed on the
   [product page](http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-P).

## Prerequisites
It is assumed that
 - The Arduino IDE has been installed.
   If not, refer to "Install the Arduino Desktop IDE" on the
   [Arduino site](https://www.arduino.cc/en/Guide/HomePage).
 - The library directory is at its default location.
   For me, Maarten, that is `C:\Users\maarten\Documents\Arduino\libraries`.

## Installation
Installation steps
 - Visit the [project page](https://github.com/maarten-pennings/iAQcore) for the Arduino iAQcore library.
 - Click the green button `Clone or download` on the right side.
 - From the pop-up choose `Download ZIP`.
 - Start Arduino, select from menu Sketch > Include Library > Add .ZIP Library... and select the just downloaded ZIP file.
 - An alternative to the last step is to manually add the library
    - Unzip the file "Here", so that this `README.md` is in the top-level directory with the name `iAQcore-master`.
    - Copy the entire tree to the Arduino library directory.
      This `README.md` should be located at e.g.
      `C:\Users\maarten\Documents\Arduino\libraries\iAQcore\README.md`.

## Build an example
To build an example sketch
 - (Re)start Arduino.
 - Open File > Example > Examples from Custom Libraries > iAQcore > iAQcore-simple.
 - Make sure Tools > Board lists the correct board.
 - Select Sketch > Verify/Compile.

## Wiring
This library has been tested with three boards.

For the NodeMCU (ESP8266), connect as follows (I did not use pull-ups, presumably they are inside the MCU)

| iAQcore | ESP8266 |
|:-------:|:-------:|
|   VCC   |   3V3   |
|   GND   |   GND   |
|   SDA   |   D2    |
|   SCL   |   D1    |

![wiring ESP8266 NoeMCU](wire-esp.jpg)

For the Pro mini (do *not* use a 5V board), connect as follows  (I did not use pull-ups, presumably they are inside the MCU)

| iAQcore | Pro mini |
|:-------:|:--------:|
|   VCC   |   VCC    |
|   GND   |   GND    |
|   SDA   |    A4    |
|   SCL   |    A5    |

![wiring pro mini](wire-promini.jpg)

For the Arduino Nano, connect as follows  (I did not use pull-ups, presumably they are inside the MCU)

| iAQcore |   Nano   |
|:-------:|:--------:|
|   VCC   |   3V3    |
|   GND   |   GND    |
|   SDA   |    A4    |
|   SCL   |    A5    |

![wiring nano](wire-nanov3.jpg)

Connect the iAQcore module as follows

![wiring iAQcore](wire-iaqcore.jpg)


## Flash an example
To build an example sketch
 - (Re)start Arduino.
 - Open File > Example > Examples from Custom Libraries > iAQcore > iAQcore-simple.
 - Alternatively open File > Example > Examples from Custom Libraries > iAQcore > iAQcore-full.
 - In `setup()` make sure to start the I2C driver correctly.
   For example, for ESP8266 NodeMCU have
     ```C++
     // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
     Wire.begin(D2,D1); 
     Wire.setClockStretchLimit(1000); 
     ```
   and for Arduino pro mini or Nano have
     ```C++
     // Enable I2C for Arduino pro mini or Nano [VDD to VCC/3V3, GND to GND, SDA to A4, SCL to A5]
     Wire.begin(); 
     ```
 - Make sure Tools > Board lists the correct board.
 - Select Sketch > Upload.
 - Select Tools > Serial Monitor.
 - Enjoy the output, which should be like this for `iAQcore-simple`:

     ```Text
     Starting iAQcore simple demo
     iAQcore: eco2=456 ppm,  stat=0x0,  resist=363157 ohm,  tvoc=127 ppb
     iAQcore: eco2=453 ppm,  stat=0x0,  resist=363586 ohm,  tvoc=126 ppb
     iAQcore: eco2=455 ppm,  stat=0x0,  resist=363157 ohm,  tvoc=127 ppb
     iAQcore: eco2=450 ppm,  stat=0x0,  resist=364447 ohm,  tvoc=125 ppb
     iAQcore: eco2=450 ppm,  stat=0x0,  resist=364878 ohm,  tvoc=125 ppb
     iAQcore: eco2=605 ppm,  stat=0x0,  resist=331010 ohm,  tvoc=168 ppb
     iAQcore: eco2=1560 ppm,  stat=0x0,  resist=212175 ohm,  tvoc=431 ppb
     iAQcore: eco2=3721 ppm,  stat=0x0,  resist=117065 ohm,  tvoc=1026 ppb
     iAQcore: eco2=4749 ppm,  stat=0x0,  resist=96488 ohm,  tvoc=1309 ppb
     iAQcore: eco2=6127 ppm,  stat=0x0,  resist=78091 ohm,  tvoc=1689 ppb
     iAQcore: eco2=4822 ppm,  stat=0x0,  resist=95307 ohm,  tvoc=1329 ppb
     iAQcore: eco2=3645 ppm,  stat=0x0,  resist=118936 ohm,  tvoc=1005 ppb
     iAQcore: eco2=2830 ppm,  stat=0x0,  resist=143618 ohm,  tvoc=781 ppb
     iAQcore: eco2=2221 ppm,  stat=0x0,  resist=169921 ohm,  tvoc=613 ppb
     iAQcore: eco2=1483 ppm,  stat=0x0,  resist=218476 ohm,  tvoc=410 ppb
     iAQcore: eco2=1091 ppm,  stat=0x0,  resist=257597 ohm,  tvoc=302 ppb
     iAQcore: eco2=912 ppm,  stat=0x0,  resist=280448 ohm,  tvoc=253 ppb
     iAQcore: eco2=852 ppm,  stat=0x0,  resist=289063 ohm,  tvoc=236 ppb
     iAQcore: eco2=824 ppm,  stat=0x0,  resist=293181 ohm,  tvoc=228 ppb
     ```

## Clock stretching
I2C _slave_ devices, such as the iAQcore, are controlled by I2C _masters_, such as the ESP8266, pro mini or nano.
The master is in charge of the communication to the slave: it toggles the clock line (SCL) high and low.
Each clock pulse one bit is transferred between the master and the slave (from or to).
If the slave is not yet ready, it may _stretch the clock_, i.e. force the SCL line low, so that the master can not complete the pulse.

The figure below shows that after the master has send the address byte of the iAQcore, this iAQcore chip
stretches the SCL line for 306.8 us. 

![Clock stretching](iaqcore-clock-stretch.png)

Unfortunately, the ESP8266 I2C library has a clock stretch timeout of [230us](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/core_esp8266_si2c.c#L74).
So, this timeout must be set to a higher value: `Wire.setClockStretchLimit(1000)`.

(end of doc)
