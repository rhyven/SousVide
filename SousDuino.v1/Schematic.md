# Description of schematic

Pinout for Arduino Nano v3 is at https://www.arduino.cc/en/uploads/Main/ArduinoNano30Schematic.pdf

240v power source goes directly to the relay (RY1), as well as to the 5vdc transformer (T1)

Transformer (T1) powers the water pump (M1), as well as connecting to VIN (J2.1) and GND (J2.2) pins on the Arduino (U1).

Heating element (R1) is run by the outputs on the relay (RY1).

Thermocouple (X1):
 * VCC is connected into 3.3V out (J2.14)
 * GND is connected to GND (J2.2)
 * SCK is connected to D4 (J1.7)
 * CS is connected to D3 (J1.6)
 * SO is connected to D2 (J1.5)
 
Relay (RY1):
 * VCC is connected to 5v out (J2.4)
 * GND is connected to GND (J2.2)
 * IN1 is connected to D11 (J1.14)
 * IN2 is connected to D12 (J1.15)
