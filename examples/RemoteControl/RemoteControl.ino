/* This example shows how to use the receivers for the infrared
proximity sensors on the Zumo 32U4 to detect and decode commands
from an infrared remote.  This code is designed to work with the
Mini IR Remote Control available from Pololu:

  https://www.pololu.com/product/2777

For this code to work, jumpers on the front sensor array should
be installed in order to connect pin 4 to RGT and connect pin 20
to LFT.

The arrow buttons control the robot's movement, while the number
buttons from 1 to 3 play different notes on the buzzer.

To change what actions are performed when a button press is
detected, you should change the processRemoteCommand and
stopCurrentCommand functions.

If you have a different remote that uses the NEC protocol with a
38 kHz, 940 nm infrared emitter, it should be possible to make it
work with this code.  You can use this code to decode the
messages from your remote, and then you can edit the constants in
RemoteConstants.h to match what was transmitted from your
remote. */

#include <Wire.h>
#include <Zumo32U4.h>
#include "RemoteConstants.h"
#include "RemoteDecoder.h"

// This variable sets the amount of time (in milliseconds) that
// we wait before considering the current message from the remote
// to have expired.  This type of remote typically sends a repeat
// command every 109 ms, so a timeout value of 115 was chosen.
// You can increase this timeout to 230 if you want to be more
// tolerant of errors that occur while you are holding down the
// button, but it will make the robot slower to respond when you
// release the button.
const uint16_t messageTimeoutMs = 115;

// This variable is true if the last message received from the
// remote is still active, meaning that we are still performing
// the action specified by the message.  A message will be active
// if the remote button is being held down and the robot has been
// successfully receiving messages from the remote since the
// button was pressed.
bool messageActive = false;

// This is the time that the current message from the remote was
// last verified, in milliseconds.  It is used to implement the
// timeout defined above.
uint16_t lastMessageTimeMs = 0;

Zumo32U4LCD lcd;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;

RemoteDecoder decoder;

void setup()
{
  decoder.init();

  lcd.clear();
  lcd.print(F("Waiting"));
}

void loop()
{
  decoder.service();

  // Turn on the yellow LED if a message is active.
  ledYellow(messageActive);

  // Turn on the red LED if we are in the middle of receiving a
  // new message from the remote.  You should see the red LED
  // blinking about 9 times per second while you hold a remote
  // button down.
  ledRed(decoder.criticalTime());

  if (decoder.criticalTime())
  {
    // We are in the middle of receiving a message from the
    // remote, so we should avoid doing anything that might take
    // more than a few tens of microseconds, and call
    // decoder.service() as often as possible.
  }
  else
  {
    // We are not in a critical time, so we can do other things
    // as long as they do not take longer than about 7.3 ms.
    // Delays longer than that can cause some remote control
    // messages to be missed.

    processRemoteEvents();
  }

  // Check how long ago the current message was last verified.
  // If it is longer than the timeout time, then the message has
  // expired and we should stop executing it.
  if (messageActive && (uint16_t)(millis() - lastMessageTimeMs) > messageTimeoutMs)
  {
    messageActive = false;
    stopCurrentCommand();
  }
}

void processRemoteEvents()
{
  if (decoder.getAndResetMessageFlag())
  {
    // The remote decoder received a new message, so record what
    // time it was received and process it.
    lastMessageTimeMs = millis();
    messageActive = true;
    processRemoteMessage(decoder.getMessage());
  }

  if (decoder.getAndResetRepeatFlag())
  {
    // The remote decoder receiver a "repeat" command, which is
    // sent about every 109 ms while the button is being held
    // down.  It contains no data.  We record what time the
    // repeat command was received so we can know that the
    // current message is still active.
    lastMessageTimeMs = millis();
  }
}

void processRemoteMessage(const uint8_t * message)
{
  // Print the raw message on the first line of the LCD, in hex.
  // The first two bytes are usually an address, and the third
  // byte is usually a command.  The last byte is supposed to be
  // the bitwise inverse of the third byte, and if that is the
  // case, then we don't print it.
  lcd.clear();
  char buffer[9];
  if (message[2] + message[3] == 0xFF)
  {
    sprintf(buffer, "%02X%02X %02X ",
      message[0], message[1], message[2]);
  }
  else
  {
    sprintf(buffer, "%02X%02X%02X%02X",
      message[0], message[1], message[2], message[3]);
  }
  lcd.print(buffer);

  // Go to the next line of the LCD.
  lcd.gotoXY(0, 1);

  // Make sure the address matches what we expect.
  if (message[0] != remoteAddressByte0 ||
    message[1] != remoteAddressByte1)
  {
    lcd.print(F("bad addr"));
    return;
  }

  // Make sure that the last byte is the logical inverse of the
  // command byte.
  if (message[2] + message[3] != 0xFF)
  {
    lcd.print(F("bad cmd"));
    return;
  }

  stopCurrentCommand();
  processRemoteCommand(message[2]);
}

// Start running the new command.
void processRemoteCommand(uint8_t command)
{
  switch(command)
  {
  case remoteUp:
    lcd.print(F("up"));
    motors.setSpeeds(400, 400);
    break;

  case remoteDown:
    lcd.print(F("down"));
    motors.setSpeeds(-400, -400);
    break;

  case remoteLeft:
    lcd.print(F("left"));
    motors.setSpeeds(-250, 250);
    break;

  case remoteRight:
    lcd.print(F("right"));
    motors.setSpeeds(250, -250);
    break;

  case remoteStopMode:
    lcd.print(F("stop"));
    break;

  case remoteEnterSave:
    lcd.print(F("enter"));
    break;

  case remoteVolMinus:
    lcd.print(F("vol-"));
    break;

  case remoteVolPlus:
    lcd.print(F("vol+"));
    break;

  case remotePlayPause:
    lcd.print(F("play"));
    break;

  case remoteSetup:
    lcd.print(F("setup"));
    break;

  case remoteBack:
    lcd.print(F("back"));
    break;

  case remote0:
    lcd.print(F("0"));
    break;

  case remote1:
    lcd.print(F("1"));
    buzzer.playNote(NOTE_C(4), 200, 15);
    break;

  case remote2:
    lcd.print(F("2"));
    buzzer.playNote(NOTE_D(4), 200, 15);
    break;

  case remote3:
    lcd.print(F("3"));
    buzzer.playNote(NOTE_E(4), 200, 15);
    break;

  case remote4:
    lcd.print(F("4"));
    break;

  case remote5:
    lcd.print(F("5"));
    break;

  case remote6:
    lcd.print(F("6"));
    break;

  case remote7:
    lcd.print(F("7"));
    break;

  case remote8:
    lcd.print(F("8"));
    break;

  case remote9:
    lcd.print(F("9"));
    break;
  }
}

// Stops the current remote control command.  This is called when
// a new command is received or if the current command has
// expired.
void stopCurrentCommand()
{
  motors.setSpeeds(0, 0);
  buzzer.stopPlaying();
}
