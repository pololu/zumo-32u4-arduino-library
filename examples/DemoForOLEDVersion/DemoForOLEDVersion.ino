/*
This is a demo program for the Zumo 32U4 OLED Robot.

It uses the buttons, display, and buzzer to provide a user
interface.  It presents a menu to the user that lets the user
select from several different demos.

To use this demo program, you will need to have the OLED
display connected to the Zumo 32U4.

If you have an older Zumo 32U4 with an LCD display,
use the other demo, DemoForLCDVersion.
*/

#include <Wire.h>
#include <Zumo32U4.h>

#include <PololuMenu.h>

Zumo32U4OLED display;

Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4IMU imu;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;

PololuMenu<typeof(display)> mainMenu;

// declarations for splash screen
#include "splash.h"

// A couple of simple tunes, stored in program space.
const char beepBrownout[] PROGMEM = "<c8";
const char beepWelcome[] PROGMEM = ">g32>>c32";
const char beepThankYou[] PROGMEM = ">>c32>g32";
const char beepFail[] PROGMEM = "<g-8r8<g-8r8<g-8";
const char beepPass[] PROGMEM = ">l32c>e>g>>c8";

// Custom characters for the display:

// This character is a back arrow.
const char backArrow[] PROGMEM = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This character is two chevrons pointing up.
const char forwardArrows[] PROGMEM = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This character is two chevrons pointing down.
const char reverseArrows[] PROGMEM = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This character is two solid arrows pointing up.
const char forwardArrowsSolid[] PROGMEM = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This character is two solid arrows pointing down.
const char reverseArrowsSolid[] PROGMEM = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharactersBackArrow()
{
  // The OLED library supports up to 8 custom characters.  Each
  // character has a number between 0 and 7.  We assign #7 to be
  // the back arrow; other characters are loaded by individual
  // demos as needed.

  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-7 to be bar graph characters.  This overwrites the
// back arrow, but we can use a trick that isn't possible on an
// actual LCD by first printing #7 to get a back arrow, then
// redefining #7 to be a bar graph character.  The already-
// printed #7 will remain a back arrow, unlike on an HD44780,
// where it would change to reflect the new #7 character.
//
// After a demo is done using the bar graph characters, it
// should restore the back arrow in #7 for other demos that
// expect it to be there.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
  display.loadCustomCharacter(levels + 7, 7);  // 8 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the display and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0, 1);
  display.print(F("\7B"));
  display.gotoXY(0, 0);
}

void displaySplash(uint8_t *graphics, uint8_t offset = 0)
{
  memset(graphics, 0, sizeof(zumo32U4Splash));
  for(uint16_t i = 0; i < sizeof(zumo32U4Splash) - offset*128; i++)
  {
    graphics[i] = pgm_read_byte(zumo32U4Splash + (i%128)*8 + i/128 + offset);
  }
  display.display();
}

void showSplash()
{
  // We only need the graphics array within showSplash(); it's not
  // used elsewhere in the demo program, so we can make it a local
  // variable.
  uint8_t graphics[1024];

  display.setLayout21x8WithGraphics(graphics);
  displaySplash(graphics, 0);

  uint16_t blinkStart = millis();
  while((uint16_t)(millis() - blinkStart) < 900)
  {
    // keep setting the LEDs on for 1s
    // the Green/Red LEDs might turn off during USB communication
    ledYellow(1);
    ledGreen(1);
    ledRed(1);
  }

  // scroll quickly up
  for(uint8_t offset = 1; offset < 6; offset ++)
  {
    delay(100);
    displaySplash(graphics, offset);
  }

  display.clear();
  display.gotoXY(0, 4);
  display.print(F("Push B to start demo!"));
  display.gotoXY(0, 5);
  display.print(F("For more info, visit"));
  display.gotoXY(3, 6);
  display.print(F("www.pololu.com/"));
  display.gotoXY(6, 7);
  display.print(F("zumo32u4"));

  while((uint16_t)(millis() - blinkStart) < 2000)
  {
    // keep the LEDs off for 1s
    ledYellow(0);
    ledGreen(0);
    ledRed(0);
  }

  // Keep blinking the green LED while waiting for the
  // user to press button B.
  blinkStart = millis();
  while (mainMenu.buttonMonitor() != 'B')
  {
    uint16_t blinkPhase = millis() - blinkStart;
    ledGreen(blinkPhase < 1000);
    if (blinkPhase >= 2000) { blinkStart += 2000; }
  }
  ledGreen(0);

  display.setLayout11x4WithGraphics(graphics);
  display.gotoXY(0,3);
  display.noAutoDisplay();
  display.print(F("Thank you!!"));
  display.display();

  buzzer.playFromProgramSpace(beepThankYou);
  delay(1000);
  display.clear();
  display.setLayout8x2();
}

// Blinks all three LEDs in sequence.
void ledDemo()
{
  displayBackArrow();

  uint8_t state = 3;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 4) { state = 0; }

      switch (state)
      {
      case 0:
        buzzer.play("c32");
        display.gotoXY(0, 0);
        display.print(F("Red   "));
        ledRed(1);
        ledGreen(0);
        ledYellow(0);
        break;

      case 1:
        buzzer.play("e32");
        display.gotoXY(0, 0);
        display.print(F("Green"));
        ledRed(0);
        ledGreen(1);
        ledYellow(0);
        break;

      case 2:
        buzzer.play("g32");
        display.gotoXY(0, 0);
        display.print(F("Yellow"));
        ledRed(0);
        ledGreen(0);
        ledYellow(1);
        break;
      }
    }
  }

  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, 7};
  display.print(barChars[height]);
}

void selfTestWaitShowingVBat()
{
  ledYellow(0);
  ledGreen(0);
  ledRed(0);
  while(!mainMenu.buttonMonitor())
  {
    display.gotoXY(0,0);
    display.print(' ');
    display.print(readBatteryMillivolts());
    display.print(F(" mV"));
    delay(100);
  }
}

void selfTestFail()
{
  display.gotoXY(0, 1);
  display.print(F("FAIL"));
  buzzer.playFromProgramSpace(beepFail);
  while(!mainMenu.buttonMonitor());
}

bool checkInput(uint8_t pin, bool expectedValue)
{
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *modeReg = portModeRegister(port);
  volatile uint8_t *outReg = portOutputRegister(port);

  if (*modeReg & bit)
  {
    // Pin is an output, so expect to read the output value.
    expectedValue = ((*outReg & bit) != 0);
  }

  if (expectedValue != digitalRead(pin))
  {
    display.print(pin);
    return false;
  }
  return true;
}

bool checkFrontInputs()
{
  // D0/PD2 is normally an output (OLED reset) but is pulled high for part of the test.
  if (!checkInput(0, HIGH)) { return false; }

  // D1/PD3 is normally an output (OLED clock) but is pulled high for part of the test.
  if (!checkInput(1, HIGH)) { return false; }

  // D2/PD1/SDA is pulled high by the IMU circuit.
  if (!checkInput(2, HIGH)) { return false; }

  // D3/PD0/SCL is pulled high by the IMU circuit.
  if (!checkInput(3, HIGH)) { return false; }

  // D4/A6/PD4 is being driven by prox sensor RIGHT and in an unknown state.

  // D5/PC6 is pulled low on the board and (prox LED PWM).
  if (!checkInput(5, LOW)) { return false; }

  // D6/A7/PD7 is pulled low on the board (buzzer control).
  if (!checkInput(6, LOW)) { return false; }

  // D11/PB7 is pulled high by the front sensor board (line sensor emitter control).
  if (!checkInput(11, HIGH)) { return false; }

  // D12/A11/PD6 is pulled low (line sensor DOWN5).
  if (!checkInput(12, LOW)) { return false; }

  // A0/D18/PF7 is pulled low (line sensor DOWN1).
  if (!checkInput(A0, LOW)) { return false; }

  // A1/D19/PF6 is connected to BATLEV (VBAT/2) and should read high (also prox LED select).
  if (!checkInput(A1, HIGH)) { return false; }

  // A2/D20/PF5 is being driven by prox sensor LEFT and in an unknown state.

  // A3/D21/PF4 is pulled low (line sensor DOWN3).
  if (!checkInput(A3, LOW)) { return false; }

  // A4/D22/PF1/A4 is being driven by prox sensor FRONT and in an unknown state.

  return true;
}

bool driveHighAndLow(uint8_t pin)
{
  pinMode(pin, OUTPUT);

  digitalWrite(pin, HIGH);
  if (!checkFrontInputs())
  {
    pinMode (pin, INPUT);
    return false;
  }

  digitalWrite(pin, LOW);
  if (!checkFrontInputs())
  {
    pinMode (pin, INPUT);
    return false;
  }

  pinMode(pin, INPUT);
  return true;
}

bool testFrontPins()
{
  TCCR4C = 0x01; // disable Timer4 PWM control of pin 6
  FastGPIO::Pin<6>::setInput();

  if (!checkFrontInputs()) { return false; }

  // while driving OLED reset and clock pins, pull the other high
  FastGPIO::Pin<1>::setInputPulledUp();
  if (!driveHighAndLow(0)) { return false; }
  FastGPIO::Pin<0>::setInputPulledUp();
  if (!driveHighAndLow(1)) { return false; }

  // reinit and refresh OLED after messing with reset and clock pins
  display.reinitialize();
  display.display();

  TWCR &= ~_BV(TWEN); // disable TWI control of SDA and SCL
  if (!driveHighAndLow(2)) { return false; }
  if (!driveHighAndLow(3)) { return false; }
  TWCR |= _BV(TWEN); // re-enable TWI

  // D4/A6/PD4 is being driven by prox sensor RIGHT; can't drive it here.
  if (!driveHighAndLow(5)) { return false; };
  if (!driveHighAndLow(6)) { return false; };
  if (!driveHighAndLow(11)) { return false; };
  if (!driveHighAndLow(12)) { return false; };
  if (!driveHighAndLow(A0)) { return false; };
  if (!driveHighAndLow(A1)) { return false; };
  // D20/A2/PF5 is being driven by prox sensor LEFT; can't drive it here.
  if (!driveHighAndLow(A3)) { return false; };
  // D22/A4/PF1/A4 is being driven by prox sensor FRONT; can't drive it here.

  TCCR4C = 0x09; // re-enable Timer4 PWM output for buzzer
  FastGPIO::Pin<6>::setOutputLow();

  return true;
}

void printAndReadProxSensors(uint8_t counts)
{
  display.gotoXY(7, 0);
  display.print(counts);
  proxSensors.read();
}

void testProxSensors()
{
  uint8_t counts;

  display.clear();
  display.print(F("Prox"));

  proxSensors.read();

  display.gotoXY(0, 1);
  display.print(F("Lft near"));
  while ((counts = proxSensors.countsLeftWithLeftLeds()) < 5) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_C(5), 100, 15);

  display.gotoXY(4, 1);
  display.print(F("far "));
  while ((counts = proxSensors.countsLeftWithLeftLeds()) > 1) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_C(4), 100, 15);

  display.gotoXY(0, 1);
  display.print(F("FrL near"));
  while ((counts = proxSensors.countsFrontWithLeftLeds()) < 5) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_D(5), 100, 15);

  display.gotoXY(4, 1);
  display.print(F("far "));
  while ((counts = proxSensors.countsFrontWithLeftLeds()) > 1) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_D(4), 100, 15);

  display.gotoXY(0, 1);
  display.print(F("Rgt near"));
  while ((counts = proxSensors.countsRightWithRightLeds()) < 5) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_E(5), 100, 15);

  display.gotoXY(4, 1);
  display.print(F("far "));
  while ((counts = proxSensors.countsRightWithRightLeds()) > 1) { printAndReadProxSensors(counts); }
  buzzer.playNote(NOTE_E(4), 100, 15);
}

bool testMotorsLineSensors()
{
  uint16_t startMs;

  display.gotoXY(0, 0);
  display.print(F("Mtr+LnSn"));
  display.gotoXY(0, 1);
  display.print(F("Press B "));

  buttonB.waitForButton();
  display.clear();
  delay(500);

  // drive forward and calibrate
  motors.setSpeeds(100, 100);
  startMs = millis();
  while ((uint16_t)(millis() - startMs) < 800)
  {
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);

  for (uint8_t i = 0; i < 3; i++)
  {
    int16_t range = lineSensors.calibratedMaximumOn[i] - lineSensors.calibratedMinimumOn[i];
    if (range < 500)
    {
      display.print(F("DN"));
      display.print(i*2+1);
      display.gotoXY(4, 0);
      display.print(range);
      return false;
    }
  }

  uint16_t lineSensorValues[3];
  enum : uint8_t {NoLine, LineStarted, LineEnded} state = NoLine;
  int16_t encoderCounts = 0;

  // drive backward and measure line width
  motors.setSpeeds(-100, -100);
  startMs = millis();
  while ((uint16_t)(millis() - startMs) < 800)
  {
    lineSensors.readCalibrated(lineSensorValues);

    if (state == NoLine && lineSensorValues[1] > 200)
    {
      state = LineStarted;
      encoders.getCountsAndResetLeft();
      encoders.getCountsAndResetRight();
    }
    else if (state == LineStarted && lineSensorValues[1] < 100)
    {
      state = LineEnded;
      encoderCounts = -(encoders.getCountsLeft() + encoders.getCountsRight());
    }
  }
  motors.setSpeeds(0, 0);

  if (state != LineEnded)
  {
    display.print(F("Bad line"));
    return false;
  }

  uint8_t gearRatio;
  if (encoderCounts >= 158 && encoderCounts <= 206) // expect 182 (91 counts on each side)
  {
    gearRatio = 50;
  }
  else if (encoderCounts >= 232 && encoderCounts <= 304) // expect 268 (134 counts on each side)
  {
    gearRatio = 75;
  }
  else if (encoderCounts >= 306 && encoderCounts <= 402) // expect 354 (177 counts on each side)
  {
    gearRatio = 100;
  }
  else
  {
    display.print(encoderCounts);
    return false;
  }

  display.print(gearRatio);
  display.print(F(":1?"));
  display.gotoXY(0, 1);
  display.print(encoderCounts);
  display.print(F("  B=Y"));
  while (true)
  {
    char button = mainMenu.buttonMonitor();
    if (button == 'B')
    {
      return true;
    }
    else if (button != 0)
    {
      return false;
    }
  }
}

void selfTest()
{
  display.gotoXY(0, 0);
  display.print(F("Zumo32U4"));
  display.gotoXY(0, 1);
  display.print(F("SelfTest"));
  delay(1000);

  // test some voltages and IMU presence
  display.clear();
  display.print(F("USB "));
  if (usbPowerPresent())
  {
    display.print(F("on"));
    selfTestFail();
    return;
  }
  else
  {
    display.print(F("off"));
  }
  ledYellow(1);
  delay(500);

  display.clear();
  display.print(F("VBAT"));
  int v = readBatteryMillivolts();
  display.print(v);
  if (v < 4000 || v > 7000)
  {
    selfTestFail();
    return;
  }
  ledGreen(1);
  delay(500);

  display.clear();
  display.print(F("Pin"));
  if (!testFrontPins())
  {
    TCCR4C = 0x09; // re-enable Timer4 PWM output for buzzer
    FastGPIO::Pin<6>::setOutputLow();

    selfTestFail();
    return;
  }
  display.print(F("s OK"));
  delay(500);

  display.clear();
  display.print(F("IMU "));
  if (!imu.init())
  {
    selfTestFail();
    return;
  }
  display.print(F("OK"));
  ledRed(1);
  delay(500);

  testProxSensors();

  if (!testMotorsLineSensors())
  {
    selfTestFail();
    return;
  }

  // Passed all tests!
  display.gotoXY(0, 1);
  display.print(F("PASS    "));
  delay(250); // finish the button beep
  buzzer.playFromProgramSpace(beepPass);
  selfTestWaitShowingVBat();
}

// Display line sensor readings. Holding button C turns off
// the IR emitters.
void lineSensorDemo()
{
  displayBackArrow();
  loadCustomCharactersBarGraph();
  display.gotoXY(6, 1);
  display.print('C');

  uint16_t lineSensorValues[3];

  while (mainMenu.buttonMonitor() != 'B')
  {
    bool emittersOff = buttonC.isPressed();

    if (emittersOff)
    {
      lineSensors.read(lineSensorValues, QTR_EMITTERS_OFF);
    }
    else
    {
      lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
    }

    display.gotoXY(1, 0);
    for (uint8_t i = 0; i < 3; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 2000, 0, 8);
      printBar(barHeight);
      display.print(' ');
    }

    // Display an indicator of whether emitters are on or
    // off.
    display.gotoXY(7, 1);
    if (emittersOff)
    {
      display.print('\xa5');  // centered dot
    }
    else
    {
      display.print('*');
    }
  }

  loadCustomCharactersBackArrow(); // restore back arrow for other demos
}

// Display proximity sensor readings.
void proxSensorDemo()
{
  displayBackArrow();
  loadCustomCharactersBarGraph();

  while (mainMenu.buttonMonitor() != 'B')
  {
    bool proxLeftActive = proxSensors.readBasicLeft();
    bool proxFrontActive = proxSensors.readBasicFront();
    bool proxRightActive = proxSensors.readBasicRight();
    proxSensors.read();

    display.gotoXY(0, 0);
    printBar(proxSensors.countsLeftWithLeftLeds());
    printBar(proxSensors.countsLeftWithRightLeds());
    display.print(' ');
    printBar(proxSensors.countsFrontWithLeftLeds());
    printBar(proxSensors.countsFrontWithRightLeds());
    display.print(' ');
    printBar(proxSensors.countsRightWithLeftLeds());
    printBar(proxSensors.countsRightWithRightLeds());

    // On the last 3 characters of the second line, display
    // basic readings of the sensors taken without sending
    // IR pulses.
    display.gotoXY(5, 1);
    printBar(proxLeftActive);
    printBar(proxFrontActive);
    printBar(proxRightActive);
  }

  loadCustomCharactersBackArrow(); // restore back arrow for other demos
}

// Starts I2C and initializes the inertial sensors.
void initInertialSensors()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
}

// Given 3 readings for axes x, y, and z, prints the sign
// and axis of the largest reading unless it is below the
// given threshold.
void printLargestAxis(int16_t x, int16_t y, int16_t z, uint16_t threshold)
{
  int16_t largest = x;
  char axis = 'X';

  if (abs(y) > abs(largest))
  {
    largest = y;
    axis = 'Y';
  }
  if (abs(z) > abs(largest))
  {
    largest = z;
    axis = 'Z';
  }

  if (abs(largest) < threshold)
  {
    display.print("  ");
  }
  else
  {
    bool positive = (largest > 0);
    display.print(positive ? '+' : '-');
    display.print(axis);
  }
}

// Print the direction of the largest rotation rate measured
// by the gyro and the up direction based on the
// accelerometer's measurement of gravitational acceleration
// (assuming gravity is the dominant force acting on the
// Zumo).
void inertialDemo()
{
  displayBackArrow();

  display.gotoXY(3, 0);
  display.print(F("Rot"));
  display.gotoXY(4, 1);
  display.print(F("Up"));

  while (mainMenu.buttonMonitor() != 'B')
  {
    imu.read();

    display.gotoXY(6, 0);
    printLargestAxis(imu.g.x, imu.g.y, imu.g.z, 2000);
    display.gotoXY(6, 1);
    printLargestAxis(imu.a.x, imu.a.y, imu.a.z, 200);
  }
}

// Provides an interface to test the motors. Holding button A or C
// causes the left or right motor to accelerate; releasing the
// button causes the motor to decelerate. Tapping the button while
// the motor is not running reverses the direction it runs.
//
// If the showEncoders argument is true, encoder counts are
// shown on the first line of the display; otherwise, an
// instructional message is shown.
void motorDemoHelper(bool showEncoders)
{
  loadCustomCharactersMotorDirs();
  display.clear();
  display.gotoXY(1, 1);
  display.print(F("A \7B C"));

  int16_t leftSpeed = 0, rightSpeed = 0;
  int8_t leftDir = 1, rightDir = 1;
  uint16_t lastUpdateTime = millis() - 100;
  uint8_t btnCountA = 0, btnCountC = 0, instructCount = 0;

  int16_t encCountsLeft = 0, encCountsRight = 0;

  while (mainMenu.buttonMonitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    // Update the display and motors every 50 ms.
    if ((uint16_t)(millis() - lastUpdateTime) > 50)
    {
      lastUpdateTime = millis();

      display.gotoXY(0, 0);
      if (showEncoders)
      {
        // pad with 0s to right-align the number
        if(encCountsLeft < 100) { display.print('0'); }
        if(encCountsLeft <  10) { display.print('0'); }

        display.print(encCountsLeft);
        display.print("  ");

        if(encCountsRight < 100) { display.print('0'); }
        if(encCountsRight <  10) { display.print('0'); }

        display.print(encCountsRight);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          display.print(F("Hold=run"));
        }
        else if (instructCount == 40)
        {
          display.print(F("Tap=flip"));
        }
        if (++instructCount == 80) { instructCount = 0; }
      }

      if (buttonA.isPressed())
      {
        if (btnCountA < 4)
        {
          btnCountA++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          leftSpeed += 15;
        }
      }
      else
      {
        if (leftSpeed == 0 && btnCountA > 0 && btnCountA < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          leftDir = -leftDir;
        }
        btnCountA = 0;
        leftSpeed -= 30;
      }

      if (buttonC.isPressed())
      {
        if (btnCountC < 4)
        {
          btnCountC++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          rightSpeed += 15;
        }
      }
      else
      {
        if (rightSpeed == 0 && btnCountC > 0 && btnCountC < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          rightDir = -rightDir;
        }
        btnCountC = 0;
        rightSpeed -= 30;
      }

      leftSpeed = constrain(leftSpeed, 0, 400);
      rightSpeed = constrain(rightSpeed, 0, 400);

      motors.setSpeeds(leftSpeed * leftDir, rightSpeed * rightDir);

      // Display arrows pointing the appropriate direction
      // (solid if the motor is running, chevrons if not).
      display.gotoXY(0, 1);
      if (leftSpeed == 0)
      {
        display.print((leftDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((leftDir > 0) ? '\2' : '\3');
      }
      display.gotoXY(7, 1);
      if (rightSpeed == 0)
      {
        display.print((rightDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((rightDir > 0) ? '\2' : '\3');
      }
    }
  }
  motors.setSpeeds(0, 0);
}


// Motor demo with instructions.
void motorDemo()
{
  motorDemoHelper(false);
}

// Motor demo with encoder counts.
void encoderDemo()
{
  motorDemoHelper(true);
}

const char fugue[] PROGMEM =
  "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugueTitle[] PROGMEM =
  "       Fugue in D Minor - by J.S. Bach       ";

// Play a song on the buzzer and display its title.
void musicDemo()
{
  displayBackArrow();

  size_t fugueTitlePos = 0;
  uint16_t lastShiftTime = millis() - 2000;

  while (mainMenu.buttonMonitor() != 'B')
  {
    // Shift the song title to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      display.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = pgm_read_byte(fugueTitle + fugueTitlePos + i);
        display.print(c);
      }
      fugueTitlePos++;

      if (fugueTitlePos + 8 >= strlen(fugueTitle))
      {
        fugueTitlePos = 0;
      }
    }

    if (!buzzer.isPlaying())
    {
      buzzer.playFromProgramSpace(fugue);
    }
  }
}

// Display the the battery (VIN) voltage and indicate whether USB
// power is detected.
void powerDemo()
{
  displayBackArrow();

  uint16_t lastDisplayTime = millis() - 2000;

  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      bool usbPower = usbPowerPresent();

      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      display.gotoXY(0, 0);

      // pad with spaces to right-align the number
      if (batteryLevel < 10000) { display.print(' '); }
      if (batteryLevel <  1000) { display.print(' '); }
      if (batteryLevel <   100) { display.print(' '); }
      if (batteryLevel <    10) { display.print(' '); }

      display.print(batteryLevel);
      display.print(F(" mV"));
      display.gotoXY(3, 1);
      display.print(F("USB="));
      display.print(usbPower ? 'Y' : 'N');
    }
  }
}

// This demo shows all characters that the OLED can display, 128 at a
// time.  Press any button to advance to the the next page of 8
// characters.  Note that the first eight are the custom characters.
// Most of these are initially blank, but if you run other demos that
// set custom characters then return here, you will see what they
// loaded.
void displayDemo()
{
  display.setLayout21x8();
  display.noAutoDisplay();

  for (int y = 0; y < 8; y++)
  {
    display.gotoXY(0, y);
    display.print(y, HEX);
    display.print(": ");
    for (int x = 0; x < 16; x++)
    {
      display.print((char)(y*16 + x));
    }
    display.print(" :");
  }
  display.display();

  // wait for a button press
  while (mainMenu.buttonMonitor() == 0);

  display.noAutoDisplay();

  for (int y = 0; y < 8; y++)
  {
    display.gotoXY(0, y);
    display.print(y + 8, HEX);
    display.print(": ");
    for(int x = 0; x < 16; x++)
    {
      // stop at 0xA5 (centered dot), the last character defined in this demo's
      // extended font
      uint8_t c = 128 + y*16 + x;
      if (c > 0xA5) { c = ' '; }
      display.print((char)c);
    }
    display.print(" :");
  }
  display.display();

  // wait for a button press
  while(mainMenu.buttonMonitor() == 0);

  display.setLayout8x2();
}

void setup()
{
  static const PololuMenuItem mainMenuItems[] = {
    { F("LEDs"), ledDemo },
    { F("LineSens"), lineSensorDemo },
    { F("ProxSens"), proxSensorDemo },
    { F("Inertial"), inertialDemo },
    { F("Motors"), motorDemo },
    { F("Encoders"), encoderDemo },
    { F("Music"), musicDemo },
    { F("Power"), powerDemo },
    { F("OLED"), displayDemo },
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  initInertialSensors();

  loadCustomCharactersBackArrow();

  // The brownout threshold on the ATmega32U4 is set to 4.3 V.
  // If VCC drops below this, a brownout reset will occur,
  // preventing the AVR from operating out of spec.
  //
  // Note: Brownout resets usually do not happen on the Zumo
  // 32U4 because the voltage regulator goes straight from 5 V
  // to 0 V when VIN drops too low.
  //
  // The bootloader is designed so that you can detect brownout
  // resets from your sketch using the following code:
  bool brownout = MCUSR >> BORF & 1;
  MCUSR = 0;

  if (brownout)
  {
    // The board was reset by a brownout reset
    // (VCC dropped below 4.3 V).
    // Play a special sound and display a note to the user.

    buzzer.playFromProgramSpace(beepBrownout);
    display.clear();
    display.print(F("Brownout"));
    display.gotoXY(0, 1);
    display.print(F(" reset! "));
    delay(1000);
  }
  else
  {
    buzzer.playFromProgramSpace(beepWelcome);
  }

  if (buttonC.isPressed())
  {
    selfTest();
    return;
  }

  showSplash();

  mainMenuWelcome();
}

// Clear LEDs and show a message about the main menu.
void mainMenuWelcome()
{
  ledYellow(false);
  ledGreen(false);
  ledRed(false);
  display.clear();
  display.print(F("  Main"));
  display.gotoXY(0, 1);
  display.print(F("  Menu"));
  delay(1000);
}

void loop()
{
  if (mainMenu.select())
  {
    // a menu item ran; show "Main Menu" again and repeat
    mainMenuWelcome();
  }
}
