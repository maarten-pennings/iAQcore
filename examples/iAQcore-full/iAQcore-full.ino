/*
  iAQcore-simple.ino - Full demo sketch (including error handling) printing results of the iAQ-core indoor air quality sensor module with I2C interface from ams.
  Created by Maarten Pennings 2017 Dec 9
*/


#include <Wire.h>   // I2C library
#include "iAQcore.h"


iAQcore iaqcore;


void setup() {
  // Enable serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting iAQcore simple demo");

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  //Wire.begin(/*SDA*/D2,/*SCL*/D1); 
  //Wire.setClockStretchLimit(1000); 
  
  // Enable I2C for Arduino pro mini or Nano [VDD to VCC/3V3, GND to GND, SDA to A4, SCL to A5]
  Wire.begin(); 

  // Enable iAQcore
  bool ok= iaqcore.begin();
  Serial.println(ok ? "iAQcore initialized" : "ERROR initializing iAQcore");
}


void loop() {
  // Read
  uint16_t eco2;
  uint16_t stat;
  uint32_t resist;
  uint16_t etvoc;
  iaqcore.read(&eco2,&stat,&resist,&etvoc);
  
  // Print
  if( stat & IAQCORE_STAT_I2CERR ) {
    Serial.println("iAQcore: I2C error");
  } else if( stat & IAQCORE_STAT_ERROR ) {
    Serial.println("iAQcore: chip broken");
  } else if( stat & IAQCORE_STAT_BUSY ) {
    Serial.println("iAQcore: chip busy");
  } else {
    Serial.print("iAQcore: ");
    Serial.print("eco2=");    Serial.print(eco2);     Serial.print(" ppm,  ");
    Serial.print("tvoc=");    Serial.print(etvoc);    Serial.print(" ppb  ");
    Serial.print("(resist="); Serial.print(resist);   Serial.print(" ohm)  ");
    if( stat&IAQCORE_STAT_RUNIN ) Serial.print(" RUNIN");
    Serial.println();
  } 

  // Wait
  delay(1000); 
}


