#include <Wire.h>
#include <Zumo32U4IMU.h>

#define TEST_REG_ERROR -1

#define LSM303D_WHO_ID  0x49
#define L3GD20H_WHO_ID  0xD7
#define LSM6DS33_WHO_ID 0x69
#define LIS3MDL_WHO_ID  0x3D

bool Zumo32U4IMU::init()
{
  if (testReg(LSM303D_ADDR, LSM303D_REG_WHO_AM_I) == LSM303D_WHO_ID &&
      testReg(L3GD20H_ADDR, L3GD20H_REG_WHO_AM_I) == L3GD20H_WHO_ID)
  {
    type = Zumo32U4IMUType::LSM303D_L3GD20H;
    return true;
  }
  else if (testReg(LSM6DS33_ADDR, LSM6DS33_REG_WHO_AM_I) == LSM6DS33_WHO_ID &&
           testReg( LIS3MDL_ADDR,  LIS3MDL_REG_WHO_AM_I) ==  LIS3MDL_WHO_ID)
  {
    type = Zumo32U4IMUType::LSM6DS33_LIS3MDL;
    return true;
  }
  else
  {
    return false;
  }
}

void Zumo32U4IMU::enableDefault()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:

    // Accelerometer

    // 0x57 = 0b01010111
    // AODR = 0101 (50 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL1, 0x57);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // AFS = 0 (+/- 2 g full scale)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL2, 0x00);
    if (lastError) { return; }

    // Magnetometer

    // 0x64 = 0b01100100
    // M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL5, 0x64);
    if (lastError) { return; }

    // 0x20 = 0b00100000
    // MFS = 01 (+/- 4 gauss full scale)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL6, 0x20);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL7, 0x00);
    if (lastError) { return; }

    // Gyro

    // 0x7F = 0b01111111
    // DR = 01 (189.4 Hz ODR); BW = 11 (70 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL1, 0x7F);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // FS = 00 (+/- 245 dps full scale)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL4, 0x00);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:

    // Accelerometer

    // 0x30 = 0b00110000
    // ODR = 0011 (52 Hz (high performance)); FS_XL = 00 (+/- 2 g full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL1_XL, 0x30);
    if (lastError) { return; }

    // Gyro

    // 0x50 = 0b01010000
    // ODR = 0101 (208 Hz (high performance)); FS_G = 00 (+/- 245 dps full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x50);
    if (lastError) { return; }

    // Accelerometer + Gyro

    // 0x04 = 0b00000100
    // IF_INC = 1 (automatically increment register address)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL3_C, 0x04);
    if (lastError) { return; }

    // Magnetometer

    // 0x70 = 0b01110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG1, 0x70);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG2, 0x00);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG3, 0x00);
    if (lastError) { return; }

    // 0x0C = 0b00001100
    // OMZ = 11 (ultra-high-performance mode for Z)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG4, 0x0C);
    return;
  }
}

void Zumo32U4IMU::configureForBalancing()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:

    // Accelerometer

    // 0x18 = 0b00011000
    // AFS = 0 (+/- 2 g full scale)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL2, 0x18);
    if (lastError) { return; }

    // Gyro

    // 0xFF = 0b11111111
    // DR = 11 (757.6 Hz ODR); BW = 11 (100 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL1, 0xFF);
    if (lastError) { return; }

    // 0x20 = 0b00100000
    // FS = 10 (+/- 2000 dps full scale)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL4, 0x20);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:

    // Accelerometer

    // 0x3C = 0b00111100
    // ODR = 0011 (52 Hz (high performance)); FS_XL = 11 (+/- 8 g full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL1_XL, 0x3C);
    if (lastError) { return; }

    // Gyro

    // 0x7C = 0b01111100
    // ODR = 0111 (833 Hz (high performance)); FS_G = 11 (+/- 2000 dps full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x7C);
    return;
  }
}

void Zumo32U4IMU::configureForTurnSensing()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:

    // Gyro

    // 0xFF = 0b11111111
    // DR = 11 (757.6 Hz ODR); BW = 11 (100 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL1, 0xFF);
    if (lastError) { return; }

    // 0x20 = 0b00100000
    // FS = 10 (+/- 2000 dps full scale)
    writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL4, 0x20);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:

    // Gyro

    // 0x7C = 0b01111100
    // ODR = 0111 (833 Hz (high performance)); FS_G = 11 (+/- 2000 dps full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x7C);
    return;
  }
}

void Zumo32U4IMU::configureForFaceUphill()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:

    // Accelerometer

    // 0x37 = 0b00110111
    // AODR = 0011 (12.5 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
    writeReg(LSM303D_ADDR, LSM303D_REG_CTRL1, 0x37);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:

    // Accelerometer

    // 0x10 = 0b00010000
    // ODR = 0001 (13 Hz (high performance)); FS_XL = 00 (+/- 2 g full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL1_XL, 0x10);
    return;
  }
}

void Zumo32U4IMU::writeReg(uint8_t addr, uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  lastError = Wire.endTransmission();
}

uint8_t Zumo32U4IMU::readReg(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  lastError = Wire.endTransmission();
  if (lastError) { return 0; }

  uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
  if (byteCount != 1)
  {
    lastError = 50;
    return 0;
  }
  return Wire.read();
}

// Reads the 3 accelerometer channels and stores them in vector a
void Zumo32U4IMU::readAcc(void)
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    // set MSB of register address for auto-increment
    readAxes16Bit(LSM303D_ADDR, LSM303D_REG_OUT_X_L_A | (1 << 7), a);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    // assumes register address auto-increment is enabled (IF_INC in CTRL3_C)
    readAxes16Bit(LSM6DS33_ADDR, LSM6DS33_REG_OUTX_L_XL, a);
    return;
  }
}

// Reads the 3 gyro channels and stores them in vector g
void Zumo32U4IMU::readGyro()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    // set MSB of register address for auto-increment
    readAxes16Bit(L3GD20H_ADDR, L3GD20H_REG_OUT_X_L | (1 << 7), g);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    // assumes register address auto-increment is enabled (IF_INC in CTRL3_C)
    readAxes16Bit(LSM6DS33_ADDR, LSM6DS33_REG_OUTX_L_G, g);
    return;
  }
}

// Reads the 3 magnetometer channels and stores them in vector m
void Zumo32U4IMU::readMag()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    // set MSB of register address for auto-increment
    readAxes16Bit(LSM303D_ADDR, LSM303D_REG_OUT_X_L_M | (1 << 7), m);
    return;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    // set MSB of register address for auto-increment
    readAxes16Bit(LIS3MDL_ADDR, LIS3MDL_REG_OUT_X_L | (1 << 7), m);
    return;
  }
}

// Reads all 9 accelerometer, gyro, and magnetometer channels and stores them
// in the respective vectors
void Zumo32U4IMU::read()
{
  readAcc();
  if (lastError) { return; }
  readGyro();
  if (lastError) { return; }
  readMag();
}

bool Zumo32U4IMU::accDataReady()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    return readReg(LSM303D_ADDR, LSM303D_REG_STATUS_A) & 0x08;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    return readReg(LSM6DS33_ADDR, LSM6DS33_REG_STATUS_REG) & 0x01;
  }
  return false;
}

bool Zumo32U4IMU::gyroDataReady()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    return readReg(L3GD20H_ADDR, L3GD20H_REG_STATUS) & 0x08;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    return readReg(LSM6DS33_ADDR, LSM6DS33_REG_STATUS_REG) & 0x02;
  }
  return false;
}

bool Zumo32U4IMU::magDataReady()
{
  switch (type)
  {
  case Zumo32U4IMUType::LSM303D_L3GD20H:
    return readReg(LSM303D_ADDR, LSM303D_REG_STATUS_M) & 0x08;

  case Zumo32U4IMUType::LSM6DS33_LIS3MDL:
    return readReg(LIS3MDL_ADDR, LIS3MDL_REG_STATUS_REG) & 0x08;
  }
  return false;
}

int16_t Zumo32U4IMU::testReg(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
  {
    return TEST_REG_ERROR;
  }

  uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
  if (byteCount != 1)
  {
    return TEST_REG_ERROR;
  }
  return Wire.read();
}

void Zumo32U4IMU::readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v)
{
  Wire.beginTransmission(addr);
  Wire.write(firstReg);
  lastError = Wire.endTransmission();
  if (lastError) { return; }

  uint8_t byteCount = (Wire.requestFrom(addr, (uint8_t)6));
  if (byteCount != 6)
  {
    lastError = 50;
    return;
  }
  uint8_t xl = Wire.read();
  uint8_t xh = Wire.read();
  uint8_t yl = Wire.read();
  uint8_t yh = Wire.read();
  uint8_t zl = Wire.read();
  uint8_t zh = Wire.read();

  // combine high and low bytes
  v.x = (int16_t)(xh << 8 | xl);
  v.y = (int16_t)(yh << 8 | yl);
  v.z = (int16_t)(zh << 8 | zl);
}