/*
  iaqcore.h - Library for the iAQ-core indoor air quality sensor module with I2C interface from ams.
  Created by Maarten Pennings 2017 Dec 9
*/
#ifndef _IAQCORE_H_
#define _IAQCORE_H_


#include <stdbool.h>
#include <stdint.h>


// The status (`stat` output parameter of `read`) uses the following flags0x00: OK (data valid)
#define IAQCORE_STAT_OK     0x0000 // Data ok
#define IAQCORE_STAT_BUSY   0x0001 // Data might be inconsistent; re-read()
#define IAQCORE_STAT_RUNIN  0x0010 // iAQcore chip in warm up phase
#define IAQCORE_STAT_ERROR  0x0080 // Sensor might be broken
#define IAQCORE_STAT_I2CERR 0x0100 // Software added error: I2C transaction error


class iAQcore {
  public: // Main API functions
    // Checks if the iAQcore is accessible (and if the Wire library is configured for clock-stretching).
    bool begin(void);
    // Reads measurement date from the iAQcore chip. Any parameter may be NULL.
    void read(uint16_t * eco2, uint16_t * stat, uint32_t * resist, uint16_t * etvoc ) ;
};


#endif
