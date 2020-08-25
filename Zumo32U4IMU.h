// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Zumo32U4IMU.h */

#pragma once

/*! \anchor device_addresses
 *
 * \name Device Addresses
 * @{
 */
#define LSM303D_ADDR  0b0011101
#define L3GD20H_ADDR  0b1101011
#define LSM6DS33_ADDR 0b1101011
#define LIS3MDL_ADDR  0b0011110
/*! @} */

/*! \anchor register_addresses
 *
 * \name Register Addresses
 * @{
 */
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
/*! @} */

/*! \brief The type of the inertial sensors. */
enum class Zumo32U4IMUType : uint8_t {
  /*! Unknown or unrecognized */
  Unknown,
  /*! LSM303D accelerometer + magnetometer, L3GD20H gyro */
  LSM303D_L3GD20H,
  /*! LSM6DS33 gyro + accelerometer, LIS3MDL magnetometer */
  LSM6DS33_LIS3MDL
};

/*! \brief Interfaces with the inertial sensors on the Zumo 32U4.
 *
 * This class allows you to configure and get readings from the I2C sensors that
 * make up the Zumo 32U4's inertial measurement unit (IMU): gyro, accelerometer,
 * and magnetometer.
 *
 * You must call `Wire.start()` before using any of this library's functions
 * that access the sensors. */
class Zumo32U4IMU
{
public:

  /*! \brief Represents a 3-dimensional vector with x, y, and z
   * components. */
  template <typename T> struct vector
  {
    T x, y, z;
  };

  /*! \brief Raw accelerometer readings. */
  vector<int16_t> a = {0, 0, 0};

  /*! \brief Raw gyro readings. */
  vector<int16_t> g = {0, 0, 0};

  /*! \brief Raw magnetometer readings. */
  vector<int16_t> m = {0, 0, 0};

  /*! \brief Returns 0 if the last I2C communication with the IMU was
   * successful, or a non-zero status code if there was an error. */
  uint8_t getLastError() { return lastError; }

  /*! \brief Initializes the inertial sensors and detects their type.
   *
   *  \return True if the sensor type was detected succesfully; false otherwise.
   */
  bool init();

  /*! \brief Returns the type of the inertial sensors on the Zumo 32U4.
   *
   * \return The sensor type as a member of the Zumo32U4IMUType enum. If the
   * type is not known (e.g. if init() has not been called yet), this will be
   * Zumo32U4IMUType::Unknown. */
  Zumo32U4IMUType getType() { return type; }

  /*! \brief Enables all of the inertial sensors with a default configuration.
   */
  void enableDefault();

  /*! \brief Configures the sensors with settings optimized for turn sensing. */
  void configureForTurnSensing();

  /*! \brief Configures the sensors with settings optimized for balancing. */
  void configureForBalancing();

  /*! \brief Configures the sensors with settings optimized for the FaceUphill
   * example program. */
  void configureForFaceUphill();

  /*! \brief Writes an 8-bit sensor register.
   *
   * \param addr Device address.
   * \param reg Register address.
   * \param value 8-bit register value to be written. */
  void writeReg(uint8_t addr, uint8_t reg, uint8_t value);

  /*! \brief Reads an 8-bit sensor register.
   *
   * \param addr Device address.
   * \param reg Register address.
   *
   * \return 8-bit register value read from the device. */
  uint8_t readReg(uint8_t addr, uint8_t reg);

  /*! \brief Takes a reading from the accelerometer and makes the measurements
   * available in #a. */
  void readAcc();

  /*! \brief Takes a reading from the gyro and makes the measurements available
   * in #g. */
  void readGyro();

    /*! \brief Takes a reading from the magnetometer and makes the measurements
   * available in #m. */
  void readMag();

    /*! \brief Takes a reading from all three sensors (accelerometer, gyro, and
     * magnetometer) and makes their measurements available in the respective
     * vectors. */
  void read();

  /*! \brief Indicates whether the accelerometer has new measurement data ready.
   *
   * \return True if there is new accelerometer data available; false otherwise.
   */
  bool accDataReady();

  /*! \brief Indicates whether the gyro has new measurement data ready.
   *
   * \return True if there is new gyro data available; false otherwise.
   */
  bool gyroDataReady();

  /*! \brief Indicates whether the magnetometer has new measurement data ready.
   *
   * \return True if there is new magnetometer data available; false otherwise.
   */
  bool magDataReady();

private:

  uint8_t lastError = 0;
  Zumo32U4IMUType type = Zumo32U4IMUType::Unknown;

  int16_t testReg(uint8_t addr, uint8_t reg);
  void readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v);
};