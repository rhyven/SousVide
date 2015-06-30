// Eric's Sous Vide controller
// Written June 2015 by Eric Light, based off code from Adafruit

// Set the target temperature here
int TARGET_TEMP = 24;

// Controller code lives below

// Pinout for MAX6675 thermocouple
int thermoDO = 2;  // Data out; may be labelled "SO"
int thermoCS = 3;  // Usually labelled "CS"
int thermoCLK = 4;  // Clock signal; may be labelled "SCK"

// The MAX6675 library sets up the pins for reading
#include "max6675.h"
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Pins for two-channel relay
int relay1=11; int relay2=12; int gndPin = 4;

void setup() {
  Serial.begin(9600);
  
  // Setup relay pins for output
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  pinMode(relay1, OUTPUT); pinMode(relay2, OUTPUT);
  
  // Turn off the relays immediately (they'll still power up briefly)
  power_off();
  
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {

  // Print the current temperature for anyone watching serial comms
  Serial.print("C = "); 
  Serial.println(thermocouple.readCelsius());

  // Check the temp and respond accordingly
  if (thermocouple.readCelsius() < TARGET_TEMP)
    { power_on(); }
  else 
    { power_off(); }
   
  // Only toggle twice a second; otherwise the relays will burn out
  delay(500);
}

void power_on() {
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);  
}

void power_off() {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
}
