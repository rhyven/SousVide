/*
  Sketch: SIM800 Serial Debug

  Function: This sketch is used to communicate with SIM800 with AT commands.
*********************************************************************************
  note: the following pins has been used and should not be used for other purposes.
  pin 8   // tx pin
  pin 7   // rx pin
  pin 9   // power key pin
  pin 12  // power status pin
*********************************************************************************
  created on 2013/12/5, version: 0.1
  by lawliet.zou(lawliet.zou@gmail.com)
*/

// #include "gprs.h"
#include <SoftwareSerial.h>

// GPRS mySIM(9600);//BaudRate

SoftwareSerial mySIM(3, 4);

void setup() {
  Serial.begin(9600);
  Serial.println("Serial debug begin...");
  //  mySIM.preInit();//Power on SIM800
  //  while(0 != mySIM.sendATTest())
  //  {
  //      Serial.println("sim800 init error!");
  //  }
  //  Serial.println("sim800 init O.K!");

  mySIM.begin(9600);
  mySIM.write("AT\n");

}

void loop() {

  char test[32];

  if (mySIM.available()) {
    Serial.write(mySIM.peek());
    Serial.write(mySIM.read());
    
  }
  if (Serial.available()) {
    mySIM.write(Serial.read());
  }

  //  mySIM.serialDebug();
}
