# Example Project using the ATmega324PB

Do you know why you need a Read-While-Write section of Memory? Do you need to write and read code at the same time? You can decrease code execution time by saving certain parts of your program in the read section while also reading and writing to other areas of memory. This is especially useful when it comes to using a bootloader while also reading data coming into the microcontroller via the Flash memory or EEPROM memory. Check out the example code below to see how much of difference it makes when you are using Read-while-Write memory. 

[![AVR® Insights - Episode 2 - Read-While-Write Memory](https://img.youtube.com/vi/YKFWUX9w4eY/0.jpg)](https://www.youtube.com/watch?v=YKFWUX9w4eY)

### Documentation Reference Links

[Microchip Tools Documentation](https://mchp.us/2Q04giE)

[AVR Libc Reference Manual](https://www.microchip.com/webdoc/AVRLibcReferenceManual/index.html)

[ATmega324PB Complete Datasheet](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590812)

[ATmega 324PB Xplained Pro User Guide](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590285)

### Application Notes

[AVR109: Self Programming](https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en591230)

[AVR106: C Functions for Reading and Writing to Flash Memory](https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en591795)

