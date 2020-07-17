// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Zumo32U4IMU.h */

#pragma once

#include <Zumo32U4IMU.h>

enum class Zumo32U4IMUType : uint8_t {
  Unknown,
  LSM303D_L3GD20H,
  LSM6DS33_LIS3MDL
};

/*! \brief
 */
class Zumo32U4IMU
{

public:

  template <typename T> struct vector
  {
    T x, y, z;
  };

  vector<int16_t> a; // accelerometer readings
  vector<int16_t> g; // gyro readings
  vector<int16_t> m; // magnetometer readings

  uint8_t last_status; // status of last I2C transmission

  bool init();

  void enableDefault();

  Zumo32U4IMUType getType() { return type; }

  void writeReg(uint8_t addr, uint8_t reg, uint8_t value);

  void readAcc();
  void readGyro();
  void readMag();
  void read();

private:

  Zumo32U4IMUType type = Zumo32U4IMUType::Unknown;

  int16_t testReg(uint8_t addr, uint8_t reg);
  void readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v);
};