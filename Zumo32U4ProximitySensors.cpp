// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <Zumo32U4ProximitySensors.h>
#include <Zumo32U4IRPulses.h>
#include <Arduino.h>
#include <FastGPIO.h>
#include <stdlib.h>

// The Arduino's digitalRead, digitalWrite, and pinMode functions will have
// unexpected behavior if the pin number argument happens to be greater than
// NUM_DIGITAL_PINS.  We protect users of our library from that.
static bool digitalReadSafe(uint8_t pin, bool defaultValue = 1)
{
    if (pin < NUM_DIGITAL_PINS)
    {
        return digitalRead(pin);
    }
    else
    {
        return defaultValue;
    }
}

static void pinModeSafe(uint8_t pin, uint8_t mode)
{
    if (pin < NUM_DIGITAL_PINS)
    {
        pinMode(pin, mode);
    }
}

void Zumo32U4ProximitySensors::clearAll()
{
    dataArray = NULL;
    numSensors = 0;

    lineSensorEmitterPin = SENSOR_NO_PIN;

    levelsArray = NULL;
    numLevels = 0;

    pulseOnTimeUs = defaultPulseOnTimeUs;
    pulseOffTimeUs = defaultPulseOffTimeUs;
    period = defaultPeriod;
}

void Zumo32U4ProximitySensors::init(uint8_t * pins, uint8_t numSensors,
    uint8_t lineSensorEmitterPin)
{
    this->lineSensorEmitterPin = lineSensorEmitterPin;

    // Allocate memory for the new dataArray.  If there is an error, return
    // before modifying the existing dataArray or numSensors so that we
    // don't leave the object in an invalid state.
    const size_t size = numSensors * sizeof(SensorData);
    void * newArray = realloc(dataArray, size);
    if (newArray == NULL) { return; }

    // Store the pointer to the new dataArray.
    this->dataArray = (SensorData *)newArray;

    // Store the number of sensors.
    this->numSensors = numSensors;

    for (uint8_t i = 0; i < numSensors; i++)
    {
        // Store the pin numbers specified by the user.
        dataArray[i].pin = pins[i];
    }
}


void Zumo32U4ProximitySensors::setBrightnessLevels(uint16_t * levels, uint8_t numLevels)
{
    const size_t size = numLevels * sizeof(uint16_t);

    void * newArray = realloc(levelsArray, size);
    if (newArray == NULL)
    {
        // There was an error allocating memory so just leave the
        // state of the object the same as it was before.
        return;
    }

    memcpy(newArray, levels, size);
    levelsArray = (uint16_t *)newArray;
    this->numLevels = numLevels;
}

void Zumo32U4ProximitySensors::prepareToRead()
{
    pullupsOn();

    lineSensorEmittersOff();

    if (levelsArray == NULL)
    {
        uint16_t defaultBrightnessLevels[] = { 4, 15, 32, 55, 85, 120 };
        setBrightnessLevels(defaultBrightnessLevels, 6);
    }
}

void Zumo32U4ProximitySensors::pullupsOn()
{
    // Set all the sensor pins to be pulled-up inputs so that they
    // are high whenever the sensor outputs are not active.
    for (uint8_t i = 0; i < numSensors; i++)
    {
        pinModeSafe(dataArray[i].pin, INPUT_PULLUP);
    }
}

// Turn off the down-facing IR LEDs because the proximity
// sensors tend to detect the IR coming from them.
void Zumo32U4ProximitySensors::lineSensorEmittersOff()
{
    if (lineSensorEmitterPin < NUM_DIGITAL_PINS)
    {
        digitalWrite(lineSensorEmitterPin, LOW);
        pinMode(lineSensorEmitterPin, OUTPUT);
        delayMicroseconds(pulseOffTimeUs);
    }
}

/* It is not feasible to turn off the pulses before checking the output of
 * the sensor because an interrupt might fire and cause the sensor check to
 * happen too late.
 */
void Zumo32U4ProximitySensors::read()
{
    prepareToRead();

    for (uint8_t i = 0; i < numSensors; i++)
    {
        dataArray[i].withRightLeds = 0;
        dataArray[i].withLeftLeds = 0;
    }

    for (uint8_t i = 0; i < numLevels; i++)
    {
        uint16_t brightness = levelsArray[i];

        Zumo32U4IRPulses::start(Zumo32U4IRPulses::Left, brightness, period);
        delayMicroseconds(pulseOnTimeUs);
        for (uint8_t i = 0; i < numSensors; i++)
        {
            if (!digitalReadSafe(dataArray[i].pin, 1))
            {
                dataArray[i].withLeftLeds++;
            }
        }
        Zumo32U4IRPulses::stop();
        delayMicroseconds(pulseOffTimeUs);

        Zumo32U4IRPulses::start(Zumo32U4IRPulses::Right, brightness, period);
        delayMicroseconds(pulseOnTimeUs);
        for (uint8_t i = 0; i < numSensors; i++)
        {
            if (!digitalReadSafe(dataArray[i].pin, 1))
            {
                dataArray[i].withRightLeds++;
            }
        }
        Zumo32U4IRPulses::stop();
        delayMicroseconds(pulseOffTimeUs);
    }
}

bool Zumo32U4ProximitySensors::readBasic(uint8_t sensorNumber)
{
    if (sensorNumber >= numSensors) { return 0; }
    return !digitalReadSafe(dataArray[sensorNumber].pin, 1);
}

uint8_t Zumo32U4ProximitySensors::countsWithLeftLeds(uint8_t sensorNumber) const
{
    if (sensorNumber >= numSensors) { return 0; }
    return dataArray[sensorNumber].withLeftLeds;
}

uint8_t Zumo32U4ProximitySensors::countsWithRightLeds(uint8_t sensorNumber) const
{
    if (sensorNumber >= numSensors) { return 0; }
    return dataArray[sensorNumber].withRightLeds;
}

uint8_t Zumo32U4ProximitySensors::findIndexForPin(uint8_t pin) const
{
    for (uint8_t i = 0; i < numSensors; i++)
    {
        if (dataArray[i].pin == pin)
        {
            return i;
        }
    }

    // The specified pin is not being used as a sensor, so return 255.  This can
    // be safely passed to countsWithLeftLeds, countsWithRightLeds, or
    // readBasic, and those functions will return 0.
    return 255;
}
