/// \file QTRSensors.h

/*
  QTRSensors.h - Library for using Pololu QTR reflectance
    sensors and reflectance sensor arrays: QTR-1A, QTR-8A, QTR-1RC, and
    QTR-8RC.  The object used will determine the type of the sensor (either
    QTR-xA or QTR-xRC).  Then simply specify in the constructor which
    Arduino I/O pins are connected to a QTR sensor, and the read() method
    will obtain reflectance measurements for those sensors.  Smaller sensor
    values correspond to higher reflectance (e.g. white) while larger
    sensor values correspond to lower reflectance (e.g. black or a void).

    * QTRSensorsRC should be used for QTR-1RC and QTR-8RC sensors.
    * QTRSensorsAnalog should be used for QTR-1A and QTR-8A sensors.
*/

/*
 * Written by Ben Schmidel et al., October 4, 2010
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J19
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */

#ifndef QTRSensors_h
#define QTRSensors_h

/// \defgroup read_modes Read modes
///
/// @{

/// \brief Specifies that the reading should be made without turning on the infrared
/// (IR) emitters.
///
/// The reading represents ambient light levels
/// near the sensor.
#define QTR_EMITTERS_OFF 0

/// \brief Specifies that the emitters should be turned on for the reading.
///
/// This results in a measure of reflectance.
#define QTR_EMITTERS_ON 1

/// \brief Specifies that a reading should be made in both the on and off states.
///
/// The values returned when this option is used are given by *on + max – off*,
/// where *on* is the reading with the emitters on, *off* is the reading with
/// the emitters off, and *max* is the maximum sensor reading. This option can
/// reduce the amount of interference from uneven ambient lighting. Note that
/// emitter control will only work if you specify a valid emitter pin in the
/// constructor.
#define QTR_EMITTERS_ON_AND_OFF 2
/// @}

#define QTR_NO_EMITTER_PIN  255

#define QTR_MAX_SENSORS 16

// This class cannot be instantiated directly (it has no constructor).
// Instead, you should instantiate one of its two derived classes (either the
// QTR-A or QTR-RC version, depending on the type of your sensor).
class QTRSensors
{
  public:

    /// \brief Reads the raw sensor values into an array.
    ///
    /// \param[out] sensor_values A pointer to an array in which to store the
    /// raw sensor readings. There *MUST* be space for as many values as
    /// there were sensors specified in the constructor.
    ///
    /// \param readMode The emitter behavior during the read (see \ref
    /// read_modes). The default is `QTR_EMITTERS_ON`.
    ///
    /// Example usage:
    /// ~~~{.cpp}
    /// unsigned int sensor_values[8];
    /// sensors.read(sensor_values);
    /// ~~~
    ///
    /// The values returned are a measure of the reflectance in abstract units,
    /// with higher values corresponding to lower reflectance (e.g. a black
    /// surface or a void).
    ///
    /// If `measureOffAndOn` is true, measures the values with the emitters on
    /// AND off and returns on - (timeout - off).  If this value is less than
    /// zero, it returns zero.
    ///
    /// This method will call the appropriate derived class's readPrivate(),
    /// which is defined as a virtual function in the base class and overridden
    /// by each derived class's own implementation.
    void read(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON);

    /// \brief Turns the IR LEDs off.
    ///
    ///  This is mainly for use by the read method, and calling this function
    ///  before or after the reading the sensors will have no effect on the
    ///  readings, but you may wish to use it for testing purposes.
    void emittersOff();

    /// \brief Turns the IR LEDs on.
    ///
    ///  This is mainly for use by the read method, and calling this function
    ///  before or after the reading the sensors will have no effect on the
    ///  readings, but you may wish to use it for testing purposes.
    void emittersOn();

    /// \brief Reads the sensors for calibration.
    ///
    /// The sensor values are not returned; instead, the maximum and minimum
    /// values found over time are stored internally and used for the
    /// readCalibrated() method.
    void calibrate(unsigned char readMode = QTR_EMITTERS_ON);

    /// \brief Resets all calibration that has been done.
    void resetCalibration();

    /// \brief Reads the sensors and provides calibrated values between 0 and
    /// 1000.
    ///
    /// \param[out] sensor_values A pointer to an array in which to store the
    /// calibrated sensor readings. There *MUST* be space for as many values as
    /// there were sensors specified in the constructor.
    ///
    /// \param readMode The emitter behavior during the read (see \ref
    /// read_modes). The default is `QTR_EMITTERS_ON`.
    ///
    /// 0 corresponds to the minimum value read by calibrate() and 1000
    /// corresponds to the maximum value.  Calibration values are
    /// stored separately for each sensor, so that differences in the
    /// sensors are accounted for automatically.
    void readCalibrated(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON);

    /// \brief Reads the sensors, provides calibrated values, and returns an
    /// estimated line position.
    ///
    /// \param[out] sensor_values A pointer to an array in which to store the
    /// raw sensor readings. There *MUST* be space for as many values as
    /// there were sensors specified in the constructor.
    ///
    /// \param readMode The emitter behavior during the read (see \ref
    /// read_modes). The default is `QTR_EMITTERS_ON`.
    ///
    /// \param white_line By default, this function assumes a dark line (high
    /// values) surrounded by white (low values).  If your line is light on
    /// black, set the optional argument `white_line` to true.  In this case,
    /// each sensor value will be replaced by (1000-value) before the averaging.
    ///
    /// This function operates the same as readCalibrated(), but also returns an
    /// estimated position of the robot with respect to a line. The
    /// estimate is made using a weighted average of the sensor indices
    /// multiplied by 1000, so that a return value of 0 indicates that
    /// the line is directly below sensor 0, a return value of 1000
    /// indicates that the line is directly below sensor 1, 2000
    /// indicates that it's below sensor 2000, etc.  Intermediate
    /// values indicate that the line is between two sensors.  The
    /// formula is (where \f$v_0\f$ represents the value from the
    /// first sensor):
    ///
    /// \f[
    /// {(0 \times v_0) + (1000 \times v_1) + (2000 \times v_2) + \cdots
    /// \over
    /// v_0 + v_1 + v_2 + \cdots}
    /// \f]
    int readLine(unsigned int *sensor_values, unsigned char readMode = QTR_EMITTERS_ON, unsigned char white_line = 0);


    /// \name Calibrated minumum and maximum values
    ///
    /// These start at 1000 and 0, respectively, so that the very first sensor
    /// reading will update both of them.
    ///
    /// The pointers are unallocated until calibrate() is called, and then
    /// allocated to exactly the size required.  Depending on the readMode
    /// argument to calibrate, only the On or Off values may be allocated, as
    /// required.
    ///
    /// These variables are made public so that you can use them for your own
    /// calculations and do things like saving the values to EEPROM, performing
    /// sanity checking, etc.
    ///
    /// @{
    unsigned int *calibratedMinimumOn;
    unsigned int *calibratedMaximumOn;
    unsigned int *calibratedMinimumOff;
    unsigned int *calibratedMaximumOff;
    /// @}

    ~QTRSensors();

  protected:

    QTRSensors()
    {

    };

    void init(unsigned char *pins, unsigned char numSensors, unsigned char emitterPin);

    unsigned char *_pins;
    unsigned char _numSensors;
    unsigned char _emitterPin;
    unsigned int _maxValue; // the maximum value returned by this function
    int _lastValue;

  private:

    virtual void readPrivate(unsigned int *sensor_values) = 0;

    // Handles the actual calibration. calibratedMinimum and
    // calibratedMaximum are pointers to the requested calibration
    // arrays, which will be allocated if necessary.
    void calibrateOnOrOff(unsigned int **calibratedMinimum,
                          unsigned int **calibratedMaximum,
                          unsigned char readMode);
};



// Object to be used for QTR-1RC and QTR-8RC sensors
class QTRSensorsRC : public QTRSensors
{
  public:

    // if this constructor is used, the user must call init() before using
    // the methods in this class
    QTRSensorsRC();

    // this constructor just calls init()
    QTRSensorsRC(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout = 4000, unsigned char emitterPin = 255);

    // The array 'pins' contains the Arduino pin number for each sensor.

    // 'numSensors' specifies the length of the 'pins' array (i.e. the
    // number of QTR-RC sensors you are using).  numSensors must be
    // no greater than 16.

    // 'timeout' specifies the length of time in microseconds beyond
    // which you consider the sensor reading completely black.  That is to say,
    // if the pulse length for a pin exceeds 'timeout', pulse timing will stop
    // and the reading for that pin will be considered full black.
    // It is recommended that you set timeout to be between 1000 and
    // 3000 us, depending on things like the height of your sensors and
    // ambient lighting.  Using timeout allows you to shorten the
    // duration of a sensor-reading cycle while still maintaining
    // useful analog measurements of reflectance

    // 'emitterPin' is the Arduino pin that controls the IR LEDs on the 8RC
    // modules.  If you are using a 1RC (i.e. if there is no emitter pin),
    // or if you just want the emitters on all the time and don't want to
    // use an I/O pin to control it, use a value of 255 (QTR_NO_EMITTER_PIN).
    void init(unsigned char* pins, unsigned char numSensors,
          unsigned int timeout = 2000, unsigned char emitterPin = QTR_NO_EMITTER_PIN);



  private:

    // Reads the sensor values into an array. There *MUST* be space
    // for as many values as there were sensors specified in the constructor.
    // Example usage:
    // unsigned int sensor_values[8];
    // sensors.read(sensor_values);
    // The values returned are a measure of the reflectance in microseconds.
    void readPrivate(unsigned int *sensor_values);
};



// Object to be used for QTR-1A and QTR-8A sensors
class QTRSensorsAnalog : public QTRSensors
{
  public:

    // if this constructor is used, the user must call init() before using
    // the methods in this class
    QTRSensorsAnalog();

    // this constructor just calls init()
    QTRSensorsAnalog(unsigned char* pins,
        unsigned char numSensors, unsigned char numSamplesPerSensor = 4,
        unsigned char emitterPin = 255);

    // the array 'pins' contains the Arduino analog pin assignment for each
    // sensor.  For example, if pins is {0, 1, 7}, sensor 1 is on
    // Arduino analog input 0, sensor 2 is on Arduino analog input 1,
    // and sensor 3 is on Arduino analog input 7.

    // 'numSensors' specifies the length of the 'analogPins' array (i.e. the
    // number of QTR-A sensors you are using).  numSensors must be
    // no greater than 16.

    // 'numSamplesPerSensor' indicates the number of 10-bit analog samples
    // to average per channel (i.e. per sensor) for each reading.  The total
    // number of analog-to-digital conversions performed will be equal to
    // numSensors*numSamplesPerSensor.  Note that it takes about 100 us to
    // perform a single analog-to-digital conversion, so:
    // if numSamplesPerSensor is 4 and numSensors is 6, it will take
    // 4 * 6 * 100 us = ~2.5 ms to perform a full readLine().
    // Increasing this parameter increases noise suppression at the cost of
    // sample rate.  The recommended value is 4.

    // 'emitterPin' is the Arduino pin that controls the IR LEDs on the 8RC
    // modules.  If you are using a 1RC (i.e. if there is no emitter pin),
    // or if you just want the emitters on all the time and don't want to
    // use an I/O pin to control it, use a value of 255 (QTR_NO_EMITTER_PIN).
    void init(unsigned char* analogPins, unsigned char numSensors,
        unsigned char numSamplesPerSensor = 4, unsigned char emitterPin = QTR_NO_EMITTER_PIN);



  private:

    // Reads the sensor values into an array. There *MUST* be space
    // for as many values as there were sensors specified in the constructor.
    // Example usage:
    // unsigned int sensor_values[8];
    // sensors.read(sensor_values);
    // The values returned are a measure of the reflectance in terms of a
    // 10-bit ADC average with higher values corresponding to lower
    // reflectance (e.g. a black surface or a void).
    void readPrivate(unsigned int *sensor_values);

    unsigned char _numSamplesPerSensor;
};


#endif
