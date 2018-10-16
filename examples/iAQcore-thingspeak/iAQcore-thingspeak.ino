/*
  iAQcore-thingspeak.ino - Upload iAQ-Core (indoor air quality) measurements to a ThingSpeak channel from an ESP8266.
  Created by Maarten Pennings 2017 Dec 9
*/
#define VERSION "v10"


/*
This sketch assumes you have
- an ESP8266 with an iAQ-Core attached to I2C (SDA/D2 and SCL/D1)
    Core library version 2.4.2
    Arduino   Board: "NodeMCU 1.0 (ESP-12E Module)"
    Arduino   CPU Frequency: "80 MHz"
- installed the I2C bus clear library (is optional)
   Goto https://github.com/maarten-pennings/I2Cbus, press Download zipfile
   Click Sketch > Include Library > Add .ZIP Library...  then select downloaded zip file
- installed the iAQcore Arduino library 
   Goto https://github.com/maarten-pennings/iAQcore, press Download zipfile
   Click Sketch > Include Library > Add .ZIP Library...  then select downloaded zip file
- installed the ThingSpeak Arduino library 
   Sketch > Include Library > Manage Libraries > ThingSpeak > Install
- a ThingSpeak account
- with a channel for iAQ-Core measurements
- with four fields for that channel:
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


// Wrapper for blinking (off+on) 'blink' times, the total taking `ms` ms.
void led_blink(int blink, int ms ) {
  for( int i=0; i<2*blink; i++ ) { led_tgl(); delay(ms/2); } 
}


void setup() {
  // Give user some time to connect USB serial
  delay(3000);
  
  // Enable serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting iAQ-Core to ThingSpeak " VERSION);

  // Print some version info
  Serial.print("init: core="); Serial.print(ESP.getCoreVersion());
  Serial.print(", sdk="); Serial.print(ESP.getSdkVersion());
  Serial.print(", freq="); Serial.print(ESP.getCpuFreqMHz());
  Serial.println();

  // Enable LED
  led_init();
  Serial.println("init: LED up");
  led_on();

  // I2C bus clear
  Serial.printf("init: I2C bus: %s\n", I2Cbus_statusstr(I2Cbus_clear(SDA,SCL)));

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, SDA to D2, SCL to D1]
  Wire.begin(SDA,SCL); 
  Wire.setClockStretchLimit(1000); // Default is 230us, see line78 of https://github.com/esp8266/Arduino/blob/master/cores/esp8266/core_esp8266_si2c.c
  Serial.println("init: I2C up");
  
  // Enable iAQ-Core
  bool ok= iaqcore.begin();
  Serial.println(ok ? "init: iAQ-Core up" : "init: iAQ-Core ERROR");

  // Enable WiFi
  Serial.printf("init: MAC %s\n",WiFi.macAddress().c_str());
  Serial.print("init: WiFi '");
  Serial.print(wifiSsid);
  Serial.print("' ..");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  while( true ) {
    if( WiFi.status()==WL_CONNECTED ) break;
    led_tgl();
    delay(250);
    Serial.printf(".");
  }
  Serial.printf(" up (%s)\n",WiFi.localIP().toString().c_str());

  // Enable ThingSpeak
  ThingSpeak.begin(client);
  Serial.println("init: ThingSpeak up");

  // End of setup() - delay helps distinguishing LED flashes
  led_off();
  delay(3000);
}


uint16_t eco2= 450;
uint16_t stat;
uint32_t resist= 0xA5A5A5;
uint16_t etvoc= 125;


void loop() {
  // Read the iAQ-Core
  iAQcore_read(&eco2,&stat,&resist,&etvoc);
  
  // Measurement feedback
  if( stat & IAQCORE_STAT_I2CERR ) {
    Serial.print("data: I2C error");
    led_blink(3,100);
  } else if( stat & IAQCORE_STAT_ERROR ) {
    Serial.print("data: chip broken");
    led_blink(2,100);
  } else if( stat & IAQCORE_STAT_BUSY ) {
    Serial.print("data: chip busy");
    led_blink(1,100);
  } else {
    Serial.print("data: ");
    Serial.print("eco2=");   Serial.print(eco2);     Serial.print(" ppm,  ");
    Serial.print("stat=0x"); Serial.print(stat,HEX); Serial.print(",  ");
    Serial.print("resist="); Serial.print(resist);   Serial.print(" ohm,  ");
    Serial.print("tvoc=");   Serial.print(etvoc);    Serial.print(" ppb");
  } 

  // Prepare ThingSpeak package
  ThingSpeak.setField(1,(int)eco2);
  ThingSpeak.setField(2,(int)stat);
  ThingSpeak.setField(3,(int)resist);
  ThingSpeak.setField(4,(int)etvoc);

  // Send to ThingSpeak 
  led_on();
  int http= ThingSpeak.writeFields(thingspeakChannelId, thingspeakWriteApiKey);  
  led_off();

  // ThingSpeak upload feedback
  Serial.print(",  http=");
  Serial.print(http);
  Serial.print(",  wifi=");
  Serial.println(WiFi.status());
  if( http!=200 ) led_blink(20,50); // Flash to show upload failure
  
  // Do not overfeed ThingSpeak
  delay(60000); 
}


