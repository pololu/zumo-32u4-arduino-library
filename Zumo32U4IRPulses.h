// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/** \file Zumo32U4IRPulses.h */

#pragma once

#include <stdint.h>

/** \brief Emits pulses of infrared (IR) light using the IR LEDs on the Zumo
32U4 Main Board.

Timer 3 is used to generate a PWM signal, so this library might conflict with
other libraries that use Timer 3.  When the pulses are stopped, Timer 3 can be
used for other purposes.

Pin A1 (PF6) is used to select which set of LEDs to turn on: the left-side LEDs
or the right-side LEDs.

Pin 5 (PC6/OC3A) is used as a PWM output to turn the LEDs on and off.

This class does not do anything with the IR LEDs or detectors on the Zumo 32U4
Front Sensor Array.
 */
class Zumo32U4IRPulses
{
public:

    /** This enum defines the two different sets of LEDs that can be used to
        emit pulses. */
    enum Direction
    {
        /** The LEDs on the left side of the robot. */
        Left = 0,

        /** The LEDs on the right side of the robot. */
        Right = 1
    };

    /** The default frequency is 16000000 / (420 + 1) = 38.005 kHz */
    static const uint16_t defaultPeriod = 420;

    /** \brief Starts emitting IR pulses.
     *
     * \param direction Specifies which set of LEDs to turn on.
     * \param brightness A number that specifies
     *   how long each pulse is.  The pulse length will be
     *   (1 + brightness) / (16 MHz).
     *   If \c brightness is greater than or equal to \c period, then the LEDs
     *   will just be on constantly.
     * \param period A number that specifies the frequency of the pulses.
     *   The interval between consecutive the rising edges of pulses will be
     *   (1 + period) / (16 MHz).
     *   The default value is 420, which results in a period very close to
     *   38 kHz. */
    static void start(Direction direction, uint16_t brightness,
      uint16_t period = defaultPeriod);

    /** \brief Stops emitting IR pulses.
     *
     * Timer 3 can be used for other purposes after calling this
     * function. */
    static void stop();
};
