// Eric's SousDuino controller, version 2
// Written June 2015 by Eric Light

int TARGET_TEMP = 60;

#include <OneWire.h>
#include <DallasTemperature.h>

// This version requires a DS18B20 temperature probe, and uses the OneWriteNoResistor library available at https://github.com/bigjosh/OneWireNoResistor/
// You'll also need the Dallas DS18 library from https://github.com/milesburton/Arduino-Temperature-Control-Library/

// Your DS18B20 should be wired on digital pins 8 (+5v), 9 (data), and 10 (GND).
// If using just a plain sensor, the flat side of the sensor should face into the center of the board.
// More info about the NoResistor library at http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892

// Thermocouple (DS18B20) PIN labels
int ds_data = 8;

// Relay (SSR) PIN labels
int relay = 11;

#define ONE_WIRE_BUS ds_data
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermocouple;


#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);


bool HEATING = false;

void setup(void)
{
  Serial.begin(9600);



  // Initialse Relay pins
  pinMode(relay, OUTPUT);  digitalWrite(relay, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

}


void loop(void)
{
  // Read temperature
  sensors.getAddress(thermocouple, 0);
  sensors.begin();
  sensors.requestTemperatures(); // Send the command to get temperature
  float tempC = sensors.getTempC(thermocouple);  // Convert temperature into Celcius


  // Make sure we're getting usable readings
  if (tempC < 0) {
    Serial.print("Unplugged");
    relay_off();
    led_unplugged();
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

