/* This example shows how to read RC pulses from pins 0 and 1 and
use them to control the motors on the Zumo 32U4.

Pins 0 and 1 are LCD control lines, so you cannot use the LCD
while running this demo. */

#include <Wire.h>
#include <Zumo32U4.h>

#define THROTTLE_PIN 0  // Throttle channel from RC receiver.
#define STEERING_PIN 1  // Steering channel from RC receiver.

// Maximum motor speed.
#define MAX_SPEED 400

// Pulse width difference from 1500 us (microseconds) to ignore,
// to compensate for control centering offset.
#define PULSE_WIDTH_DEADBAND 40

// Pulse width difference from 1500 us to be treated as full
// scale input.  For example, a value of 350 means any pulse
// width <= 1150 us or >= 1850 us is considered full scale.
#define PULSE_WIDTH_RANGE 350

Zumo32U4Motors motors;

void setup()
{
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
}

void loop()
{
  // Read one pulse from each pin.
  int throttle = pulseIn(THROTTLE_PIN, HIGH);
  int steering = pulseIn(STEERING_PIN, HIGH);

  int leftSpeed, rightSpeed;

  if (throttle > 0 && steering > 0)
  {
    // Both RC signals are good.  Turn on the yellow LED.
    ledYellow(1);

    // RC signals encode information in pulse width centered on
    // 1500 us; subtract 1500 to get a value centered on 0.
    throttle -= 1500;
    steering -= 1500;

    // Apply deadband.
    if (abs(throttle) <= PULSE_WIDTH_DEADBAND)
    {
      throttle = 0;
    }
    if (abs(steering) <= PULSE_WIDTH_DEADBAND)
    {
      steering = 0;
    }

    // Convert from pulse widths to speeds.
    int throttleSpeed = (long)throttle * MAX_SPEED / PULSE_WIDTH_RANGE;
    int steeringSpeed = (long)steering * MAX_SPEED / PULSE_WIDTH_RANGE;

    // Mix throttle and steering inputs to obtain left & right
    // motor speeds.
    leftSpeed = throttleSpeed - steeringSpeed;
    rightSpeed = throttleSpeed + steeringSpeed;

    // Cap the speeds to their maximum values.
    leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
    rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);
  }
  else
  {
    // At least one RC signal is not good.  Turn off the LED and
    // stop motors.
    ledYellow(0);

    leftSpeed = 0;
    rightSpeed = 0;
  }

  motors.setSpeeds(leftSpeed, rightSpeed);
}
