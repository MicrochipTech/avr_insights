# Example Project using the ATmega324PB

Do you have an ADC error because your ADC input voltage range is too large? Want to increase sensor resolution by decreasing adc voltage input? Having a problem with ADC offset error? Learn how to use the AVR ADC Differential mode!

The AVR ADC differential mode allows you to change the range of the adc inputs to increase the resolution of measuring the signals coming to the ADC. The ADC changes the resolution by changing the upper limit as compared to the negative input of the ADC. This way, you can measure smaller voltage ranges, such as the signal over a load that ranges three volts. The differential mode will also allow you to calibrate ADC offset voltage. Check out the example code below for an explanation on how to read the adc offset.

[![AVR® Insights - Episode 15 - ADC Differential Mode](https://img.youtube.com/vi/rh_8gBkiQk4/0.jpg)](https://www.youtube.com/watch?v=rh_8gBkiQk4)

### Documentation Reference Links

[Microchip Tools Documentation](https://mchp.us/2WOO2eE)

[AVR Libc Reference Manual](https://www.microchip.com/webdoc/AVRLibcReferenceManual/index.html)

[ATmega324PB Complete Datasheet](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590812)

[ATmega 324PB Xplained Pro User Guide](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en590285)

### Application Notes

[AVR127: Understanding ADC parameters](https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en590903)

[AVR120: Characterization and Calibration of the ADC on an AVR](https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en591791)