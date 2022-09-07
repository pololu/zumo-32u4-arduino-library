# Zumo32U4 library

[www.pololu.com](http://www.pololu.com/)

## Summary

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the [Zumo 32U4 robot](https://www.pololu.com/category/170/zumo-32u4-oled-robot) (both the [newer OLED version](https://www.pololu.com/category/170/zumo-32u4-robot) and the [original LCD version](https://www.pololu.com/category/286/original-zumo-32u4-robot)).

The Zumo 32U4 robot consists of the Zumo chassis, the Zumo 32U4 Main Board, and the Zumo 32U4 Front Sensor Array.  It has an integrated AVR ATmega32U4 microcontroller, motor drivers, encoders, proximity sensors, line sensors, inertial sensors, buzzer, four buttons, and display screen (graphical OLED or LCD).  The user's guide for the Zumo 32U4 robot is here:

https://www.pololu.com/docs/0J63

Please note that this library does NOT work with the Zumo Shield for Arduino, which is a very different product.  The Zumo Shield does not have an integrated microcontroller, so it must be connected to an Arduino-sized board to run.  If you have the Zumo Shield, then you should not use this library and instead refer to the Zumo Shield documentation [here](https://www.pololu.com/docs/0J57).

## Installing the library

Use the Library Manager in version 1.8.10 or later of the Arduino software (IDE) to install this library:

1. In the Arduino IDE, open the "Tools" menu and select "Manage Libraries...".
2. Search for "Zumo32U4".
3. Click the Zumo32U4 entry in the list.
4. Click "Install".
5. If you see a prompt asking to install missing dependencies, click "Install all".

## Usage

To access all features of this library, you just need these include statements:

```cpp
#include <Wire.h>
#include <Zumo32U4.h>
```

## Examples

Several example sketches are available that show how to use the library.  You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "Zumo32U4".  If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Classes and functions

The main classes and functions provided by the library are listed below:

* Zumo32U4ButtonA
* Zumo32U4ButtonB
* Zumo32U4ButtonC
* Zumo32U4Buzzer
* Zumo32U4Encoders
* Zumo32U4IMU
* Zumo32U4IRPulses
* Zumo32U4LCD
* Zumo32U4LineSensors
* Zumo32U4Motors
* Zumo32U4OLED
* Zumo32U4ProximitySensors
* ledRed()
* ledGreen()
* ledYellow()
* usbPowerPresent()
* readBatteryMillivolts()

## Dependencies

This library also references several other Arduino libraries which are used to help implement the classes and functions above.

* [FastGPIO](https://github.com/pololu/fastgpio-arduino)
* [PololuBuzzer](https://github.com/pololu/pololu-buzzer-arduino)
* [PololuHD44780](https://github.com/pololu/pololu-hd44780-arduino)
* [PololuMenu](https://github.com/pololu/pololu-menu-arduino)
* [PololuOLED](https://github.com/pololu/pololu-oled-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)
* [USBPause](https://github.com/pololu/usb-pause-arduino)

## Documentation

For complete documentation, see https://pololu.github.io/zumo-32u4-arduino-library.  If you are already on that page, then click on the links in the "Classes and functions" section above.

## Version history

* 2.0.1 (2022-09-06): Fixed a bug in the Encoders demo that could prevent encoder errors from being shown properly on the display.
* 2.0.0 (2021-11-23): Added support for the Zumo 32U4 OLED. Reorganized the library's directory structure and replaced most of the bundled component libraries with Arduino Library Manager dependencies. Removed the previously bundled LSM303 and L3G libraries.
* 1.2.0 (2020-09-11): Added a Zumo32U4IMU class that abstracts some details of the inertial sensors and supports different IMU types. The examples have been updated to use this class.
* 1.1.4 (2017-07-17): Fixed a bug that caused errors from the right encoder to be reported as errors from the left encoder.
* 1.1.3 (2016-10-12): Fixed a bug that caused the buzzer's `isPlaying` method to malfunction sometimes when link time optimization is enabled.  Also incorporated some minor fixes to the QTRSensors and LSM303 libraries.
* 1.1.2 (2016-03-14): Updated the Demo example so it can compile in Arduino 1.6.7.
* 1.1.1 (2015-09-01): Moved the library out of the a-star repository into its own repository. Added Demo example.
* 1.1.0 (2015-05-06): Updated FastGPIO to version 1.0.2.  Fixed a bug in Zumo32U4ProximitySensors where the wrong array length was used.  Added five demos: RotationResist, FaceUphill, RemoteControl, Balancing, and SumoProximitySensors.
* 1.0.1 (2015-03-11): Improve the Buttons example.
* 1.0.0 (2015-03-05): Original release.
