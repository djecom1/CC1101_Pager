Pager-cc1101
============

Simple 2 way pager with ack.
It use :
-for TX : Arduino Uno + TI-CC1101
-for RX : Arduino Uno + TI-CC1101 + Oled diplay i2c

arduino-cc1101
==============

This is an Arduino library for interfacing with CC1101 transceivers.

This project is a fork of the [panStamp arduino_avr project][1], with
only the minimum requirements to interface with CC1101 transceivers.

[1]: https://github.com/panStamp/arduino_avr "panStamp arduino_avr"


Installation
------------

-Include cc1101 library to your Arduino IDE
-Upload TX.ino in your TX Arduino
-Upload RX.ino in your RX Arduino


Usage
-----

RX works without computer.
Plug TX Arduino to your computer and open a serial monitor (9600bds) to see ack messages.

F4GMU
