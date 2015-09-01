/* This code for the Zumo 32U4 will solve a line maze constructed
with a black line on a white background, as long as there are no
loops and all intersections are at right angles.  It has two
phases: first, it learns the maze with a left-hand-on-the-wall
strategy and computes the most efficient path to the finish.
Second, it follows the previously-computed path.

This code is meant to run on a Zumo with 4 NiMH batteries and two
75:1 HP micro metal gearmotors.  If you have different batteries
or motors, you will probably need to adjust the timing and speed
parameters in GridMovement.h.

== Hardware setup ==

This demo requires a Zumo 32U4 Front Sensor Array to be
connected, and jumpers on the front sensor array must be
installed in order to connect pin 4 to DN4 and pin 20 to DN2.

== Usage ==

First, place your Zumo on the maze.  You should place it so that
the line sensors are approximately centered above one of the
black lines.

Next, turn on the Zumo, and be careful to not move it for a few
seconds afterwards while the gyro is being calibrated.  During
the gyro calibration, the yellow LED is on and the words "Gyro
cal" are displayed on the LCD.

After the gyro calibration is done, press button A to start the
line sensor calibration.  The robot will turn left 90 degrees,
return to center, turn right 90 degrees, and then return
approximately to center.  If the angles are incorrect or the
robot turns in circles, then something is probably wrong with the
gyro and you should power off and try again.

The robot's position might shift during calibration.  If none of
the inner three line sensors can see the line, as indicated on
the LCD, you should reposition it to center the sensors on the
line.

Press button A.  The robot will explore the maze, searching for a
large dark spot which indicates the end and computing the optimal
path to reach it.  When it reaches the end, it will beep.

Move the robot back to the starting position, and press button A
again.  The robot will go directly to the end of the maze using
the path it previously learned. */

#include <Wire.h>
#include <Zumo32U4.h>
#include "GridMovement.h"

Zumo32U4LCD lcd;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
L3G gyro;

// The path variable will store the path that the robot has
// taken.  It is stored as an array of characters, each of which
// represents the turn that should be made at one intersection in
// the sequence:
//  'L' for left
//  'R' for right
//  'S' for straight (going straight through an intersection)
//  'B' for back (U-turn)
//
// The simplifyPath() function checks for dead ends and removes
// them from the path every time the robot makes a turn.
char path[100];
uint8_t pathLength = 0;

void setup()
{
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  buzzer.playFromProgramSpace(PSTR("!>g32>>c32"));

  gridMovementSetup();

  // mazeSolve() explores every segment of the maze until it
  // finds the optimal path.
  mazeSolve();
}

void loop()
{
  // The maze has been solved.  Wait for the user to press a button.
  buttonA.waitForButton();

  // Follow the simplified path computed earlier to reach the end
  // of the maze.
  mazeFollowPath();
}

// This function decides which way to turn during the learning
// phase of maze solving.  It uses the variables found_left,
// found_straight, and found_right, which indicate whether there
// is an exit in each of the three directions, applying the
// left-hand-on-the-wall strategy.
char selectTurn(bool foundLeft, bool foundStraight, bool foundRight)
{
  // Make a decision about how to turn.  The following code
  // implements a left-hand-on-the-wall strategy, where we always
  // turn as far to the left as possible.
  if(foundLeft) { return 'L'; }
  else if(foundStraight) { return 'S'; }
  else if(foundRight) { return 'R'; }
  else { return 'B'; }
}

// The mazeSolve() function works by applying a
// left-hand-on-the-wall strategy: the robot follows a segment
// until it reaches an intersection, where it takes the leftmost
// fork available to it. It records each turn it makes, and as
// long as the maze has no loops, this strategy will eventually
// lead it to the finish. Afterwards, the recorded path is
// simplified by removing dead ends. More information can be
// found in the 3pi maze solving example.
void mazeSolve()
{
  // Start with an empty path.  We will add and remove entries to
  // the path as we solve the maze.
  pathLength = 0;

  // Play a starting tune.
  buzzer.playFromProgramSpace(PSTR("!L16 cdegreg4"));

  // Delay so the robot does not move while the user is still
  // touching the button.
  delay(1000);

  while(1)
  {
    // Navigate current line segment until we enter an intersection.
    followSegment();

    // Drive stright forward to get to the center of the
    // intersection and check for exits to the left, right, and
    // straight ahead.
    bool foundLeft, foundStraight, foundRight;
    driveToIntersectionCenter(&foundLeft, &foundStraight, &foundRight);

    if(aboveDarkSpot())
    {
      // We found the end of the maze, so we succeeded in solving
      // the maze.
      break;
    }

    // Choose a direction to turn.
    char dir = selectTurn(foundLeft, foundStraight, foundRight);

    // Make sure we don't overflow the pathLength buffer,
    // which could lead to unpredictable behavior of the
    // robot.
    if (pathLength >= sizeof(path))
    {
      lcd.clear();
      lcd.print(F("pathfull"));
      while(1)
      {
        ledRed(1);
      }
    }

    // Store the intersection in the path variable.
    path[pathLength] = dir;
    pathLength++;

    // Simplify the learned path.
    simplifyPath();

    // Display the path on the LCD.
    displayPath();

    // If the path is equal to "BB", it means we have searched the
    // whole maze and not found the path.  We beep but
    // continue searching, because maybe the sensors missed
    // something earlier.
    if (pathLength == 2 && path[0] == 'B' && path[1] == 'B')
    {
      buzzer.playFromProgramSpace(PSTR("!<b4"));
    }

    // Make the turn.
    turn(dir);
  }

  // We have solved the maze and found an optimal path.  Stop the
  // motors and play a note with the buzzer.
  motors.setSpeeds(0, 0);
  buzzer.playFromProgramSpace(PSTR("!>>a32"));
}

// Re-run the maze, following the recorded path.  We need to
// detect intersections, but it is not necessary to identify all
// the exits from them, so this function is simpler than
// mazeSolve.
void mazeFollowPath()
{
  // Play a starting note.
  buzzer.playFromProgramSpace(PSTR("!>c32"));

  // Delay so the robot does not move while the user is still
  // touching the button.
  delay(1000);

  for(uint16_t i = 0; i < pathLength; i++)
  {
    // Follow a line segment until we get to the center of an
    // intersection.
    followSegment();
    driveToIntersectionCenter();

    // Make a turn according to the instruction stored in
    // path[i].
    turn(path[i]);
  }

  // Follow the last segment.
  followSegment();

  // The end of the path has been reached.  Stop the motors and
  // play a note with the buzzer.
  motors.setSpeeds(0, 0);
  buzzer.playFromProgramSpace(PSTR("!>>a32"));
}

// Path simplification.  The strategy is that whenever we
// encounter a sequence xBx, we can simplify it by cutting out
// the dead end.  For example, LBL -> S, because a single S
// bypasses the dead end represented by LBL.
void simplifyPath()
{
  // Only simplify the path if it is at least three instructions
  // long and the second-to-last turn was a 'B'.
  if(pathLength < 3 || path[pathLength - 2] != 'B')
  {
    return;
  }

  int16_t totalAngle = 0;

  for(uint8_t i = 1; i <= 3; i++)
  {
    switch(path[pathLength - i])
    {
    case 'L':
      totalAngle += 90;
      break;
    case 'R':
      totalAngle -= 90;
      break;
    case 'B':
      totalAngle += 180;
      break;
    }
  }

  // Get the angle as a number between 0 and 360 degrees.
  totalAngle = totalAngle % 360;

  // Replace all of those turns with a single one.
  switch(totalAngle)
  {
  case 0:
    path[pathLength - 3] = 'S';
    break;
  case 90:
    path[pathLength - 3] = 'L';
    break;
  case 180:
    path[pathLength - 3] = 'B';
    break;
  case 270:
    path[pathLength - 3] = 'R';
    break;
  }

  // The path is now two steps shorter.
  pathLength -= 2;
}

void displayPath()
{
  // Set the last character of the path to a 0 so that the print()
  // function can find the end of the string.  This is how strings
  // are normally terminated in C.
  path[pathLength] = 0;

  lcd.clear();
  lcd.print(path);
  if(pathLength > 8)
  {
    lcd.gotoXY(0, 1);
    lcd.print(path + 8);
  }
}
