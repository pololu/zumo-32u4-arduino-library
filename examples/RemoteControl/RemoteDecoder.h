#pragma once

#include <Zumo32U4.h>

// Uncomment the line below to enable debugging output that
// gets sent to the serial monitor.
// #define REMOTE_DEBUG

// Decodes remote control signals that use the NEC protocol:
// http://www.sbprojects.com/knowledge/ir/nec.php
class RemoteDecoder
{
public:
  // In this protocol, every high and low pulse from the IR LED
  // is supposed to have a length that is a multiple of 562 us,
  // and we will call this time "1 time unit".
  static const uint16_t unitPulseTimeUs = 562;

  // The number of bytes in the code sent by the remote.
  static const uint8_t messageSize = 4;

  // States for the low-level state machine that say what
  // part of the protocol we are currently processing.
  //
  // The sensor is inactive during the Idle, StartSpace, and Space states.
  // The sensor is active during the StartMark, Mark, and EndMark states.
  //
  // The expected sequence of states for a normal command is:
  // - Init, Idle, StartMark, StartSpace,
  //   Mark+Space repeated 32 times, EndMark, Idle
  //
  // The expected sequence of states for a "repeat" command is:
  // - Init, Idle, StartMark, StartSpace, EndMark, Idle
  enum State {
    Init,
    Idle,
    StartMark,
    StartSpace,
    Mark,
    Space,
    EndMark,
  };

  RemoteDecoder()
  {
    memset(message, 0, messageSize);
  }

  void init()
  {
    // Enable pull-up resistors on all the sensor inputs.
    FastGPIO::Pin<SENSOR_LEFT>::setInputPulledUp();
    FastGPIO::Pin<SENSOR_RIGHT>::setInputPulledUp();
    FastGPIO::Pin<SENSOR_FRONT>::setInputPulledUp();
  }

  // If false, then it is OK for other parts of the code to do blocking
  // operations that take up to (16 - 3)*562 = 7306 microseconds long.
  // If true, then you should avoid doing any blocking operations, and
  // should call service() as often as possible.
  //
  // If you want to do a long blocking operation (like writing to
  // the LCD), make sure that this function returns false, or you
  // could miss commands sent by the remote.  (There should be
  // practically no risk of receiving false commands though, regardless
  // of the timing of the calls to service().)
  bool criticalTime()
  {
    return state != Idle;
  }

  // This function returns a pointer to the last 4-byte message
  // received from the remote.  The bytes will all be 0 if no
  // message has been received yet.
  const uint8_t * getMessage()
  {
    return message;
  }

  // Returns true once whenever a new message is received.
  // Call getMessage to see the data in the message.
  bool getAndResetMessageFlag()
  {
    if (messageFlag)
    {
      messageFlag = false;
      return true;
    }
    return false;
  }

  // Returns true once whenever a new repeat command is received.
  // This repeat command does not necessarily correspond to the
  // message returned by getMessage.
  bool getAndResetRepeatFlag()
  {
    if (repeatFlag)
    {
      repeatFlag = false;
      return true;
    }
    return false;
  }

  // Returns true once whenever an error happens, which could
  // indicate that the remote is blocked or uses a different
  // protocol.
  bool getAndResetErrorFlag()
  {
    if (errorFlag)
    {
      errorFlag = false;
      return true;
    }
    return false;
  }

  // This should be called as often as possible to monitor the
  // remote.
  void service()
  {
    // First, get the number of time units that have elapsed
    // in the current state.
    uint32_t time = timeInThisState();
    switch(state)
    {
    case Init:
      // Init is the initial state, and also the state used after
      // there are any errors.  Just wait for the signal to turn
      // off and then go to the idle state.
      if (!pulseOn())
      {
        changeState(Idle);
      }
      break;

    case Idle:
      // The sensors are off and we are waiting for the next
      // command.
      if (pulseOn())
      {
        changeState(StartMark);
      }
      break;

    case StartMark:
      // The sensors turned on while we were idle, so we think
      // this is the start mark of command, which is supposed to
      // be 16 units long (9 ms).
      if (time > 16)
      {
        #ifdef REMOTE_DEBUG
        Serial.println(F("Start mark too long"));
        #endif
        error();
      }
      else if (!pulseOn())
      {
        // We allow the start mark to be as short as 2.5 time
        // units, because it is possible that the AVR might be
        // busy with other that cause the start of the start mark
        // to be missed for a while, and the measured time to be
        // shorter than it really is.
        if (time >= 3)
        {
          // Successfully detected the a start mark.
          changeState(StartSpace);
        }
        else
        {
          // This error happens a lot if the signal is bad,
          // because any error will lead to the Idle state, and
          // the remaining marks in the signal will all be
          // treated as if they might be a start mark.
          #ifdef REMOTE_DEBUG
          Serial.print(F("Start mark too short: "));
          #endif
          error();
        }
      }
      break;

    case StartSpace:
      // This is the space after the start mark, which is
      // supposed to be 8 units long for a normal command and 4
      // units long for a "repeat command", a special type of
      // command which has no data and just tells us that the
      // last command is still valid.
      if (time > 8)
      {
        #ifdef REMOTE_DEBUG
        Serial.println(F("Start space too long"));
        #endif
        error();
      }
      else if (pulseOn())
      {
        if (time == 4)
        {
          // This looks like a repeat command so far.
          repeatCommand = true;
          changeState(EndMark);
        }
        else if (time == 8)
        {
          // This looks like a normal command, which will give us
          // a new 32-bit message. Get ready to receive the new
          // message.
          bitsReceived = 0;
          memset(incomingMessage, 0, messageSize);
          changeState(Mark);
        }
        else
        {
          #ifdef REMOTE_DEBUG
          Serial.println(F("Start space bad length"));
          #endif
          error();
        }
      }
      break;

    case Mark:
      // This is a mark, a time when the sensor is active which
      // should last for one unit, and indicates that a new bit
      // of data will be transmitted.  (We won't know the value of
      // that bit of data until later, when we time the length of
      // the space after this mark.)
      if (time > 1)
      {
        #ifdef REMOTE_DEBUG
        Serial.println(F("Mark too long"));
        #endif
        error();
      }
      else if (!pulseOn())
      {
        if (time == 1)
        {
          // Successfully detected a mark.
          changeState(Space);
        }
        else
        {
          #ifdef REMOTE_DEBUG
          Serial.println(F("Mark too short"));
          #endif
          error();
        }
      }
      break;

    case Space:
      // This is a space, a time when the sensor is not active
      // which immediately follows a mark.  The length of this
      // state determines whether the next bit in the command is
      // 0 or 1.
      if (time > 3)
      {
        #ifdef REMOTE_DEBUG
        Serial.println(F("Space too long"));
        #endif
        error();
      }
      else if (pulseOn())
      {
        if (time == 1)
        {
          // The next bit is 0.
          processNewBit(0);
        }
        else if (time == 2)
        {
          #ifdef REMOTE_DEBUG
          Serial.println(F("Invalid space length 2"));
          #endif
          error();
        }
        else if (time == 3)
        {
          // The next bit is 1.
          processNewBit(1);
        }
        else
        {
          #ifdef REMOTE_DEBUG
          Serial.println(F("Space too short"));
          #endif
          error();
        }
      }
      break;

    case EndMark:
      // The last part of any command is an end mark, a time when the
      // sensor is active for one time unit.  This happens at the end of
      // a normal command (which has 32 bits of data) or a "repeat" command
      // which just says to keep doing the previous command.

      if (time > 1)
      {
        #ifdef REMOTE_DEBUG
        Serial.println(F("End mark too long"));
        #endif
        error();
      }
      else if (!pulseOn())
      {
        if (time == 1)
        {
          // Successfully detected the end of a command.

          if (repeatCommand)
          {
            // Successfully received a repeat command.
            repeatFlag = true;

            #ifdef REMOTE_DEBUG
            Serial.println(F("REPEAT COMMAND"));
            #endif
          }
          else
          {
            // Successfully received a normal command that
            // includes a new message.
            memcpy(message, incomingMessage, messageSize);
            messageFlag = true;

            #ifdef REMOTE_DEBUG
            Serial.println(F("NORMAL COMMAND"));
            #endif
          }

          // Go back to the idle state to wait for the next
          // command.
          changeState(Idle);
        }
        else
        {
          #ifdef REMOTE_DEBUG
          Serial.println(F("End mark too short"));
          #endif
          error();
        }
      }
      break;
    }
  }

private:

  // This is called by the low-level state machine at the end of
  // a space state when we have figured out the next bit in the
  // message.  This function records that bit, increments the bit
  // counter, and transitions to the appropriate next state.
  void processNewBit(bool b)
  {
    if (b)
    {
      // The bit is 1, so we need to set the appropriate spot
      // in our incoming message buffer.
      incomingMessage[bitsReceived / 8] |= 1 << bitsReceived % 8;
    }
    bitsReceived++;

    if (bitsReceived == messageSize * 8)
    {
      // We have received a complete message.  Go to the EndMark
      // state so we can measure the time of the mark at the end
      // of the message.
      repeatCommand = false;
      changeState(EndMark);
    }
    else
    {
      // We still need more data, so go to the mark state.
      changeState(Mark);
    }
  }

  // This is called when almost anything goes wrong.
  void error()
  {
    errorFlag = true;
    changeState(Init);
  }

  // Changes to a new state and records the time that the change
  // was made.
  void changeState(State newState)
  {
    state = newState;
    stateStartTimeUs = micros();
  }

  // The number of microseconds we have been in this state.
  uint32_t timeInThisStateUs()
  {
    return (uint32_t)(micros() - stateStartTimeUs);
  }

  // The number of time units we have spent in this state,
  // rounded to the nearest time unit.
  uint32_t timeInThisState()
  {
    return (timeInThisStateUs() + unitPulseTimeUs / 2) / unitPulseTimeUs;
  }

  // Returns true if the IR receiver output is active, which
  // indicates that a 38 kHz IR signal is detected.  This is the
  // code you should change if you want to use an IR receiver on
  // a different pin.
  bool pulseOn()
  {
    return !FastGPIO::Pin<SENSOR_LEFT>::isInputHigh() ||
           !FastGPIO::Pin<SENSOR_RIGHT>::isInputHigh() ||
           !FastGPIO::Pin<SENSOR_FRONT>::isInputHigh();
  }


  /** High-level state variables ********/

  // This gets set to true if a new message has been received,
  // and gets cleared when getAndResetMessageFlag() is called.
  bool messageFlag = false;

  // This gets set to true if a new repeat command has been
  // received, and gets cleared when getAndResetRepeatFlag() is
  // called.
  bool repeatFlag = false;

  // This gets set to true if an error happens, and gets cleared
  // when getAndResetErrorFlag() is called.
  bool errorFlag = false;

  // Holds the last message received.  The first two bytes should
  // be the address, and the second two bytes should be the
  // command.
  uint8_t message[messageSize];


  /** Low-level state variables *********/

  // The overall state of the low-level state machine.
  State state = Init;

  // A buffer is used to hold a command code that has only
  // partially been received.
  uint8_t incomingMessage[messageSize];

  // The number of bits received so far.  This is set before
  // going into the first Mark state.  It is only used during
  // Mark and Space states.
  uint16_t bitsReceived = 0;

  // This is only used in the EndMark state.  It lets us remember
  // whether we are processing the final mark of an actual command
  // or just a repeat command.  Alternatively, we could have just
  // made two states: EndMarkNormal and EndMarkRepeat.
  bool repeatCommand;

  // The time that we entered this state, in microseconds.
  // Gets updated whenever the state is changed.
  uint32_t stateStartTimeUs = 0;
};
