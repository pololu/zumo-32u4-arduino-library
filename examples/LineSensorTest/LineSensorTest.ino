/** This example shows how to read the raw values from the five
line sensors on the Zumo32U4 Front Sensor Array.  This example is
useful if you are having trouble with the sensors or just want to
characterize their behavior.

The raw (uncalibrated) values are reported to the serial monitor,
and also displayed on the LCD.  The first line of the LCD
displays a bar graph of all five readings.  The upper right
corner shows a an "E" if the IR emitters are being used (the
default) or an "e" if they are not being used.  The second line
displays the raw reading for the currently-selected sensor.

You can press the "A" and "B" buttons to change which sensor is
selected.

You can press the "C" button to toggle whether the IR emitters
are on during the reading.

In order for the second and forth sensors to work, jumpers on the
front sensor array must be installed in order to connect pin 4 to
DN4 and pin 20 to DN2. */

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];

bool useEmitters = true;

uint8_t selectedSensorIndex = 0;

void setup()
{
  lineSensors.initFiveSensors();

  loadCustomCharacters();
}

void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  lcd.loadCustomCharacter(levels + 0, 0);  // 1 bar
  lcd.loadCustomCharacter(levels + 1, 1);  // 2 bars
  lcd.loadCustomCharacter(levels + 2, 2);  // 3 bars
  lcd.loadCustomCharacter(levels + 3, 3);  // 4 bars
  lcd.loadCustomCharacter(levels + 4, 4);  // 5 bars
  lcd.loadCustomCharacter(levels + 5, 5);  // 6 bars
  lcd.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, 255};
  lcd.print(barChars[height]);
}

void printReadingsToLCD()
{
  // On the first line of the LCD, display the bar graph.
  lcd.gotoXY(0, 0);
  for (uint8_t i = 0; i < 5; i++)
  {
    uint8_t barHeight = map(lineSensorValues[i], 0, 2000, 0, 8);
    printBar(barHeight);
  }

  // Print "E" if the emitters are on, "e" if they are off.
  lcd.gotoXY(7, 0);
  lcd.print(useEmitters ? 'E' : 'e');

  // On the second line of the LCD, display one raw reading.
  lcd.gotoXY(0, 1);
  lcd.print(selectedSensorIndex);
  lcd.print(F(": "));
  lcd.print(lineSensorValues[selectedSensorIndex]);
  lcd.print(F("    "));
}

// Prints a line with all the sensor readings to the serial
// monitor.
void printReadingsToSerial()
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %4d %4d %c\n",
    lineSensorValues[0],
    lineSensorValues[1],
    lineSensorValues[2],
    lineSensorValues[3],
    lineSensorValues[4],
    useEmitters ? 'E' : 'e'
  );
  Serial.print(buffer);
}

void loop()
{
  static uint16_t lastSampleTime = 0;

  if ((uint16_t)(millis() - lastSampleTime) >= 100)
  {
    lastSampleTime = millis();

    // Read the line sensors.
    lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);

    // Send the results to the LCD and to the serial monitor.
    printReadingsToLCD();
    printReadingsToSerial();
  }

  // If button A is pressed, select the previous sensor.
  if (buttonA.getSingleDebouncedPress())
  {
    selectedSensorIndex = (selectedSensorIndex + NUM_SENSORS - 1) % NUM_SENSORS;
  }

  // If button B is pressed, select the next sensor.
  if (buttonB.getSingleDebouncedPress())
  {
    selectedSensorIndex = (selectedSensorIndex + 1) % NUM_SENSORS;
  }

  // If button C is pressed, toggle the state of the emitters.
  if (buttonC.getSingleDebouncedPress())
  {
    useEmitters = !useEmitters;
  }
}
