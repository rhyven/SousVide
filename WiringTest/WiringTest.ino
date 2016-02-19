/*

   Wiring test library for Sous Vide HW version 2.0

*/


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
RBD::Timer screen_refresh;
bool START = false;


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
int lcd_char = 33;

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

  // Set timer timeouts
  screen_refresh.setHertz(2);


  // Initialise thermocouple
  sensors.getAddress(thermocouple, 0);
  sensors.setResolution(thermocouple, TEMPERATURE_PRECISION);
  sensors.setWaitForConversion(false);
  sensors.begin();
  sensors.requestTemperatures();

}

void loop(void)
{
 
  // Test a new character every .5 second
  if (screen_refresh.onRestart()) {
    lcd_test(lcd_char);
    lcd_char++;
    if(lcd_char > 126) {
      lcd_char=33;
    }
  }

  check_buttons();

  // Button Voltages
  int up_btn = analogRead(A1);
  int dn_btn = analogRead(A0);
  int st_btn = analogRead(A3);

  
  Serial.println("Temp = " + String(tempC) + "  Up = " + String(up_btn) + "  Dn = " + String(dn_btn) + "  St = " + String(st_btn));
  

  poll_temperature();

}

void check_buttons() {

  // Poll the Start button
  if (button_start.onPressed()) {
    Serial.println("Start button pressed");
    START = !START;
    digitalWrite(start_LED, START);
  }

  // Up button

  if (button_up.onPressed()) {
    Serial.println("Up button pressed");
  }

  // down button
  if (button_down.onPressed()) {
    Serial.println("Down button pressed");
  }

}


void poll_temperature() {
  // Read the most recent temperature calculated by the probe, and tell it to start a new one.
  tempC = sensors.getTempC(thermocouple);
  sensors.requestTemperatures();
}


void lcd_test(int test_char)
{
  // Write the test character to a position
  
  lcd.setCursor(test_char % 16, 0);
  lcd.write(test_char);

  lcd.setCursor(test_char % 16, 1);
  lcd.write(test_char);
}



