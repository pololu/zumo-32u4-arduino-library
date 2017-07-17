// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Zumo32U4Encoders.h>
#include <FastGPIO.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define LEFT_XOR   8
#define LEFT_B     IO_E2
#define RIGHT_XOR  7
#define RIGHT_B    23

static volatile bool lastLeftA;
static volatile bool lastLeftB;
static volatile bool lastRightA;
static volatile bool lastRightB;

static volatile bool errorLeft;
static volatile bool errorRight;

// These count variables are uint16_t instead of int16_t because
// signed integer overflow is undefined behavior in C++.
static volatile uint16_t countLeft;
static volatile uint16_t countRight;

ISR(PCINT0_vect)
{
    bool newLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    bool newLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ newLeftB;

    countLeft += (newLeftA ^ lastLeftB) - (lastLeftA ^ newLeftB);

    if((lastLeftA ^ newLeftA) & (lastLeftB ^ newLeftB))
    {
        errorLeft = true;
    }

    lastLeftA = newLeftA;
    lastLeftB = newLeftB;
}

static void rightISR()
{
    bool newRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    bool newRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ newRightB;

    countRight += (newRightA ^ lastRightB) - (lastRightA ^ newRightB);

    if((lastRightA ^ newRightA) & (lastRightB ^ newRightB))
    {
        errorRight = true;
    }

    lastRightA = newRightA;
    lastRightB = newRightB;
}

void Zumo32U4Encoders::init2()
{
    // Set the pins as pulled-up inputs.
    FastGPIO::Pin<LEFT_XOR>::setInputPulledUp();
    FastGPIO::Pin<LEFT_B>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_XOR>::setInputPulledUp();
    FastGPIO::Pin<RIGHT_B>::setInputPulledUp();

    // Enable pin-change interrupt on PB4 for left encoder, and disable other
    // pin-change interrupts.
    PCICR = (1 << PCIE0);
    PCMSK0 = (1 << PCINT4);
    PCIFR = (1 << PCIF0);  // Clear its interrupt flag by writing a 1.

    // Enable interrupt on PE6 for the right encoder.  We use attachInterrupt
    // instead of defining ISR(INT6_vect) ourselves so that this class will be
    // compatible with other code that uses attachInterrupt.
    attachInterrupt(4, rightISR, CHANGE);

    // Initialize the variables.  It's good to do this after enabling the
    // interrupts in case the interrupts fired by accident as we were enabling
    // them.
    lastLeftB = FastGPIO::Pin<LEFT_B>::isInputHigh();
    lastLeftA = FastGPIO::Pin<LEFT_XOR>::isInputHigh() ^ lastLeftB;
    countLeft = 0;
    errorLeft = 0;

    lastRightB = FastGPIO::Pin<RIGHT_B>::isInputHigh();
    lastRightA = FastGPIO::Pin<RIGHT_XOR>::isInputHigh() ^ lastRightB;
    countRight = 0;
    errorRight = 0;
}

int16_t Zumo32U4Encoders::getCountsLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    sei();
    return counts;
}

int16_t Zumo32U4Encoders::getCountsRight()
{
    init();

    cli();
    int16_t counts = countRight;
    sei();
    return counts;
}

int16_t Zumo32U4Encoders::getCountsAndResetLeft()
{
    init();

    cli();
    int16_t counts = countLeft;
    countLeft = 0;
    sei();
    return counts;
}

int16_t Zumo32U4Encoders::getCountsAndResetRight()
{
    init();

    cli();
    int16_t counts = countRight;
    countRight = 0;
    sei();
    return counts;
}

bool Zumo32U4Encoders::checkErrorLeft()
{
    init();

    bool error = errorLeft;
    errorLeft = 0;
    return error;
}

bool Zumo32U4Encoders::checkErrorRight()
{
    init();

    bool error = errorRight;
    errorRight = 0;
    return error;
}
