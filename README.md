# Zumo32U4 library

Version: 1.1.0<br/>
Release date: 2015 May 06<br/>
[www.pololu.com](http://www.pololu.com/)

## Summary

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the Zumo 32U4 robot.

The Zumo 32U4 robot consists of the Zumo chassis, the Zumo 32U4 Main Board, and the Zumo 32U4 Front Sensor Array.  It has an integrated AVR ATmega32U4 microcontroller, motor drivers, encoders, proximity sensors, line sensors, buzzer, four buttons, LCD connector, LSM303D accelerometer, and L3GD20H gyro.  The user's guide for the Zumo 32U4 robot is here:

https://www.pololu.com/docs/0J63

Please note that this library does NOT work with the Zumo Shield for Arduino, which is a very different product.  The Zumo Shield does not have an integrated microcontroller, so it must be connected to an Arduino-sized board to run.  If you have the Zumo Shield, then you should not use this library and instead refer to the Zumo Shield documentation [here](https://www.pololu.com/docs/0J57).

## Installing the library

You can download this library from the [Zumo 32U4 Robot User's Guide](https://www.pololu.com/docs/0J63), or from the [a-star repository on GitHub](https://github.com/pololu/a-star).  Extract the files from the downloaded ZIP file and find the "Zumo32U4" directory, which is inside the "libraries" folder.  Copy the "Zumo32U4" folder into the "libraries" subdirectory inside your Arduino sketchbook directory.  The Arduino sketchbook location is typically in your Documents folder in a subfolder named "Arduino".  You can see the sketchbook location in the Arduino IDE Preferences dialog, which is available from the File menu.

For example, if you are using Windows and you have not changed the sketchbook location, the "Zumo32U4" folder would be copied to:

    C:\Users\<username>\Documents\Arduino\libraries\Zumo32U4

Next, restart the Arduino IDE.

After installing, you should look for the "Zumo32U4" entry in the "Examples" menu.  This is where you can find all the example code provided by this library.  If you cannot find the examples, then the library was probably not installed correctly and you should retry the installation instructions above.

## Classes and functions

The main classes and functions provided by the library are listed below:

* L3G
* LSM303
* Zumo32U4ButtonA
* Zumo32U4ButtonB
* Zumo32U4ButtonC
* Zumo32U4Buzzer
* Zumo32U4Encoders
* Zumo32U4IRPulses
* Zumo32U4LCD
* Zumo32U4LineSensors
* Zumo32U4Motors
* Zumo32U4ProximitySensors
* ledRed()
* ledGreen()
* ledYellow()
* usbPowerPresent()
* readBatteryMillivolts()

## Component libraries

This library also includes copies of several other Arduino libraries inside it which are used to help implement the classes and functions above.

* [FastGPIO](https://github.com/pololu/fastgpio-arduino)
* [LSM303](https://github.com/pololu/lsm303-arduino)
* [L3G](https://github.com/pololu/l3g-arduino)
* [PololuBuzzer](https://github.com/pololu/pololu-buzzer-arduino)
* [PololuHD44780](https://github.com/pololu/pololu-hd44780-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)
* [QTRSensors](https://github.com/pololu/qtr-sensors-arduino)
* [USBPause](https://github.com/pololu/usb-pause-arduino)

You can use these libraries in your sketch automatically without any extra installation steps and without needing to add any extra `#include` lines to your sketch.

You should avoid adding extra `#include` lines such as `#include <Pushbutton.h>` because then the Arduino IDE might try to use the standalone Pushbutton library (if you previously installed it), and it would conflict with the copy of the Pushbutton code included in this library.  The only `#include` lines needed to access all features of this library are:

~~~{.cpp}
#include <Wire.h>
#include <Zumo32U4.h>
~~~

## Documentation

For complete documentation, see https://pololu.github.io/zumo-32u4.  If you are already on that page, then click on the links in the "Classes and functions" section above.

## Version history

* 1.1.0 (2015 May 06): Updated FastGPIO to version 1.0.2.  Fixed a bug in Zumo32U4ProximitySensors where the wrong array length was used.  Added five demos: RotationResist, FaceUphill, RemoteControl, Balancing, and SumoProximitySensors.
* 1.0.1 (2015 Mar 11): Improve the Buttons example.
* 1.0.0 (2015 Mar 05): Original release.
