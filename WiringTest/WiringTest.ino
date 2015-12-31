// Wiring tester
// Written November 2015 by Eric Light

int TARGET_TEMP = 60;

// *************************************
// TO DO:  Test relay, test thermocouple
// *************************************


/*   Testing class for Sous vide version 2
 *   Requires OneWireNoResistor, and Dallas Temperature
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
 *  A0 - 'Up' button
 *  A1 - 'Down' button
 *  A2 - 'Start' button LED+
 *  A3 - 'Start' button
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

// Thermocouple (DS18B20) PIN labels
const int ds_data = 8;

// Relay (SSR) PIN labels
const int relay = 11;

// Set up Button labels and libraries
#include <RBD_Button.h>
 
RBD::Button button_up(A0); // input_pullup by default
RBD::Button button_down(A1); // input_pullup by default
RBD::Button button_start(A3); // input_pullup by default


// const int button_up = A0;
// const int button_down = A1;
const int button_start_LED = A2;
// const int button_start = A3;


// Constants for buttons -- debounce and analog High voltage
const int analogHigh = 500;
const int debounce = 100;

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
int start_button_pressed = 0;
int up_button_pressed = 0;
int down_button_pressed = 0;
unsigned long lastPress = 0;

void setup(void)
{
  Serial.begin(9600);

  Serial.println("Initialising variables");

  // Initialise Button pins
  pinMode(button_up, INPUT_PULLUP); digitalWrite(button_up, HIGH);
  pinMode(button_down, INPUT_PULLUP); digitalWrite(button_down, HIGH);
  pinMode(button_start, INPUT_PULLUP); digitalWrite(button_start, HIGH);
  pinMode(button_start_LED, OUTPUT); digitalWrite(button_start_LED, LOW);
  
  
  // Initialse Relay pins
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

}


void check_buttons() {

  // read the state of the start button
  int button_start_v = analogRead(button_start);
  int button_up_v = analogRead(button_up);
  int button_down_v = analogRead(button_down);

  Serial.print("Start = " + String(button_start_v));
  Serial.print("\t\tUp =   " + String(button_up_v));
  Serial.println("\t\tDown = " + String(button_down_v));

  // Check to see if the start button is being pressed
  if (button_start_v > analogHigh) {
    // Only respond if it's been a delay since last press
    if (millis() - lastPress > debounce) {  
      start_button_pressed++; 
      digitalWrite(button_start_LED, start_button_pressed % 2);
      }
    lastPress = millis();
  }


  // Check to see if the up button is being pressed
  if (button_up_v > analogHigh) {
    // Only respond if it's been a delay since last press
    if (millis() - lastPress > debounce) {  up_button_pressed++;  }
    lastPress = millis();
  }


  // Check to see if the down button is being pressed
  if (button_down_v > analogHigh) {
    // Only respond if it's been a delay since last press
    if (millis() - lastPress > debounce) {  down_button_pressed++;  }
    lastPress = millis();
  }


}

void loop(void)
{

  check_buttons();
  led_update();
  delay(50);

}

void led_update()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Btn_Up: " + String(up_button_pressed));
  lcd.setCursor(0, 1);
  lcd.print("Btn_Dn:"  + String(down_button_pressed));
}

