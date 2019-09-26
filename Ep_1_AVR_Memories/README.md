# Example Project using the ATmega324PB

Learn how the memory is organized in AVR MCUs and why you should read and write to Flash memory or EEPROM memory. The AVR MCU memory is based upon a Harvard architecture, which means that the Flash memory is in a different area than the RAM, IO registers, and EEPROM memory. If you are having problems writing to Flash memory, you may be able to fix your problem by writing to EEPROM memory instead. For example, frequent writing to the Flash memory in AVR devices can cause errors that are solved by writing to EEPROM instead. 
The example code functions which shows reading and writing Flash memory in AVR microcontrollers. A second example also shows reading and writing to EEPROM memory in AVR microcontrollers. Both these examples show how to use the LIBc in Atmel Studio to program memory. 

[![AVR® Insights - Episode 1 - AVR Memories](https://img.youtube.com/vi/ZeY6BKqIZGk/0.jpg)](https://www.youtube.com/watch?v=ZeY6BKqIZGk)

### Documentation Reference Links

[Microchip Tools Documentation](https://mchp.us/2JCAAqL)

[AVR Libc Reference Manual](https://www.microchip.com/webdoc/AVRLibcReferenceManual/index.html)

[ATmega324PB Complete Datasheet](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590812)

[ATmega 324PB Xplained Pro User Guide](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590285)