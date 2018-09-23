/*
  iAQcore.cpp - Library for the iAQ-Core indoor air quality sensor module with I2C interface from ams.
  Created by Maarten Pennings 2017 Dec 9
*/


#include <Arduino.h>
#include <Wire.h>
#include "iAQcore.h"


// Chip constants
#define IAQCORE_I2CADDR          0x5A  // 7-bit I2C slave address of the iAQ-Core
#define IAQCORE_SIZE             9     // Size (in bytes) of the measurement data block 

// To help users, this library can print error messages
#define PRINTLN(msg)             Serial.println(msg)
//#define PRINTLN(msg)             // nothing


// Checks if the iAQ-Core is accessible (and if the Wire library is configured for clock-stretching).
bool iAQcore::begin(void) {
  // Check if device is accessible (by getting a measurement data block)
  uint16_t stat;
  uint32_t resist;
  read(0,&stat,&resist,0);  
  if( stat & IAQCORE_STAT_I2CERR ) { PRINTLN("iAQcore: begin(): Could not access iAQ-Core chip (I2C)"); return false; }
  // Check if the wire library does wait long enough for the clock stretching needed by the iAQ-Core (data sheet specs that MSB of resist is 0x00)
  if( (resist>>24)==0xFF )  { PRINTLN("iAQcore: begin(): Is there 'Wire.setClockStretchLimit()' after 'Wire.begin()'?"); return false; }
  // All ok
  return true;
}


// Reads measurement date from the iAQ-Core chip (returns true if successful). Any parameter may be NULL.
void iAQcore::read(uint16_t * eco2, uint16_t * stat, uint32_t * resist, uint16_t * etvoc ) {
  // Try to read the measurement data block.
  int num= Wire.requestFrom((uint8_t)IAQCORE_I2CADDR,(size_t)IAQCORE_SIZE); 
  // Get bytes into local buf
  uint8_t buf[IAQCORE_SIZE];
  for( int i=0; i<IAQCORE_SIZE; i++ ) buf[i]= Wire.read();
  // Pack into output parameters (little-endian)
  if( eco2  !=0 ) *eco2  =  (buf[0]<<8) + (buf[1]<<0);
  if( stat  !=0 ) *stat  =  ( num==IAQCORE_SIZE ? 0 : IAQCORE_STAT_I2CERR ) + (buf[2]<<0); // Add I2C status to chip status
  if( resist!=0 ) *resist=  ((uint32_t)buf[3]<<24) + ((uint32_t)buf[4]<<16) + ((uint32_t)buf[5]<<8) + ((uint32_t)buf[6]<<0); 
  if( etvoc !=0 ) *etvoc =  (buf[7]<<8) + (buf[8]<<0);
}

