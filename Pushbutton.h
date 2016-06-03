// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Pushbutton.h
 *
 * This is the main header file for the %Pushbutton library.
 *
 * For an overview of the library's features, see
 * https://github.com/pololu/pushbutton-arduino.  That is the main repository
 * for the library, though copies of the library may exist in other
 * repositories. */

#pragma once

#include <Arduino.h>

/*! Indicates the that pull-up resistor should be disabled. */
#define PULL_UP_DISABLED    0

/*! Indicates the that pull-up resistor should be enabled. */
#define PULL_UP_ENABLED     1

/*! Indicates that the default (released) state of the button is when the
 *  I/O line reads low. */
#define DEFAULT_STATE_LOW   0

/*! Indicates that the default (released) state of the button is when the
 *  I/O line reads high. */
#define DEFAULT_STATE_HIGH  1

/*! The pin used for the button on the
 * [Zumo Shield for Arduino](http://www.pololu.com/product/2508).
 *
 * This does not really belong here in this general pushbutton library and will
 * probably be removed in the future. */
#define ZUMO_BUTTON 12

// \cond
/** \brief A state machine that detects when a boolean value changes from false
 * to true, with debouncing.
 *
 * This should be considered a private implementation detail of the library.
 */
class PushbuttonStateMachine
{
public:

  PushbuttonStateMachine();

  /** This should be called repeatedly in a loop.  It will return true once after
   * each time it detects the given value changing from false to true. */
  bool getSingleDebouncedRisingEdge(bool value);

private:

  uint8_t state;
  uint16_t prevTimeMillis;
};
// \endcond

/*! \brief General pushbutton class that handles debouncing.
 *
 * This is an abstract class used for interfacing with pushbuttons.  It knows
 * about debouncing, but it knows nothing about how to read the current state of
 * the button.  The functions in this class get the current state of the button
 * by calling isPressed(), a virtual function which must be implemented in a
 * subclass of PushbuttonBase, such as Pushbutton.
 *
 * Most users of this library do not need to directly use PushbuttonBase or even
 * know that it exists.  They can use Pushbutton instead.*/
class PushbuttonBase
{
public:

  /*! \brief Waits until the button is pressed and takes care of debouncing.
   *
   * This function waits until the button is in the pressed state and then
   * returns.  Note that if the button is already pressed when you call this
   * function, it will return quickly (in 10 ms). */
  void waitForPress();

  /*! \brief Waits until the button is released and takes care of debouncing.
   *
   * This function waits until the button is in the released state and then
   * returns.  Note that if the button is already released when you call this
   * function, it will return quickly (in 10 ms). */
  void waitForRelease();

  /*! \brief Waits until the button is pressed and then waits until the button
   *  is released, taking care of debouncing.
   *
   * This is equivalent to calling waitForPress() and then waitForRelease(). */
  void waitForButton();

  /*! \brief Uses a state machine to return true once after each time it detects
   *  the button moving from the released state to the pressed state.
   *
   * This is a non-blocking function that is meant to be called repeatedly in a
   * loop.  Each time it is called, it updates a state machine that monitors the
   * state of the button.  When it detects the button changing from the released
   * state to the pressed state, with debouncing, it returns true. */
  bool getSingleDebouncedPress();

  /*! \brief Uses a state machine to return true once after each time it detects the button moving from the pressed state to the released state.
   *
   * This is just like getSingleDebouncedPress() except it has a separate state
   * machine and it watches for when the button goes from the pressed state to
   * the released state.
   *
   * There is no strict guarantee that every debounced button press event
   * returned by getSingleDebouncedPress() will have a corresponding
   * button release event returned by getSingleDebouncedRelease(); the two
   * functions use independent state machines and sample the button at different
   * times. */
  bool getSingleDebouncedRelease();

  /*! \brief indicates whether button is currently pressed without any debouncing.
   *
   * @return 1 if the button is pressed right now, 0 if it is not.
   *
   * This function must be implemented in a subclass of PushbuttonBase, such as
   * Pushbutton. */
  virtual bool isPressed() = 0;

private:

  PushbuttonStateMachine pressState;
  PushbuttonStateMachine releaseState;
};

/** \brief Main class for interfacing with pushbuttons.
 *
 * This class can interface with any pushbutton whose state can be read with
 * the `digitalRead` function, which is part of the Arduino core.
 *
 * See https://github.com/pololu/pushbutton-arduino for an overview
 * of the different ways to use this class. */
class Pushbutton : public PushbuttonBase
{
public:

  /** Constructs a new instance of Pushbutton.
   *
   * @param pin The pin number of the pin. This is used as an argument to
   * `pinMode` and `digitalRead`.
   *
   * @param pullUp Specifies whether the pin's internal pull-up resistor should
   * be enabled.  This should be either #PULL_UP_ENABLED (which is the default if
   * the argument is omitted) or #PULL_UP_DISABLED.
   *
   * @param defaultState Specifies the voltage level that corresponds to the
   * button's default (released) state.  This should be either
   * #DEFAULT_STATE_HIGH (which is the default if this argument is omitted) or
   * #DEFAULT_STATE_LOW. */
  Pushbutton(uint8_t pin, uint8_t pullUp = PULL_UP_ENABLED,
      uint8_t defaultState = DEFAULT_STATE_HIGH);

  virtual bool isPressed();

private:

  void init()
  {
    if (!initialized)
    {
      initialized = true;
      init2();
    }
  }

  void init2();

  bool initialized;
  uint8_t _pin;
  bool _pullUp;
  bool _defaultState;
};
