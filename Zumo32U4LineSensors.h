/** \file Zumo32U4LineSensors.h **/

#pragma once

#include <QTRSensors.h>

/** \brief The pin number for the standard pin that is used to read line sensor
 * 1, the left-most sensor. */
static const uint8_t SENSOR_DOWN1 = 18;

/** \brief The pin number for the standard pin that is used to read line sensor
 * 2, the sensor second from the left. */
static const uint8_t SENSOR_DOWN2 = 20;

/** \brief The pin number for standard pin that is used to read line sensor 3,
 * the middle sensor. */
static const uint8_t SENSOR_DOWN3 = 21;

/** \brief The pin number for the standard pin that is used to read line sensor
 * 4, the sensor second from the right. */
static const uint8_t SENSOR_DOWN4 = 4;

/** \brief The pin number for the standard pin that is used to read line sensor
 * 5, the right-most sensor. */
static const uint8_t SENSOR_DOWN5 = 12;

/** \brief The pin number for the standard pin that is used to enable or disable
 * the IR emitters of the line sensors. */
static const uint8_t SENSOR_LEDON = 11;

/** \brief Gets readings from the five down-facing line sensors on the front
 * sensor array.
 *
 * The functions that this class inherits from QTRSensorsRC, such as read() and
 * calibrate(), are documented in the
 * [user's guide for the %QTRSensors library](https://www.pololu.com/docs/0J19).
 * %QTRSensors is a separate Arduino library with a [separate GitHub
 * repository](https://github.com/pololu/qtr-sensors-arduino), but we include a
 * copy of it in the Zumo32U4 library since it is needed for the
 * Zumo32U4LineSensors class.
 *
 */
class Zumo32U4LineSensors : public QTRSensorsRC
{
public:

    /** \brief Minimal constructor.
     *
     * If you use this (i.e. by not providing any arguments when you create the
     * Zumo32U4ProximitySensors object), then you will have to call
     * initThreeSensors(), initFiveSensors(), or init() before using the
     * functions in this class. */
    Zumo32U4LineSensors() { }

    /** \brief Constructor that takes pin arguments.
     *
     * This constructor calls init() with the specified arguments. */
    Zumo32U4LineSensors(uint8_t * pins, uint8_t numSensors,
        uint8_t emitterPin = SENSOR_LEDON)
    {
        init(pins, numSensors, emitterPin);
    }

    /** \brief Configures this object to use just three line sensors.
     *
     * This function configures this object to just use line sensors 1, 3, and
     * 5. */
    void initThreeSensors(uint8_t emitterPin = SENSOR_LEDON)
    {
        uint8_t pins[] = { SENSOR_DOWN1, SENSOR_DOWN3, SENSOR_DOWN5 };
        init(pins, sizeof(pins), 2000, emitterPin);
    }

    /** \brief Configures this object to use all five line sensors.
     *
     * This function configures this object to use all five line sensors.
     *
     * For this configuration to work, jumpers on the front sensor array must be
     * installed in order to connect pin 20 to DN2 and connect pin 4 to DN4. */
    void initFiveSensors(uint8_t emitterPin = SENSOR_LEDON)
    {
        uint8_t pins[] = { SENSOR_DOWN1, SENSOR_DOWN2, SENSOR_DOWN3,
                           SENSOR_DOWN4, SENSOR_DOWN5 };
        init(pins, sizeof(pins), 2000, emitterPin);
    }

    /** \brief Configures this object to use a custom set of pins.
     *
     * \param pins A pointer to an array with the pin numbers for the sensors.
     * \param numSensors The number of sensors.
     * \param timeout Specifies the length of time in microseconds beyond
     *   which you consider the sensor reading completely black.
     * \param emitterPin The number of the pin that controls the
     *   emitters for the line sensors.  You can specify a value of
     *   QTR_NO_EMITTER_PIN for this parameter if you want this object to not do
     *   anything to the emitters. */
    void init(uint8_t * pins, uint8_t numSensors,
        uint16_t timeout = 2000, uint8_t emitterPin = SENSOR_LEDON)
    {
        QTRSensorsRC::init(pins, numSensors, timeout, emitterPin);
    }
};

