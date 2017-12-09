/*
  iAQcore-thingspeak.ino - Upload iAQcore (indoor air quality) measurements to a ThingSpeak channel from an ESP8266.
  Created by Maarten Pennings 2017 Dec 9
*/

/*
This script assumes you
- have (created) an ThingSpeak account
- have (created) a channel for iAQcore measurements
- created 4 fields for that channel:
   + Field 1: eCO2 (ppm)
   + Field 2: status
   + Field 3: resistance (Ω)
   + Field 4: eTVOC (ppb)
- used the "Channel ID" of that channel as initializer for thingspeakChannelId
- used the "Write API Key" of that channel as initializer for thingspeakWriteApiKey
*/


#include <Wire.h>        // I2C library
#include <ESP8266WiFi.h> // ESP8266 WiFi library
#include "ThingSpeak.h"  // In Arduino IDE: Sketch > Include Library > Manage Libraries > ThingSpeak > Install
#include "iAQcore.h"     // Install from https://github.com/maarten-pennings/iAQcore


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
  // File that contains (my secret) credentails for WiFi and ThingSpeak
  #include "credentials.h"
#endif


WiFiClient  client;
iAQcore     iaqcore;


void setup() {
  // Enable serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting iAQcore to ThingSpeak");

  // Enable LED
  led_init();
  Serial.println("init: LED up");
  led_on();
  
  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  Wire.begin(/*SDA*/D2,/*SCL*/D1); 
  Wire.setClockStretchLimit(1000); 
  Serial.println("init: I2C up");
  
  // Enable iAQcore
  bool ok= iaqcore.begin();
  Serial.println(ok ? "init: iAQcore up" : "init: iAQcore ERROR");

  // Enable WiFi
  Serial.print("init: WiFi ");
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


#define IAQCORE_STAT9_ERROR 0x100


void loop() {
  // Signal start-of-measurement
  led_on();
  
  // Read the iAQcore
  uint16_t eco2;
  uint8_t  stat8;
  uint32_t resist;
  uint16_t etvoc;
  bool ok=iaqcore.read(&eco2,&stat8,&resist,&etvoc);
  uint16_t stat9 = ok ? stat8 : (stat8|IAQCORE_STAT9_ERROR); // Raise bit outsode byte to denote I2C error

  // On error, use previous values for iAQcore
  static uint16_t prev_eco2= 450;
  static uint32_t prev_resist= 0x555555;
  static uint16_t prev_etvoc= 125;
  if( stat9 & (IAQCORE_STAT9_ERROR|IAQCORE_STAT_ERROR|IAQCORE_STAT_BUSY) ) {
    eco2= prev_eco2;
    resist= prev_resist;
    etvoc= prev_etvoc;
  }
  prev_eco2= eco2;
  prev_resist= resist;
  prev_etvoc= etvoc;
  
  // Print
  if( stat9 & IAQCORE_STAT9_ERROR ) {
    Serial.println("data: I2C error");
  } else if( stat9 & IAQCORE_STAT_ERROR ) {
    Serial.println("data: chip broken");
  } else if( stat9 & IAQCORE_STAT_BUSY ) {
    Serial.println("data: chip busy");
  } else {
    Serial.print("data: ");
    Serial.print("eco2=");    Serial.print(eco2);     Serial.print(" ppm,  ");
    Serial.print("stat9=0x"); Serial.print(stat9,HEX);Serial.print(",  ");
    Serial.print("resist=");  Serial.print(resist);   Serial.print(" ohm,  ");
    Serial.print("tvoc=");    Serial.print(etvoc);    Serial.print(" ppb  ");
    Serial.println();
  } 

  // Send to ThingSpeak
  ThingSpeak.setField(1,(int)eco2);
  ThingSpeak.setField(2,(int)stat9);
  ThingSpeak.setField(3,(int)resist);
  ThingSpeak.setField(4,(int)etvoc);
  ThingSpeak.writeFields(thingspeakChannelId, thingspeakWriteApiKey);  

  // Signal end-of-measurement
  led_off();
  
  // Do not overfeed ThingSpeak
  delay(60000); 
}


