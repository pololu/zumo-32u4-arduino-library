/* This example shows how to:

- Measure the voltage of the Zumo's batteries.
- Detect whether USB power is present.

The results are printed to the LCD and also to the serial
monitor.

The battery voltage can only be read when the power switch is in
the "On" position.  If the power switch is off, the voltage
reading displayed by this demo will be low, but it might not be
zero because the Zumo 32U4 has capacitors that take a while to
discharge. */

#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;

void setup()
{

}

void loop()
{
  bool usbPower = usbPowerPresent();

  uint16_t batteryLevel = readBatteryMillivolts();

  // Print the results to the LCD.
  lcd.clear();
  lcd.print(F("B="));
  lcd.print(batteryLevel);
  lcd.print(F("mV   "));
  lcd.gotoXY(0, 1);
  lcd.print(F("USB="));
  lcd.print(usbPower ? 'Y' : 'N');

  // Print the results to the serial monitor.
  Serial.print(F("USB="));
  Serial.print(usbPower ? 'Y' : 'N');
  Serial.print(F(" B="));
  Serial.print(batteryLevel);
  Serial.println(F(" mV"));

  delay(200);
}
