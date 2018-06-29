/*
  iAQcore-simple.ino - Simple demo sketch (no error handling) printing results of the iAQ-core indoor air quality sensor module with I2C interface from ams.
  Created by Maarten Pennings 2017 Dec 9
*/


#include <Wire.h>   // I2C library
#include "iAQcore.h"


iAQcore iaqcore;


void setup() {
  // Enable serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting iAQ-Core simple demo");

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  //Wire.begin(/*SDA*/D2,/*SCL*/D1); 
  //Wire.setClockStretchLimit(310); 
  
  // Enable I2C for Arduino pro mini or Nano [VDD to VCC/3V3, GND to GND, SDA to A4, SCL to A5]
  Wire.begin(); 

  // Enable iAQ-Core
  iaqcore.begin();
}


void loop() {
  // Read
  uint16_t eco2;
  uint16_t stat;
  uint32_t resist;
  uint16_t etvoc;
  iaqcore.read(&eco2,&stat,&resist,&etvoc);
  
  // Print
  Serial.print("iAQcore: ");
  Serial.print("eco2=");   Serial.print(eco2);     Serial.print(" ppm,  ");
  Serial.print("stat=0x"); Serial.print(stat,HEX); Serial.print(",  ");
  Serial.print("resist="); Serial.print(resist);   Serial.print(" ohm,  ");
  Serial.print("tvoc=");   Serial.print(etvoc);    Serial.print(" ppb");
  Serial.println();
  
  // Wait
  delay(1000); 
}


