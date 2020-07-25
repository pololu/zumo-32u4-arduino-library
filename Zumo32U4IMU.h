// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Zumo32U4IMU.h */

#pragma once

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define LSM303D_ADDR  0b0011101
#define L3GD20H_ADDR  0b1101011
#define LSM6DS33_ADDR 0b1101011
#define LIS3MDL_ADDR  0b0011110


#define LSM303D_REG_STATUS_M  0x07
#define LSM303D_REG_OUT_X_L_M 0x08
#define LSM303D_REG_WHO_AM_I  0x0F
#define LSM303D_REG_CTRL1     0x20
#define LSM303D_REG_CTRL2     0x21
#define LSM303D_REG_CTRL5     0x24
#define LSM303D_REG_CTRL6     0x25
#define LSM303D_REG_CTRL7     0x26
#define LSM303D_REG_STATUS_A  0x27
#define LSM303D_REG_OUT_X_L_A 0x28

#define L3GD20H_REG_WHO_AM_I 0x0F
#define L3GD20H_REG_CTRL1    0x20
#define L3GD20H_REG_CTRL4    0x23
#define L3GD20H_REG_STATUS   0x27
#define L3GD20H_REG_OUT_X_L  0x28

#define LSM6DS33_REG_WHO_AM_I   0x0F
#define LSM6DS33_REG_CTRL1_XL   0x10
#define LSM6DS33_REG_CTRL2_G    0x11
#define LSM6DS33_REG_CTRL3_C    0x12
#define LSM6DS33_REG_STATUS_REG 0x1E
#define LSM6DS33_REG_OUTX_L_G   0x22
#define LSM6DS33_REG_OUTX_L_XL  0x28

#define LIS3MDL_REG_WHO_AM_I   0x0F
#define LIS3MDL_REG_CTRL_REG1  0x20
#define LIS3MDL_REG_CTRL_REG2  0x21
#define LIS3MDL_REG_CTRL_REG3  0x22
#define LIS3MDL_REG_CTRL_REG4  0x23
#define LIS3MDL_REG_STATUS_REG 0x27
#define LIS3MDL_REG_OUT_X_L    0x28


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

  Zumo32U4IMUType getType() { return type; }

  void enableDefault();

  void configureForTurnSensing();
  void configureForBalancing();
  void configureForFaceUphill();

  void writeReg(uint8_t addr, uint8_t reg, uint8_t value);
  uint8_t readReg(uint8_t addr, uint8_t reg);

  void readAcc();
  void readGyro();
  void readMag();
  void read();

  bool accDataReady();
  bool gyroDataReady();
  bool magDataReady();

private:

  Zumo32U4IMUType type = Zumo32U4IMUType::Unknown;

  int16_t testReg(uint8_t addr, uint8_t reg);
  void readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v);
};