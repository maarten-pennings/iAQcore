/*
  iaqcore.cpp - Library for the iAQ-core indoor air quality sensor module with I2C interface from ams.
  Created by Maarten Pennings 2017 Dec 9
*/


#include <arduino.h>
#include <Wire.h>
#include "iAQcore.h"


// Chip constants
#define IAQCORE_I2CADDR          0x5A  // 7-bit I2C slave address of the iAQcore
#define IAQCORE_SIZE             9     // Size (in bytes) of the measurement data block 

// To help users, this library can print error messages
#define PRINTLN(msg)             Serial.println(msg)
//#define PRINTLN(msg)             // nothing


// Checks if the iAQcore is accessible (and if the Wire library is configured for clock-stretching).
bool iAQcore::begin(void) {
  // Check if device is accessible (by getting a measurement daat block)
  uint32_t resist;
  bool ok= read(0,0,&resist,0);  
  if( !ok ) { PRINTLN("iaqcore: Could not access iAQ-core chip"); return false; }
  // Check if the wire library does wait long enough for the clock stretching needed by the iAQcore (data sheet specs that MSB of resist is 0x00)
  if( (resist>>24)==0xFF )  { PRINTLN("iaqcore: Is there 'Wire.setClockStretchLimit(1000)' after 'Wire.begin()'?"); return false; }
  // All ok
  return true;
}


// Reads measurement date from the iAQcore chip (returns true if successful). Any parameter may be NULL.
bool iAQcore::read(uint16_t * eco2, uint8_t * stat, uint32_t * resist, uint16_t * etvoc ) {
  // Trye to read the measurement data block.
  int num= Wire.requestFrom(IAQCORE_I2CADDR,IAQCORE_SIZE,true); 
  if( num != IAQCORE_SIZE ) { PRINTLN("iaqcore: iaqcore_read failed"); return false; }
  // Get bytes into local buf
  uint8_t buf[IAQCORE_SIZE];
  for( int i=0; i<IAQCORE_SIZE; i++ ) buf[i]= Wire.read();
  // Pack into output parameters (little endian)
  if( eco2  !=0 ) *eco2  =  (buf[0]<<8) + (buf[1]<<0);
  if( stat  !=0 ) *stat  =  (buf[2]<<0);
  if( resist!=0 ) *resist=  ((uint32_t)buf[3]<<24) + ((uint32_t)buf[4]<<16) + ((uint32_t)buf[5]<<8) + ((uint32_t)buf[6]<<0); 
  if( etvoc !=0 ) *etvoc =  (buf[7]<<8) + (buf[8]<<0);
  // Success
  return true;
}

