/* This example demonstrates basic use of the Zumo 32U4 display.
It prints the word "hi" on the first line of the display and
prints the number 1234 on the second line. */

#include <Wire.h>
#include <Zumo32U4.h>

// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
// Zumo32U4LCD display;
Zumo32U4OLED display;

void setup()
{

}

void loop()
{
  // Clear the screen
  display.clear();

  // Print a string
  display.print("hi");

  // Go to the next line
  display.gotoXY(0, 1);

  // Print a number
  display.print(1234);

  delay(1000);
}

