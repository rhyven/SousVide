/*************************
 * TO DO
 * 
 * 1) add in a counter that increments on each "tweak"
 *   - if we have to do too many tweaks at a time, either increase the tweak time, or decrease the 0.4 degree margin
 *   
 *  
 */


float TARGET_TEMP = 58.0;
String HW_VERSION = "2.0";
String SW_VERSION = "2.5";

// Written by Eric Light (starting in June 2015)
// Requires OneWriteNoResistor library from https://github.com/bigjosh/OneWireNoResistor/
// and the Dallas Temperature library from https://github.com/milesburton/Arduino-Temperature-Control-Library/
// And the RBD_Button and RBD_Timer libraries from http://robotsbigdata.com/docs-arduino-button.html

/* Pinout:
 *  
 *  GND
 *  D2 - LED screen RS
 *  D3 - LED screen E
 *  D4 - LED screen D4
 *  D5 - LED screen D5
 *  D6 - LED screen D6
 *  D7 - LED screen D7
 *  D8 - Temperature probe data pin  (DS18B20)
 *  D9
 *  D10
 *  D11 - Relay control pin
 *  D12
 *  D13
 *  3.3v
 *  A0 - 'Down' button (other leg to GND)
 *  A1 - 'Up' button (other leg to GND)
 *  A2 - 'Start' button LED+ (other leg to GND)
 *  A3 - 'Start' button (other leg to GND)
 *  A4
 *  A5
 *  A6 - (can't use due to weird A6/A7)
 *  A7 - (can't use due to weird A6/A7)
 *  5v - 'Start' button right
 *  GND - 'Start' button LED-
 *  
 *  
 *  Screen PINS
 *  A - 5v
 *  K - GND
 *  
 */ 

// Set up pin constants
const int ds_data = 8;  // Thermocouple (DS18B20) data pin
const int relay = 11;  // Relay (SSR) pin
const int start_LED = A2;  // "Start" LED pin
int start_button_pressed = 0;  // For start button toggling

// Set up Button labels, timers, and libraries
#include <RBD_Button.h>
#include <RBD_Timer.h>
RBD::Button button_down(A0);
RBD::Button button_up(A1);
RBD::Button button_start(A3);
RBD::Timer up_timer;
RBD::Timer down_timer;

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
bool HEATING = false;
bool start_button_pressed = false;
unsigned long lastPress = 0;
float tempC = 0.0;

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Initialising variables");

  // Initialise pins
  pinMode(start_LED, OUTPUT); digitalWrite(start_LED, LOW);
  pinMode(relay, OUTPUT); digitalWrite(relay, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting up...");
  lcd.setCursor(0, 1);
  lcd.print("HW v" + HW_VERSION + "; SW v" + SW_VERSION);

  // Initialise thermocouple
  sensors.getAddress(thermocouple, 0);
  sensors.setResolution(thermocouple, TEMPERATURE_PRECISION);
  sensors.setWaitForConversion(false);
  sensors.begin();
  sensors.requestTemperatures();

}

void loop(void)
{

  check_buttons();  // Check the buttons for presses and react appropriately

  poll_temperature();

  temp_react();  // React intelligently to thermocouple data

}

void check_buttons() {

  // Poll the Start button
  if(button_start.onPressed()) {
    Serial.println("Toggling");
    digitalWrite(start_LED, !digitalRead(start_LED));
  }

// Up button

  // When pressed, start "hold-button" timer with a 1sec expiry
  if(button_up.onPressed()) {
    TARGET_TEMP += 0.1; 
    Serial.println("New temperature is " + String(TARGET_TEMP));
    up_timer.setTimeout(800);
    up_timer.restart();
  }

  // If the button is still being pressed once the timer has expired, start responding again
  if(button_up.isPressed()) {
    if(up_timer.isExpired()) {
      TARGET_TEMP += 0.1;
      Serial.println("New temperature is " + String(TARGET_TEMP));
      delay(50);
    }
  }

  // Cancel the button timer when the button is released
  if(button_up.onReleased()) {
    up_timer.stop();
  }

// down button

  // When pressed, start "hold-button" timer with a 1sec expiry
  if(button_down.onPressed()) {
    TARGET_TEMP -= 0.1; 
    Serial.println("New temperature is " + String(TARGET_TEMP));
    down_timer.setTimeout(800);
    down_timer.restart();
  }

  // If the button is still being pressed once the timer has expired, start responding again
  if(button_down.isPressed()) {
    if(down_timer.isExpired()) {
      TARGET_TEMP -= 0.1;
      Serial.println("New temperature is " + String(TARGET_TEMP));
      delay(50);
    }
  }

  // Cancel the button timer when the button is released
  if(button_down.onReleased()) {
    down_timer.stop();
  }
}


void poll_temperature(){
  // Read the most recent temperature calculated by the probe, and tell it to start a new one.
  tempC = sensors.getTempC(thermocouple);
  sensors.requestTemperatures();
}

void temp_react(){

  // Make sure we're getting usable readings

  if (tempC < 0 || tempC > 80) {
    Serial.print("Unplugged");
    led_unplugged();
    relay_control(LOW);
  }
  else {

    // Only do this stuff if there's a thermostat plugged in

    if (tempC > TARGET_TEMP ) {
      // Overtemp!
      relay_control(LOW);
    }
    else {
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
    
    led_update(TARGET_TEMP, tempC, true);
  }

  delay(200);

}



void relay_control(bool control) {


  // No matter what, if that start button is off, the relay is off

  if (start_button_pressed) {
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Probe unplugged!");
  lcd.setCursor(0, 1);
  lcd.print("Waiting for data");
}

void led_update(float target, float current, bool Heating)
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Target:  " + String(target));

  if (Heating && start_button_pressed) {
    lcd.print(" H");
  }
  else {
    lcd.print("  ");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Current: " + String(current));
}



