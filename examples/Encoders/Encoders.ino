/* This program shows how to read the encoders on the Zumo 32U4.
The encoders can tell you how far, and in which direction each
motor has turned.

You can press button A on the Zumo to drive both motors forward
at full speed.  You can press button C to drive both motors
in reverse at full speed.

Encoder counts are printed to the display and to the serial monitor.

On the display, the top line shows the counts from the left encoder,
and the bottom line shows the counts from the right encoder.
Encoder errors should not happen, but if one does happen then the
buzzer will beep and an exclamation mark will appear temporarily
on the display.

In the serial monitor, the first and second numbers represent
counts from the left and right encoders, respectively.  The third
and fourth numbers represent errors from the left and right
encoders, respectively. */

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC;

// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
// Zumo32U4LCD display;
Zumo32U4OLED display;

const char encoderErrorLeft[] PROGMEM = "!<c2";
const char encoderErrorRight[] PROGMEM = "!<e2";

char report[80];

void setup()
{

}

void loop()
{
  static uint8_t lastDisplayTime;
  static uint8_t displayErrorLeftCountdown = 0;
  static uint8_t displayErrorRightCountdown = 0;

  if ((uint8_t)(millis() - lastDisplayTime) >= 100)
  {
    lastDisplayTime = millis();

    int16_t countsLeft = encoders.getCountsLeft();
    int16_t countsRight = encoders.getCountsRight();

    bool errorLeft = encoders.checkErrorLeft();
    bool errorRight = encoders.checkErrorRight();

    if (errorLeft)
    {
      // An error occurred on the left encoder channel.
      // Show it on the display for the next 10 iterations and
      // also beep.
      displayErrorLeftCountdown = 10;
      buzzer.playFromProgramSpace(encoderErrorLeft);
    }

    if (errorRight)
    {
      // An error occurred on the right encoder channel.
      // Show it on the display for the next 10 iterations and
      // also beep.
      displayErrorRightCountdown = 10;
      buzzer.playFromProgramSpace(encoderErrorRight);
    }

    // Update the display with encoder counts and error info.
    display.clear();
    display.print(countsLeft);
    display.gotoXY(0, 1);
    display.print(countsRight);

    if (displayErrorLeftCountdown)
    {
      // Show an exclamation point on the first line to
      // indicate an error from the left encoder.
      display.gotoXY(7, 0);
      display.print('!');
      displayErrorLeftCountdown--;
    }

    if (displayErrorRightCountdown)
    {
      // Show an exclamation point on the second line to
      // indicate an error from the left encoder.
      display.gotoXY(7, 1);
      display.print('!');
      displayErrorRightCountdown--;
    }

    // Send the information to the serial monitor also.
    snprintf_P(report, sizeof(report),
        PSTR("%6d %6d %1d %1d"),
        countsLeft, countsRight, errorLeft, errorRight);
    Serial.println(report);
  }

  if (buttonA.isPressed())
  {
    motors.setSpeeds(400, 400);
  }
  else if (buttonC.isPressed())
  {
    motors.setSpeeds(-400, -400);
  }
  else
  {
    motors.setSpeeds(0, 0);
  }
}
