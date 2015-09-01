// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Zumo32U4Encoders.h */

#pragma once

#include <stdint.h>

/*! \brief Reads counts from the encoders on the Zumo 32U4.
 *
 * This class allows you to read counts from the encoders on the Zumo 32U4,
 * which lets you tell how much each motor has turned and in what direction.
 *
 * The encoders are monitored in the background using interrupts, so your code
 * can perform other tasks without missing encoder counts.
 *
 * To read the left encoder, this class uses an interrupt service routine (ISR)
 * for PCINT0_vect, so there will be a compile-time conflict with any other code
 * that defines a pin-change ISR.
 *
 * To read the right encoder, this class calls
 * [attachInterrupt()](http://arduino.cc/en/Reference/attachInterrupt), so there
 * will be a compile-time conflict with any other code that defines an ISR for
 * an external interrupt directly instead of using attachInterrupt(). */
class Zumo32U4Encoders
{

public:

    /*! This function initializes the encoders if they have not been initialized
     *  already and starts listening for counts.  This
     *  function is called automatically whenever you call any other function in
     *  this class, so you should not normally need to call it in your code. */
    static void init()
    {
        static bool initialized = 0;
        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    /*! Returns the number of counts that have been detected from the left-side
     * encoder.  These counts start at 0.  Positive counts correspond to forward
     * movement of the left side of the Zumo, while negative counts correspond
     * to backwards movement.
     *
     * The count is returned as a signed 16-bit integer.  When the count goes
     * over 32767, it will overflow down to -32768.  When the count goes below
     * -32768, it will overflow up to 32767. */
    static int16_t getCountsLeft();

    /*! This function is just like getCountsLeft() except it applies to the
     *  right-side encoder. */
    static int16_t getCountsRight();

    /*! This function is just like getCountsLeft() except it also clears the
     *  counts before returning.  If you call this frequently enough, you will
     *  not have to worry about the count overflowing. */
    static int16_t getCountsAndResetLeft();

    /*! This function is just like getCountsAndResetLeft() except it applies to
     *  the right-side encoder. */
    static int16_t getCountsAndResetRight();

    /*! This function returns true if an error was detected on the left-side
     * encoder.  It resets the error flag automatically, so it will only return
     * true if an error was detected since the last time checkErrorLeft() was
     * called.
     *
     * If an error happens, it means that both of the encoder outputs changed at
     * the same time from the perspective of the ISR, so the ISR was unable to
     * tell what direction the motor was moving, and the encoder count could be
     * inaccurate.  The most likely cause for an error is that the interrupt
     * service routine for the encoders could not be started soon enough.  If
     * you get encoder errors, make sure you are not disabling interrupts for
     * extended periods of time in your code. */
    static bool checkErrorLeft();

    /*! This function is just like checkErrorLeft() except it applies to
     *  the right-side encoder. */
    static bool checkErrorRight();

private:

    static void init2();
};
