/*************************
   TO DO

   1) add in a counter that increments on each "tweak"
     - if we have to do too many tweaks at a time, either increase the tweak time, or decrease the 0.4 degree margin
   2)  Temperature calibration!!!

*/


float TARGET_TEMP = 58.0f;
String HW_VERSION = "2.0";
String SW_VERSION = "2.8";

// Written by Eric Light (starting in June 2015)
// Requires OneWriteNoResistor library from https://github.com/bigjosh/OneWireNoResistor/
// and the Dallas Temperature library from https://github.com/milesburton/Arduino-Temperature-Control-Library/
// And the RBD_Button and RBD_Timer libraries from http://robotsbigdata.com/docs-arduino-button.html

/* Pinout:

    GND
    D2 - LED screen RS
    D3 - LED screen E
    D4 - LED screen D4
    D5 - LED screen D5
    D6 - LED screen D6
    D7 - LED screen D7
    D8 - Temperature probe data pin  (DS18B20)
    D9
    D10
    D11 - Relay control pin
    D12
    D13
    3.3v
    A0 - 'Down' button (other leg to GND)
    A1 - 'Up' button (other leg to GND)
    A2 - 'Start' button LED+ (other leg to GND)
    A3 - 'Start' button (other leg to GND)
    A4
    A5
    A6 - (can't use due to weird A6/A7)
    A7 - (can't use due to weird A6/A7)
    5v - 'Start' button right
    GND - 'Start' button LED-


    Screen PINS
    A - 5v
    K - GND
    D7 - 7
    D6 - 6
    D5 - 5
    D4 - 4
    D0 through D3 - nothing
    E - 3
    RW - GND
    RS - 2
    V0 - 10k Trimpot
    VDD - +5v
    VSS - GND
*/

// Set up pin constants
const int ds_data = 8;  // Thermocouple (DS18B20) data pin
const int relay = 11;  // Relay (SSR) pin
const int start_LED = A2;  // "Start" LED pin

// Set up Button labels, timers, and libraries
#include <RBD_Button.h>
#include <RBD_Timer.h>
RBD::Button button_down(A0);
RBD::Button button_up(A1);
RBD::Button button_start(A3);
RBD::Timer up_timer;
RBD::Timer down_timer;
RBD::Timer screen_refresh;
bool START = false;

// Set up timer and libraries to save EEPROM data
#include <EEPROM.h>
RBD::Timer save_temp_setting;
int EEPROM_target_temp_address = 0;


// Set up thermocouple libraries
// Use a precision of 11, because it's able to poll in 375ms instead of 750ms(!!)
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS ds_data
#define TEMPERATURE_PRECISION 11
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermocouple;

// Set up LCD screen
#include <LiquidCrystal.h>
const int RS = 2; const int E = 3; const int D4 = 4; const int D5 = 5; const int D6 = 6; const int D7 = 7;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


// Set up other variables
float tempC = 0.0;

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Initialising variables");

  // Initialise pins
  pinMode(start_LED, OUTPUT); digitalWrite(start_LED, LOW);
  pinMode(relay, OUTPUT); digitalWrite(relay, LOW);

  // Check EEPROM for last-saved target temperature
  TARGET_TEMP = read_target_temperature();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting up...");
  lcd.setCursor(0, 1);
  lcd.print("HW v" + HW_VERSION + "; SW v" + SW_VERSION);
  delay(2500);

  // Set timer timeouts
  save_temp_setting.setTimeout(10000);
  screen_refresh.setHertz(1);
  up_timer.setTimeout(800);
  down_timer.setTimeout(800);


  // Initialise thermocouple
  sensors.getAddress(thermocouple, 0);
  sensors.setResolution(thermocouple, TEMPERATURE_PRECISION);
  sensors.setWaitForConversion(false);
  sensors.begin();
  sensors.requestTemperatures();

}

void loop(void)
{

  // Clear the LCD every second for fresh data
  if (screen_refresh.onRestart()) {
    lcd.clear();
    Serial.println("Target temperature is " + String(TARGET_TEMP) + "; Current temperature is " + String(tempC));
  }

  // Save the target temperature after 10 seconds
  if (save_temp_setting.onExpired()) {
    save_target_temperature();
  }

  check_buttons();  // Check the buttons for presses and react appropriately

  poll_temperature();

  temp_react();  // React intelligently to thermocouple data

}

float read_target_temperature() {

  float EEPROM_target = 0.00f;
  EEPROM.get(EEPROM_target_temp_address, EEPROM_target);
  Serial.println("Retrieved temperature " + String(EEPROM_target) + " from address " + String(EEPROM_target_temp_address));
  if (EEPROM_target > 40 && EEPROM_target < 90) {
    Serial.println("Setting target temperature from EEPROM");
    return EEPROM_target;
  }
  return TARGET_TEMP;
}

void save_target_temperature() {

  Serial.println("Saving temperature " + String(TARGET_TEMP) + " to address " + String(EEPROM_target_temp_address));
  EEPROM.put(EEPROM_target_temp_address, TARGET_TEMP);
  
}

void check_buttons() {

  // Poll the Start button
  if (button_start.onPressed()) {
    Serial.println("Toggling");
    START = !START;
    digitalWrite(start_LED, START);
  }

  // Up button

  // When pressed, start "hold-button" timer with a 1sec expiry
  if (button_up.onPressed()) {
    TARGET_TEMP += 0.1;
    Serial.println("New temperature is " + String(TARGET_TEMP));
    up_timer.restart();
    save_temp_setting.restart();
  }

  // If the button is still being pressed once the timer has expired, start responding again
  if (button_up.isPressed()) {
    if (up_timer.isExpired()) {
      TARGET_TEMP += 0.1;
      Serial.println("New temperature is " + String(TARGET_TEMP));
      delay(50);
    }
  }

  // Cancel the button timer when the button is released
  if (button_up.onReleased()) {
    up_timer.stop();
    save_temp_setting.restart();
  }

  // down button

  // When pressed, start "hold-button" timer with a 1sec expiry
  if (button_down.onPressed()) {
    TARGET_TEMP -= 0.1;
    Serial.println("New temperature is " + String(TARGET_TEMP));
    down_timer.restart();
  }

  // If the button is still being pressed once the timer has expired, start responding again
  if (button_down.isPressed()) {
    if (down_timer.isExpired()) {
      TARGET_TEMP -= 0.1;
      Serial.println("New temperature is " + String(TARGET_TEMP));
      delay(50);
    }
  }

  // Cancel the button timer when the button is released
  if (button_down.onReleased()) {
    down_timer.stop();
    save_temp_setting.restart();
  }
}


void poll_temperature() {
  // Read the most recent temperature calculated by the probe, and tell it to start a new one.
  tempC = sensors.getTempC(thermocouple);
  sensors.requestTemperatures();
}

void temp_react() {

  // Make sure we're getting usable readings

  if (tempC < 0 || tempC > 80) {
    Serial.println("Unplugged");
    relay_control(LOW);
    led_unplugged();
  }
  else {
    if (START) {
      // Only do this stuff if there's a thermostat plugged in

      if (tempC > TARGET_TEMP ) {
        // Overtemp!
        relay_control(LOW);
      }

      if (TARGET_TEMP - tempC > .4) {
        // we're off by over half a degree, so go hard
        relay_control(HIGH);
      }
      else {
        // We're only off by a tiny amount, so just give it a tickle
        Serial.print("Tweak");
        relay_control(HIGH);
        delay(100);
        relay_control(LOW);
      }

    }
    else {
      // If the Start button is off, ALWAYS turn off the relay
      relay_control(LOW);
    }

    // Update the LCD screen whether heating is enabled or not
    led_update(TARGET_TEMP, tempC);
  }


}



void relay_control(bool control) {


  // No matter what, if that start button is off, the relay is off

  if (START) {
    digitalWrite(relay, control);

    switch (control) {
      case HIGH: Serial.println("... Heating"); break;
      case LOW: Serial.println("... Cooling"); break;
    }
  }
  else {
    digitalWrite(relay, LOW);
  }

}

void led_unplugged() {
  lcd.setCursor(0, 0);
  lcd.print("Probe unplugged!");
  lcd.setCursor(0, 1);
  lcd.print("Waiting for data");
  delay(500);
}

void led_update(float target, float current)
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Target:  " + String(target));

  if (digitalRead(relay) == HIGH) {
    lcd.print(" H");
  }
  else {
    lcd.print("  ");
  }

  lcd.setCursor(0, 1);
  lcd.print("Current: " + String(current) + "  ");
}



