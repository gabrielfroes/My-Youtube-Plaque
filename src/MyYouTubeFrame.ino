// *****************************************************
// MY YOUTUBE PLAQUE STATS COUNTER
// Year: 2019 
// Authors: Gabriel Froes & Vanessa Weber
// Youtube / Instagram: @codigofontetv
// Created for #CDFTV channel 100K subs celebration
// *****************************************************

// Required Libs
#include <stdlib.h>
#include <string.h>
#include <Wire.h>
#include <YoutubeApi.h>             // https://github.com/witnessmenow/arduino-youtube-api
#include <ArduinoJson.h>            // https://github.com/bblanchon/ArduinoJson
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"

// Display
LiquidCrystal_I2C lcd( 0x27, 16, 2 );  // Use I2CScan to discover your address. Mine is: 0x27 for a 16 chars and 2 line display

// Wifi
WiFiClientSecure client;

// Youtube API
YoutubeApi    api( API_KEY, client );
unsigned long api_mtbs = 20000;     // (20s) Time between API requests
unsigned long api_lasttime;         // Last time API request has been hit

// Misc
unsigned long display_mtbs = 5000; // (5s) Time to change channel info in display

// *****************************************************
// Arduino setup function
// *****************************************************
void setup() {
  Serial.begin(9600);

  // ------------------------
  // 1. START DISPLAY
  // ------------------------    
  lcd.init();

  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

  print_display(CHANNEL_SHORT_NAME, VERSION);
  
  // ------------------------
  // 2. CONNECT WIFI
  // ------------------------
  WiFi.mode( WIFI_STA );
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wi-fi network:
  Serial.print("\nConnecting to:");
  Serial.println(WIFI_SSID);
  print_display("Connecting Wi-fi:", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("Wi-fi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  print_display( "Connected!", "" );
}
// *****************************************************

// *****************************************************
// Arduino loop function
// *****************************************************
void loop() {
  if (millis() > api_lasttime + api_mtbs) {
    print_display("Loading...", "");
    Serial.println("Loading...");
    if(api.getChannelStatistics(CHANNEL_ID)) {
      api_lasttime = millis();
      Serial.println("-----Channel Stats-----");
      Serial.print("Subscribers Count: ");
      Serial.println(api.channelStats.subscriberCount);
      Serial.print("Views Count: ");
      Serial.println(api.channelStats.viewCount);
      Serial.print("Videos Count: ");
      Serial.println(api.channelStats.videoCount);
      Serial.println("------------------------");

      // Print on Display  
      for (int i=0; i<=(api_mtbs/display_mtbs); i++) {
        print_stats("Subscribers:", api.channelStats.subscriberCount);
        print_stats("Views:", api.channelStats.viewCount);
        print_stats("Videos:", api.channelStats.videoCount);
      }
    } else {
      print_display("Oops!", "Trying again...");
      delay(2000);
      api_lasttime = 0;
    }
  }
}
// *****************************************************

// *****************************************************
// Set Message on Display
// *****************************************************
void print_stats(String msg, long val){
  print_display( msg,  String(val) );
}
void print_display(String msg1, String msg2){
	    lcd.clear();
      lcd.home ();
      lcd.print(msg1);  
      lcd.setCursor (0, 1); // next line
      lcd.print (msg2);
      delay(display_mtbs);
}
// *****************************************************