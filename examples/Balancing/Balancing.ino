/* This example shows how to use the Zumo 32U4's LSM303D
accelerometer and L3GD20H gyro to balance on its front end.

Please note that the balancing algorithm in this code is not
perfect: the robot tends to drift away from its starting position
over time.  We found that this code works better on carpet than
on a hard surface.

You will have to remove the Zumo's blade if one is installed.
After removing the blade, be sure to reinstall the screws that
were holding the blade.

This code is sensitive to changes in the Zumo's center of mass,
so make sure the LCD is plugged in.

This code was designed for Zumos with 75:1 HP micro metal
gearmotors, and it might need to be adjusted to work on Zumos
with other types of motors. */

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;

L3G gyro;
LSM303 compass;

// This is the average reading obtained from the gyro's Y axis
// during calibration.
float gyroOffsetY;

// This variable holds our estimation of the robot's angle based
// on the gyro and the accelerometer.  A value of 0 means the
// robot is perfectly vertical.  A value of -90 means that the
// robot is horizontal and the battery holder is facing down.  A
// value of 90 means that the robot is horizontal and the battery
// holder is facing up.
float angle = 0;

// This is just like "angle", but it is based solely on the
// accelerometer.
float aAngle = 0;

void setup()
{
  Wire.begin();

  // Set up the L3GD20H gyro.
  gyro.init();

  // 800 Hz output data rate,
  // low-pass filter cutoff 100 Hz.
  gyro.writeReg(L3G::CTRL1, 0b11111111);

  // 2000 dps full scale.
  gyro.writeReg(L3G::CTRL4, 0b00100000);

  // High-pass filter disabled.
  gyro.writeReg(L3G::CTRL5, 0b00000000);

  // Set up the LSM303D accelerometer.
  compass.init();

  // 50 Hz output data rate
  compass.writeReg(LSM303::CTRL1, 0x57);

  // 8 g full-scale
  compass.writeReg(LSM303::CTRL2, 0x18);

  lcd.clear();
  lcd.print(F("Gyro cal"));
  ledYellow(1);

  // Delay to give the user time to remove their finger.
  delay(500);

  // Calibrate the gyro.
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!gyro.readReg(L3G::STATUS_REG) & 0x08);
    gyro.read();

    // Add the Y axis reading to the total.
    gyroOffsetY += gyro.g.y;
  }
  gyroOffsetY /= 1024;

  lcd.clear();
  ledYellow(0);

  // Display the angle until the user presses A.
  while (!buttonA.getSingleDebouncedRelease())
  {
    // Update the angle using the gyro as often as possible.
    updateAngleGyro();

    // Every 20 ms (50 Hz), correct the angle using the
    // accelerometer and also print it.
    static uint8_t lastCorrectionTime = 0;
    uint8_t m = millis();
    if ((uint8_t)(m - lastCorrectionTime) >= 20)
    {
      lastCorrectionTime = m;
      correctAngleAccel();
      printAngles();
    }
  }
  delay(500);
}

void loop()
{
  // Update the angle using the gyro as often as possible.
  updateAngleGyro();

  // Every 20 ms (50 Hz), correct the angle using the
  // accelerometer, print it, and set the motor speeds.
  static byte lastCorrectionTime = 0;
  byte m = millis();
  if ((byte)(m - lastCorrectionTime) >= 20)
  {
    lastCorrectionTime = m;
    correctAngleAccel();
    printAngles();
    setMotors();
  }
}

void printAngles()
{
  lcd.gotoXY(0, 0);
  lcd.print(angle);
  lcd.print(F("  "));

  lcd.gotoXY(0, 1);
  lcd.print(aAngle);
  lcd.print("  ");
}

// Reads the gyro and uses it to update the angle estimation.
void updateAngleGyro()
{
  // Figure out how much time has passed since the last update.
  static uint16_t lastUpdate = 0;
  uint16_t m = micros();
  uint16_t dt = m - lastUpdate;
  lastUpdate = m;

  gyro.read();

  // Calculate how much the angle has changed, in degrees, and
  // add it to our estimation of the current angle.  The gyro's
  // sensitivity is 0.07 dps per digit.
  angle += ((float)gyro.g.y - gyroOffsetY) * 70 * dt / 1000000000;
}

// Reads the accelerometer and uses it to adjust the angle
// estimation.
void correctAngleAccel()
{
  compass.read();

  // Calculate the angle according to the accelerometer.
  aAngle = -atan2(compass.a.z, -compass.a.x) * 180 / M_PI;

  // Calculate the magnitude of the measured acceleration vector,
  // in units of g.
  LSM303::vector<float> const aInG = {
    (float)compass.a.x / 4096,
    (float)compass.a.y / 4096,
    (float)compass.a.z / 4096}
  ;
  float mag = sqrt(LSM303::vector_dot(&aInG, &aInG));

  // Calculate how much weight we should give to the
  // accelerometer reading.  When the magnitude is not close to
  // 1 g, we trust it less because it is being influenced by
  // non-gravity accelerations, so we give it a lower weight.
  float weight = 1 - 5 * abs(1 - mag);
  weight = constrain(weight, 0, 1);
  weight /= 10;

  // Adjust the angle estimation.  The higher the weight, the
  // more the angle gets adjusted.
  angle = weight * aAngle + (1 - weight) * angle;
}

// This function uses our current angle estimation and a PID
// algorithm to set the motor speeds.  This is the core of the
// robot's balancing algorithm.
void setMotors()
{
  const float targetAngle = 2.0;

  int32_t speed;
  if (abs(angle) > 45)
  {
    // If the robot is tilted more than 45 degrees, it is
    // probably going to fall over.  Stop the motors to prevent
    // it from running away.
    speed = 0;
  }
  else
  {
    static float lastError = 0;
    static float integral = 0;

    float error = angle - targetAngle;

    integral += error;
    integral = constrain(integral, -40, 40);

    float errorDifference = error - lastError;
    speed = error * 35 + errorDifference * 10 + integral * 5;
    speed = constrain(speed, -400, 400);

    lastError = error;
  }
  motors.setSpeeds(speed, speed);
}



