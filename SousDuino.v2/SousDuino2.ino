// Eric's SousDuino controller, version 2
// Written June 2015 by Eric Light

int TARGET_TEMP = 22;

#include <OneWire.h>
#include <DallasTemperature.h>

// This version requires a DS18B20 temperature probe, and uses the OneWriteNoResistor library available at josh.com

// Your DS18B20 should be wired on digital pins 8 (+5v), 9 (data), and 10 (GND).
// If using just a plain sensor, the flat side of the sensor should face into the center of the board.
// More info about the NoResistor library at http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892

// Thermocouple (DS18B20) PIN labels
int ds_power = 8;
int ds_data = 9;
int ds_gnd = 10;

// Relay (SSR) PIN labels
int relay_power = 11;
int relay_gnd = 12;

#define ONE_WIRE_BUS ds_data
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // Initialise DS18B20 pins
  digitalWrite(ds_power, LOW); pinMode(ds_power, OUTPUT);
  digitalWrite(ds_gnd, LOW); pinMode(ds_gnd, OUTPUT);
  
  // Initialse Relay pins
  digitalWrite(relay_power, HIGH); pinMode(relay_power, OUTPUT);
  digitalWrite(relay_gnd, LOW); pinMode(relay_gnd, OUTPUT);
  Serial.begin(9600);
}


void loop(void)
{ 

  DeviceAddress thermocouple; // We'll use this variable to store a found device address    
  sensors.getAddress(thermocouple, 0);

  // Fix this, shouldn't be here  
  sensors.begin();
  
  // Probably want to put the following into a while loop
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempC(thermocouple);
  Serial.println(tempC);
  
  if (tempC < TARGET_TEMP)
    { digitalWrite(relay_power, LOW); }
  else 
    { digitalWrite(relay_power, HIGH); }
  
  // Add a delay here

}
