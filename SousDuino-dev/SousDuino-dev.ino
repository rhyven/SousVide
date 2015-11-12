// Eric's SousDuino controller, version 2
// Written June 2015 by Eric Light

int TARGET_TEMP = 60;

// Download the OneWriteNoResistor library from https://github.com/bigjosh/OneWireNoResistor/
// More info about the NoResistor library at http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892
// Also install the Dallas Temperature library from https://github.com/milesburton/Arduino-Temperature-Control-Library/

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
 *  A0 - 'Up' button
 *  A1 - 'Down' button
 *  A2 - 'Start' button LED+  (note can't be A6 due to weird A6/A7)
 *  A3 - 'Start' button left -- ALSO 10k resistor from A7 to GND
 *  A4
 *  A5
 *  A6
 *  A7
 *  5v - 'Start' button right
 *  GND - 'Start' button LED-
 *  
 *  
 *  Screen PINS
 *  A - 5v
 *  K - GND
 *  
 */ 


// Thermocouple (DS18B20) PIN labels
const int ds_data = 8;

// Relay (SSR) PIN labels
const int relay = 11;

// Button labels
const int button_up = A0;
const int button_down = A1;
const int button_start_LED = A2;
const int button_start = A3;
const int analogHigh = 900;
const int button_delay = 100;

// Set up thermocouple libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS ds_data
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermocouple;


// Set up LCD screen libraries
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
const int RS = 2; const int E = 3; const int D4 = 4; const int D5 = 5; const int D6 = 6; const int D7 = 7;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


// Set up other variables
bool HEATING = false;
bool start_button_pressed = false;
unsigned long lastPress = 0;

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Initialising variables");

  pinMode(button_up, INPUT);
  pinMode(button_down, INPUT);
  pinMode(button_start, INPUT);
  pinMode(button_start_LED, OUTPUT);
  
  // Initialse Relay pins
  pinMode(relay, OUTPUT);  digitalWrite(relay, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

}


void check_buttons() {

  // read the state of the up button

  // read the state of the down button

  // Shit I'm sure there's a better way to do this.  Gonna have to break this out into a function.

  // read the state of the start button
  int button_start_voltage = analogRead(button_start);
  Serial.println("Start button voltage = " + String(button_start_voltage));

  // Check to see if the start button is being pressed
  if (button_start_voltage > analogHigh) {
    
    // Only respond if it's been a delay since last press
    if (millis() - lastPress > button_delay) {
      start_button_pressed = !start_button_pressed; 
    }
    
    lastPress = millis();
  }

  // Update Start button LED state
  digitalWrite(button_start_LED, start_button_pressed);
  
}

void loop(void)
{

/*
 * I really want to be checking the state of all the buttons every loop
 * Need to check button_up, button_down, and button_start
 * 
 */
  
  
  // Read temperature
  sensors.getAddress(thermocouple, 0);
  sensors.begin();
  sensors.requestTemperatures(); // Send the command to get temperature
  float tempC = sensors.getTempC(thermocouple);  // Convert temperature into Celcius


  check_buttons();








  // Make sure we're getting usable readings
  if (tempC < 0) {
    Serial.print("Unplugged");
    relay_off();
    led_unplugged();
    delay(500);
  }

  // See if we need to heat
  else {
    Serial.print(tempC);

    if (tempC > TARGET_TEMP ) {
      // Overtemp!
      relay_off();
    }
    else {
      if (TARGET_TEMP - tempC > .4) {
        // we're off by over half a degree, so go hard
        relay_on();
      }
      else {
        // We're only off by a tiny amount, so just give it a tickle
        Serial.print("Tweak");
        relay_on();
        delay(100);
        relay_off();
      }
    }
    // Only update the LCD if there's a thermostat plugged in
    led_update(TARGET_TEMP, tempC, true);
  }

  delay(200);

}

void relay_off()
{
  Serial.println("... Cooling");
  digitalWrite(relay, LOW);
  HEATING = false;
}

void relay_on()
{
  Serial.println("... Heating");
  digitalWrite(relay, HIGH);
  HEATING = true;
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
  lcd.setCursor(0, 1);
  lcd.print("Current: " + String(current));
  if (HEATING) {
    lcd.setCursor(15, 0);
    lcd.print("H");
  }


}

