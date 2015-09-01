/* GridMovement.h and GridMovement.cpp provide functions to help
the Zumo navigate a grid made of black lines on a white surface.

The code uses the line sensors to follow lines and detect
intersections, and it uses the gyro to help perform turns.

This code was designed with maze-solving in mind, but it can be
used in other applications as long as there are black lines on a
white surface, and the lines are not too close together, and the
lines intersect at right angles.

The speed and delay parameters in this file were designed for a
Zumo 32U4 with 4 NiMH batteries and 75:1 HP gearmotors.  They
might need to be adjusted depending on your motor and battery
voltage. */

#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

// Motor speed when driving straight.  400 is the max speed.
const uint16_t straightSpeed = 200;

// The delay to use between first detecting an intersection and
// starting to turn.  During this time, the robot drives
// straight.  Ideally this delay would be just long enough to get
// the robot from the point where it detected the intersection to
// the center of the intersection.
const uint16_t intersectionDelay = 130;

// Motor speed when turning.  400 is the max speed.
const uint16_t turnSpeed = 200;

// Motor speed when turning during line sensor calibration.
const uint16_t calibrationSpeed = 200;

// This line sensor threshold is used to detect intersections.
const uint16_t sensorThreshold = 200;

// This line sensor threshold is used to detect the end of the
// maze.
const uint16_t sensorThresholdDark = 600;

// The number of line sensors we are using.
const uint8_t numSensors = 5;

// For angles measured by the gyro, our convention is that a
// value of (1 << 29) represents 45 degrees.  This means that a
// uint32_t can represent any angle between 0 and 360.
const int32_t gyroAngle45 = 0x20000000;

// Functions and variables defined in GridMovement.cpp:
void printBar(uint8_t barHeight);
uint16_t readSensors();
bool aboveLine(uint8_t sensorIndex);
bool aboveLineDark(uint8_t sensorIndex);
bool aboveDarkSpot();
extern uint16_t lineSensorValues[numSensors];
void turn(char dir);
void followSegment();
void driveToIntersectionCenter();
void driveToIntersectionCenter(bool * foundLeft, bool * foundStraight, bool * foundRight);
void gridMovementSetup();

// These must be defined by the sketch:
extern L3G gyro;
extern Zumo32U4ButtonA buttonA;
extern Zumo32U4Motors motors;
extern Zumo32U4LCD lcd;
extern Zumo32U4LineSensors lineSensors;
