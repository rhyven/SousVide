// Eric's SousDuino controller, version 2
// Written June 2015 by Eric Light

int TARGET_TEMP = 24;

#include <OneWire.h>
#include <DallasTemperature.h>

// This version requires a DS18B20 temperature probe, and uses the OneWriteNoResistor library available at https://github.com/bigjosh/OneWireNoResistor/
// You'll also need the Dallas DS18 library from https://github.com/milesburton/Arduino-Temperature-Control-Library/

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

// Use LED to indicate heating
int led = 13;

#define ONE_WIRE_BUS ds_data
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermocouple;

void setup(void)
{
  Serial.begin(9600);
  
  // Initialise DS18B20 pins
  pinMode(ds_power, OUTPUT); pinMode(ds_gnd, OUTPUT);
  digitalWrite(ds_power, LOW); digitalWrite(ds_gnd, LOW);
  
  // Initialse Relay pins
  pinMode(relay_power, OUTPUT); pinMode(relay_gnd, OUTPUT);
  digitalWrite(relay_power, HIGH); digitalWrite(relay_gnd, LOW); 
 
  // Initialise LED pin
  pinMode(led, OUTPUT);
  
}


void loop(void)
{ 

  sensors.getAddress(thermocouple, 0);
  sensors.begin();
  
  sensors.requestTemperatures(); // Send the command to get temperature
  float tempC = sensors.getTempC(thermocouple);  // Convert temperature into Celcius
  
  if (tempC < TARGET_TEMP ) {
     Serial.print("Heating... ");
     digitalWrite(led, HIGH);
     digitalWrite(relay_power, HIGH); 
  }
  else {
    Serial.print("Cooling... ");
    digitalWrite(led, LOW);
    digitalWrite(relay_power, LOW);   
  }
  
  Serial.println(tempC);

  delay(200);
}


