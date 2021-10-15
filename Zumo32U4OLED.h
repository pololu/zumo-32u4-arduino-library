// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#pragma once

#include <Arduino.h>
#include <FastGPIO.h>
#include <USBPause.h>
#include <PololuSH1106Main.h>
#include <util/delay.h>

// This asm inside the macro below is an optimized version of
//   FastGPIO::Pin<mosPin>::setOutputValue(d >> b & 1);
// It prevents the compiler from using slow branches to implement the
// conditional logic, and also ensures that the writing speed doesn't depend
// on the data.
#define _P3PP_OLED_SEND_BIT(b) \
  FastGPIO::Pin<clkPin>::setOutputValueLow(); \
  asm volatile( \
    "sbrc %2, %3\n" "sbi %0, %1\n" \
    "sbrs %2, %3\n" "cbi %0, %1\n" \
    : : \
    "I" (FastGPIO::pinStructs[mosPin].portAddr - __SFR_OFFSET), \
    "I" (FastGPIO::pinStructs[mosPin].bit), \
    "r" (d), \
    "I" (b)); \
  FastGPIO::Pin<clkPin>::setOutputValueHigh();

/// @brief Low-level functions for writing data to the SH1106 OLED on the
/// Pololu Zumo 32U4 OLED robot.
class Zumo32U4OLEDCore
{
  // Pin assignments
  static const uint8_t clkPin = 1, mosPin = IO_D5, resPin = 0, dcPin = 17;

public:
  void initPins()
  {
    FastGPIO::Pin<clkPin>::setOutputLow();
  }

  void reset()
  {
    FastGPIO::Pin<resPin>::setOutputLow();
    _delay_us(10);
    FastGPIO::Pin<resPin>::setOutputHigh();
    _delay_us(10);
  }

  void sh1106TransferStart()
  {
    // From https://github.com/pololu/usb-pause-arduino/blob/master/USBPause.h:
    // Disables USB interrupts because the Arduino USB interrupts use some of
    // the OLED pins.
    savedUDIEN = UDIEN;
    UDIEN = 0;
    savedUENUM = UENUM;
    UENUM = 0;
    savedUEIENX0 = UEIENX;
    UEIENX = 0;

    savedStateMosi = FastGPIO::Pin<mosPin>::getState();
    savedStateDc = FastGPIO::Pin<dcPin>::getState();

    FastGPIO::Pin<mosPin>::setOutputLow();
  }

  void sh1106TransferEnd()
  {
    FastGPIO::Pin<mosPin>::setState(savedStateMosi);
    FastGPIO::Pin<dcPin>::setState(savedStateDc);

    // From https://github.com/pololu/usb-pause-arduino/blob/master/USBPause.h
    UENUM = 0;
    UEIENX = savedUEIENX0;
    UENUM = savedUENUM;
    UDIEN = savedUDIEN;
  }

  void sh1106CommandMode()
  {
    FastGPIO::Pin<dcPin>::setOutputLow();
  }

  void sh1106DataMode()
  {
    FastGPIO::Pin<dcPin>::setOutputHigh();
  }

  void sh1106Write(uint8_t d)
  {
    _P3PP_OLED_SEND_BIT(7);
    _P3PP_OLED_SEND_BIT(6);
    _P3PP_OLED_SEND_BIT(5);
    _P3PP_OLED_SEND_BIT(4);
    _P3PP_OLED_SEND_BIT(3);
    _P3PP_OLED_SEND_BIT(2);
    _P3PP_OLED_SEND_BIT(1);
    _P3PP_OLED_SEND_BIT(0);
  }

private:
  uint8_t savedStateMosi, savedStateDc;
  uint8_t savedUDIEN, savedUENUM, savedUEIENX0;
};

/// @brief Makes it easy to show text and graphics on the SH1106 OLED of
/// the Pololu Zumo 32U4 OLED robot.
///
/// This class inherits from the PololuSH1106Main class in the PololuOLED
/// library, which provides almost all of its functionality.  See the
/// [PololuOLED library documentation](https://pololu.github.io/pololu-oled-arduino/)
/// for more information about how to use this class.
///
/// This class also inherits from the Arduino Print class
/// (via PololuSH1106Main), so you can call the `print()` function on it with a
/// variety of arguments.  See the
/// [Arduino print() documentation](http://arduino.cc/en/Serial/Print) for
/// more information.
class Zumo32U4OLED : public PololuSH1106Main<Zumo32U4OLEDCore>
{
};
