int TARGET_TEMP = 22;

#include <OneWire.h>
#include <DallasTemperature.h>

// Updated SousDuino controller, v2

// This version requires a DS18B20 temperature probe, and uses the OneWriteNoResistor library available at josh.com

// Your DS18B20 should be wired on digital pins 8 (+5v), 9 (data), and 10 (GND).
// If using just a plain sensor, the flat side of the sensor should face into the center of the board.
// More info about the NoResistor library at http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892

#define ONE_WIRE_BUS 9
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// And pass our oneWire reference to Dallas Temperature. 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  // Initialise DS18B20 pins
  digitalWrite( 8 , LOW ); pinMode( 8  , OUTPUT );
  digitalWrite( 10 , LOW ); pinMode( 10 , OUTPUT );
  
  // Initialse Relay pins
  digitalWrite(11, HIGH); pinMode(11, OUTPUT);
  digitalWrite(12, LOW); pinMode(12, OUTPUT);
  Serial.begin(9600);
}


void loop(void)
{ 

  DeviceAddress thermoprobe; // We'll use this variable to store a found device address    
  sensors.getAddress(thermoprobe, 0);
  
  sensors.begin();
  
  // while loop
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempC(thermoprobe);
  Serial.println(tempC);
  
  if (tempC < TARGET_TEMP)
    { digitalWrite(11, LOW); }
  else 
    { digitalWrite(11, HIGH); }
  
  // if tempC > setpoint then relays off
  // else relays on
}
