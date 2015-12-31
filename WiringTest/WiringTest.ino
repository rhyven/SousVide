// Wiring tester
// Written November 2015 by Eric Light

float TARGET_TEMP = 60.0;

// *************************************
// TO DO:  Test relay, test thermocouple
// *************************************


/*   Testing class for Sous vide version 2
 *   Requires OneWireNoResistor, Dallas Temperature, RBD_Butoon, and RBD_Timer libraries
 *  
 * Pinout:
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


-// Set up other variables
-bool HEATING = false;
-bool start_button_pressed = false;
-unsigned long lastPress = 0;
-float tempC = 0.0;


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

void loop(void)
{

 check_buttons();
 led_update();
 // delay(50);

}

void led_update()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print(TARGET_TEMP);
}

