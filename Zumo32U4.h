// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Zumo32U4.h
 *
 * \brief Main header file for the Zumo32U4 library.
 *
 * This file includes all the other headers files provided by the library.
 */

#pragma once

#ifndef __AVR_ATmega32U4__
#error "This library only supports the ATmega32U4.  Try selecting A-Star 32U4 in the Boards menu."
#endif

#include <FastGPIO.h>
#include <LSM303.h>
#include <L3G.h>
#include <Zumo32U4Buttons.h>
#include <Zumo32U4Buzzer.h>
#include <Zumo32U4Encoders.h>
#include <Zumo32U4IRPulses.h>
#include <Zumo32U4LCD.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4ProximitySensors.h>

// TODO: servo support

/*! \brief Turns the red user LED (RX) on or off.

@param on 1 to turn on the LED, 0 to turn it off.

The red user LED is on pin 17, which is also known as PB0, SS, and RXLED.  The
Arduino core code uses this LED to indicate when it receives data over USB, so
it might be hard to control this LED when USB is connected. */
inline void ledRed(bool on)
{
    FastGPIO::Pin<17>::setOutput(!on);
}

/*! \brief Turns the yellow user LED on pin 13 on or off.

@param on 1 to turn on the LED, 0 to turn it off. */
inline void ledYellow(bool on)
{
    FastGPIO::Pin<13>::setOutput(on);
}

/*! \brief Turns the green user LED (TX) on or off.

@param on 1 to turn on the LED, 0 to turn it off.

The green user LED is pin PD5, which is also known as TXLED.  The Arduino core
code uses this LED to indicate when it receives data over USB, so it might be
hard to control this LED when USB is connected. */
inline void ledGreen(bool on)
{
    FastGPIO::Pin<IO_D5>::setOutput(!on);
}

/*! \brief Returns true if USB power is detected.

This function returns true if power is detected on the board's USB port and
returns false otherwise.  It uses the ATmega32U4's VBUS line, which is directly
connected to the power pin of the USB connector.

\sa A method for detecting whether the board's virtual COM port is open:
  http://arduino.cc/en/Serial/IfSerial */
inline bool usbPowerPresent()
{
    return USBSTA >> VBUS & 1;
}

/*! \brief Reads the battery voltage and returns it in millivolts. */
inline uint16_t readBatteryMillivolts()
{
    const uint8_t sampleCount = 8;
    uint16_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++)
    {
        sum += analogRead(A1);
    }

    // VBAT = 2 * millivolt reading = 2 * raw * 5000/1024
    //      = raw * 625 / 64
    // The correction term below makes it so that we round to the
    // nearest whole number instead of always rounding down.
    const uint32_t correction = 32 * sampleCount - 1;
    return ((uint32_t)sum * 625 + correction) / (64 * sampleCount);
}

/**

\class LSM303
\brief Interfaces with the accelerometer.

For complete documentation of the this class, see
https://github.com/pololu/lsm303-arduino


\class L3G
\brief Interfaces with the gyro.

For complete documentation of this class, see
https://github.com/pololu/l3g-arduino


\class QTRSensors
For complete documentation of this class, see
https://www.pololu.com/docs/0J19


\class QTRSensorsRC
For complete documentation of this class, see
https://www.pololu.com/docs/0J19

*/
