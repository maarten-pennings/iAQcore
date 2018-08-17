/*
  iAQcore-thingspeak.ino - Upload iAQ-Core (indoor air quality) measurements to a ThingSpeak channel from an ESP8266.
  Created by Maarten Pennings 2017 Dec 9
*/
#define VERSION "v7"


/*
This sketch assumes you have
- an ESP8266 with an iAQ-Core attached to I2C (SDA/D2 and SCL/D1)
- installed the I2C bus clear library (is optional)
   Goto https://github.com/maarten-pennings/I2Cbus, press Download zipfile
   Click Sketch > Include Library > Add .ZIP Library...  then select downloaded zip file
- installed the iAQcore Arduino library 
   Goto https://github.com/maarten-pennings/iAQcore, press Download zipfile
   Click Sketch > Include Library > Add .ZIP Library...  then select downloaded zip file
- installed the ThingSpeak Arduino library 
   Sketch > Include Library > Manage Libraries > ThingSpeak > Install
- a ThingSpeak account
- a channel for iAQ-Core measurements
- created 4 fields for that channel:
   + Field 1: eCO2 (ppm)
   + Field 2: status
   + Field 3: resistance (Ω)
   + Field 4: eTVOC (ppb)
- used the "Channel ID" of that channel as initializer for thingspeakChannelId (see below)
- used the "Write API Key" of that channel as initializer for thingspeakWriteApiKey (see below)
*/


#include <Wire.h>        // I2C library
#include <I2Cbus.h>      // I2Cbus library
#include <ESP8266WiFi.h> // ESP8266 WiFi library
#include "ThingSpeak.h"  // ThingSpeak library
#include "iAQcore.h"     // iAQcore library
#define LED_PIN    D4    // GPIO2 == D4 == standard BLUE led available on most NodeMCU boards (LED on == D4 low)
#define led_init() pinMode(LED_PIN, OUTPUT)
#define led_on()   digitalWrite(LED_PIN, LOW)
#define led_off()  digitalWrite(LED_PIN, HIGH)
#define led_tgl()  digitalWrite(LED_PIN, (HIGH+LOW)-digitalRead(LED_PIN) );


#if 0
  // Fill out the credentials of your local WiFi Access Point
  const char *  wifiSsid              = "xxxxx"; // Your WiFi network SSID name
  const char *  wifiPassword          = "xxxxx"; // Your WiFi network password
  // Fill out the credentials of your ThingSpeak channel
  unsigned long thingspeakChannelId   = 1234567; // Your ThingSpeak Channel ID 
  const char *  thingspeakWriteApiKey = "xxxxx"; // Your ThingSpeak write api key
#else
  // File that contains (my secret) credentials for WiFi and ThingSpeak
  #include "credentials.h"
#endif


WiFiClient  client;
iAQcore     iaqcore;


// Wrapper calling iaqcore.read. It updates the arguments when the read was successful.
void iAQcore_read(uint16_t * eco2, uint16_t * stat, uint32_t * resist, uint16_t * etvoc) {
  // Read the iAQ-Core
  uint16_t _eco2;
  uint16_t _stat;
  uint32_t _resist;
  uint16_t _etvoc;
  iaqcore.read(&_eco2,&_stat,&_resist,&_etvoc);
  
  // Use new values
  if( _stat & (IAQCORE_STAT_I2CERR|IAQCORE_STAT_ERROR|IAQCORE_STAT_BUSY) ) {
    // Errors, so keep old values except stat
    *stat= _stat;
  } else {
    *eco2= _eco2;
    *stat= _stat;
    *resist= _resist;
    *etvoc= _etvoc;
  }
}


void setup() {
  // Enable serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting iAQ-Core to ThingSpeak " VERSION);

  // Enable LED
  led_init();
  Serial.println("init: LED up");
  led_on();

  // I2C bus clear
  Serial.printf("init: I2C bus: %s\n", I2Cbus_statusstr(I2Cbus_clear(SDA,SCL)));

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  Wire.begin(SDA,SCL); 
  Wire.setClockStretchLimit(1000); // 500ms is not enough
  Serial.println("init: I2C up");
  
  // Enable iAQ-Core
  bool ok= iaqcore.begin();
  Serial.println(ok ? "init: iAQ-Core up" : "init: iAQ-Core ERROR");

  // Enable WiFi
  Serial.printf("init: MAC %s\n",WiFi.macAddress().c_str());
  Serial.print("init: WiFi '");
  Serial.print(wifiSsid);
  Serial.print("' ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  while( true) {
    if( WiFi.status()==WL_CONNECTED ) break;
    led_tgl();
    delay(250);
    Serial.printf(".");
  }
  Serial.printf(" up (%s)\n",WiFi.localIP().toString().c_str());

  // Enable ThingSpeak
  ThingSpeak.begin(client);
  Serial.println("init: ThingSpeak up");
  
  led_off();
}


uint16_t eco2= 450;
uint16_t stat;
uint32_t resist= 0xA5A5A5;
uint16_t etvoc= 125;


void loop() {
  // Signal start-of-measurement
  led_on();
  
  // Read the iAQ-Core
  iAQcore_read(&eco2,&stat,&resist,&etvoc);
  
  // Print
  if( stat & IAQCORE_STAT_I2CERR ) {
    Serial.println("data: I2C error");
  } else if( stat & IAQCORE_STAT_ERROR ) {
    Serial.println("data: chip broken");
  } else if( stat & IAQCORE_STAT_BUSY ) {
    Serial.println("data: chip busy");
  } else {
    Serial.print("data: ");
    Serial.print("eco2=");   Serial.print(eco2);     Serial.print(" ppm,  ");
    Serial.print("stat=0x"); Serial.print(stat,HEX); Serial.print(",  ");
    Serial.print("resist="); Serial.print(resist);   Serial.print(" ohm,  ");
    Serial.print("tvoc=");   Serial.print(etvoc);    Serial.print(" ppb  ");
    Serial.println();
  } 

  // Send to ThingSpeak
  ThingSpeak.setField(1,(int)eco2);
  ThingSpeak.setField(2,(int)stat);
  ThingSpeak.setField(3,(int)resist);
  ThingSpeak.setField(4,(int)etvoc);
  ThingSpeak.writeFields(thingspeakChannelId, thingspeakWriteApiKey);  

  // Signal end-of-measurement
  led_off();
  
  // Do not overfeed ThingSpeak
  delay(60000); 
}


