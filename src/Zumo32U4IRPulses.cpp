// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Zumo32U4IRPulses.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void Zumo32U4IRPulses::start(Direction direction, uint16_t brightness, uint16_t period)
{
    // Disable Timer 3's interrupts.  This should be done first because another
    // library might be using the timer and its ISR might be modifying timer
    // registers.
    TIMSK3 = 0;

    // Make sure brightness is not larger than period because then the compare
    // match would never happen and the pulse count would always be zero.
    if (brightness > period)
    {
        brightness = period;
    }

    // Set the PWM pin to be an input temporarily.  Otherwise, when we configure
    // the COM3A<1:0> bits, the OC03A signal might be high from previous
    // activity of the timer and result in a glitch on the pin.
    PORTC &= ~(1 << 6);
    DDRC &= ~(1 << 6);

    // Put the timer into a known state that should not cause any trouble while
    // we are reconfiguring it.
    // COM3A<1:0> = 10 : Clear OC3A on match, set at top.
    TCCR3A = (1 << COM3A1);
    TCCR3B = 0;

    // Simulate a compare match, which makes the OC3A PWM signal (which is not
    // connected to the I/O pin yet) go low.  We must do this after configuring
    // COM3A<1:0>.
    TCCR3C = (1 << FOC3A);

    // Make the PWM pin be an output.  The OC03A signal will control its value.
    DDRC |= (1 << 6);

    // Drive PF6/A1 high or low to select which LEDs to use.
    if (direction)
    {
        // Right
        PORTF |= (1 << 6);
    }
    else
    {
        // Left
        PORTF &= ~(1 << 6);
    }
    DDRF |= (1 << 6);

    // Set frequency/period.
    ICR3 = period;

    // Set the count to be one less than ICR3 so that the new duty cycle
    // will take effect very soon.
    TCNT3 = period - 1;

    // Set the duty cycle.
    OCR3A = brightness;

    // Start the timer.  It will start running once the clock source bits
    // in TCCR3B are set.
    //
    // COM3A<1:0> = 10 : Set OC3A on match, clear at top.
    // WGM3<3:0> = 1110 : Fast PWM, with ICR3 as the TOP.
    // CS3<3:0> = 001 : Internal clock with no prescaler
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS30);
}

void Zumo32U4IRPulses::stop()
{
    // Prepare the PWM pin to drive low.  We don't want to just set it as an
    // input because then it might decay from high to low gradually and the
    // LEDs would not turn off immediately.
    PORTC &= ~(1 << 6);
    DDRC |= (1 << 6);

    // Disconnect the PWM signal from the pin, causing it to drive low.  We must
    // do this before stopping the timer to avoid glitches.
    TCCR3A = (1 << WGM31);

    // Turn off the timer.
    TCCR3B = 0;

    // Restore the timer's default settings to help avoid compatibilty issues
    // with other libraries.
    TIMSK3 = 0;
    TCCR3A = 0;
    OCR3A = 0;
    ICR3 = 0;
    TCNT3 = 0;

    // Change the IR LED direction pin (A1) back to an input so it
    // can be used for measuring the battery level.
    DDRF &= ~(1 << 6);
    PORTF &= ~(1 << 6);
}
