// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include "Pushbutton.h"

// \cond

PushbuttonStateMachine::PushbuttonStateMachine()
{
  state = 0;
}

// state 0: The value is considered to be true.
// state 1: The value was considered to be true, but there
//   was a recent false reading so it might be falling.
// state 2: The value is considered to be false.
// state 3: The value was considered to be false, but there
//   was a recent true reading so it might be rising.
//
// The transition from state 3 to state 0 is the point where we
// have successfully detected a rising edge an we return true.
//
// The prevTimeMillis variable holds the last time that we
// transitioned to states 1 or 3.
bool PushbuttonStateMachine::getSingleDebouncedRisingEdge(bool value)
{
  uint16_t timeMillis = millis();

  switch (state)
  {
  case 0:
    // If value is false, proceed to the next state.
    if (!value)
    {
      prevTimeMillis = timeMillis;
      state = 1;
    }
    break;

  case 1:
    if (value)
    {
      // The value is true or bouncing, so go back to previous (initial)
      // state.
      state = 0;
    }
    else if ((uint16_t)(timeMillis - prevTimeMillis) >= 15)
    {
      // It has been at least 15 ms and the value is still false, so
      // proceed to the next state.
      state = 2;
    }
    break;

  case 2:
    // If the value is true, proceed to the next state.
    if (value)
    {
      prevTimeMillis = timeMillis;
      state = 3;
    }
    break;

  case 3:
    if (!value)
    {
      // The value is false or bouncing, so go back to previous state.
      state = 2;
    }
    else if ((uint16_t)(timeMillis - prevTimeMillis) >= 15)
    {
      // It has been at least 15 ms and the value is still true, so
      // go back to the initial state and report this rising edge.
      state = 0;
      return true;
    }
    break;
  }

  return false;
}

// \endcond

void PushbuttonBase::waitForPress()
{
  do
  {
    while (!isPressed()); // wait for button to be pressed
    delay(10);            // debounce the button press
  }
  while (!isPressed());   // if button isn't still pressed, loop
}

void PushbuttonBase::waitForRelease()
{
  do
  {
    while (isPressed()); // wait for button to be released
    delay(10);           // debounce the button release
  }
  while (isPressed());   // if button isn't still released, loop
}

void PushbuttonBase::waitForButton()
{
  waitForPress();
  waitForRelease();
}

bool PushbuttonBase::getSingleDebouncedPress()
{
  return pressState.getSingleDebouncedRisingEdge(isPressed());
}

bool PushbuttonBase::getSingleDebouncedRelease()
{
  return releaseState.getSingleDebouncedRisingEdge(!isPressed());
}

Pushbutton::Pushbutton(uint8_t pin, uint8_t pullUp, uint8_t defaultState)
{
  initialized = false;
  _pin = pin;
  _pullUp = pullUp;
  _defaultState = defaultState;
}

void Pushbutton::init2()
{
  if (_pullUp == PULL_UP_ENABLED)
  {
    pinMode(_pin, INPUT_PULLUP);
  }
  else
  {
    pinMode(_pin, INPUT); // high impedance
  }

  delayMicroseconds(5); // give pull-up time to stabilize
}

bool Pushbutton::isPressed()
{
  init();  // initialize if needed
  return digitalRead(_pin) != _defaultState;
}
