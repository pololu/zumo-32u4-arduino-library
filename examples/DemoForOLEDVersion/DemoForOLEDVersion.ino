/* This demo program shows many features of the Zumo 32U4.

It uses the buttons, LCD, and buzzer to provide a user interface.
It presents a menu to the user that lets the user select from
several different demos.

To use this demo program, you will need to have the LCD connected
to the Zumo 32U4.  If you cannot see any text on the LCD,
try rotating the contrast potentiometer. */

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
const char beepButtonA[] PROGMEM = "!c32";
const char beepButtonB[] PROGMEM = "!e32";
const char beepButtonC[] PROGMEM = "!g32";

// Custom characters for the LCD:

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

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0,1);
  display.print(F("\7B"));
  display.gotoXY(0,0);
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
  display.gotoXY(0, 6);
  display.print(F("   www.pololu.com/"));
  display.gotoXY(0, 7);
  display.print(F("      zumo32u4"));

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
  display.clear();
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
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

// Display line sensor readings. Holding button C turns off
// the IR emitters.
void lineSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();
  display.gotoXY(6, 1);
  display.print('C');

  uint16_t lineSensorValues[3];
  char c;

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
}

// Display proximity sensor readings.
void proxSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();

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
// displayed on the first line of the LCD; otherwise, an
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
  char buf[4];

  while (mainMenu.buttonMonitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    // Update the LCD and motors every 50 ms.
    if ((uint16_t)(millis() - lastUpdateTime) > 50)
    {
      lastUpdateTime = millis();

      display.gotoXY(0, 0);
      if (showEncoders)
      {
        sprintf(buf, "%03d", encCountsLeft);
        display.print(buf);
        display.gotoXY(5, 0);
        sprintf(buf, "%03d", encCountsRight);
        display.print(buf);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          display.print("Hold=run");
        }
        else if (instructCount == 40)
        {
          display.print("Tap=flip");
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

  uint8_t fugueTitlePos = 0;
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
  char buf[6];

  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      bool usbPower = usbPowerPresent();

      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      display.gotoXY(0, 0);
      sprintf(buf, "%5d", batteryLevel);
      display.print(buf);
      display.print(F(" mV"));
      display.gotoXY(3, 1);
      display.print(F("USB="));
      display.print(usbPower ? 'Y' : 'N');
    }
  }
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
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  initInertialSensors();

  loadCustomCharacters();

  // The brownout threshold on the ATmega32U4 is set to 4.3
  // V.  If VCC drops below this, a brownout reset will
  // occur, preventing the AVR from operating out of spec.
  //
  // Note: Brownout resets usually do not happen on the Zumo
  // 32U4 because the voltage regulator goes straight from 5
  // V to 0 V when VIN drops too low.
  //
  // The bootloader is designed so that you can detect
  // brownout resets from your sketch using the following
  // code:
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
  if(mainMenu.select())
  {
    // a menu item ran; show "Main Menu" again and repeat
    mainMenuWelcome();
  }
}
