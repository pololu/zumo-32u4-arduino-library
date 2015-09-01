/* This example demonstrates three different ways to
interface with a user pushbutton on the Zumo 32U4. */

#include <Wire.h>
#include <Zumo32U4.h>

// These objects provide access to the Zumo 32U4's
// on-board buttons.
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LCD lcd;

void setup()
{
  lcd.clear();
  lcd.print(F("Press A"));

  /* Method 1: Use the waitForButton() function, which blocks and
   * doesn't return until a button press and release are
   * detected. This function takes care of button debouncing. */
  buttonA.waitForButton();

  lcd.clear();
}

void loop()
{
  /* Method 2: Directly read the state of the button with the
   * isPressed() function.  This method is non-blocking and
   * provides no debouncing. */
  if (buttonB.isPressed())
  {
    // Whenever the button is pressed, turn on the yellow LED.
    ledYellow(1);
  }
  else
  {
    // Whenever the button is not pressed, turn off the yellow
    // LED.
    ledYellow(0);
  }

  /* Method 3: Call getSingleDebouncedPress() regularly in a
   * loop, which returns true to report a single button press or
   * false otherwise. This function is non-blocking and takes
   * care of button debouncing. */
  static int cPressedCount = 0;
  if (buttonC.getSingleDebouncedPress())
  {
    cPressedCount += 1;
    Serial.print(F("Button C was pressed "));
    Serial.print(cPressedCount);
    Serial.println(F(" times."));

    lcd.clear();
    lcd.print(cPressedCount);
  }

  /* If you use non-blocking functions like isPressed() and
   * getSingleDebouncedPress(), then you can monitor multiple
   * buttons at the same time and also take care of other tasks
   * at the same time.  In this example, we blink the red LED
   * while monitoring the buttons. */
  ledRed(millis() % 1024 < 100);
}
