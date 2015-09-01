/* Turnsensor.h and TurnSensor.cpp provide functions for
configuring the L3GD20H gyro, calibrating it, and using it to
measure how much the robot has turned about its Z axis. */

#pragma once

#include <Zumo32U4.h>

// This constant represents a turn of 45 degrees.
const int32_t turnAngle45 = 0x20000000;

// This constant represents a turn of 90 degrees.
const int32_t turnAngle90 = turnAngle45 * 2;

// This constant represents a turn of approximately 1 degree.
const int32_t turnAngle1 = (turnAngle45 + 22) / 45;

// These are defined in TurnSensor.cpp:
void turnSensorSetup();
void turnSensorReset();
void turnSensorUpdate();
extern uint32_t turnAngle;
extern int16_t turnRate;

// These objects must be defined in your sketch.
extern Zumo32U4ButtonA buttonA;
extern Zumo32U4LCD lcd;
extern L3G gyro;
