// Librarys
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// TSL2591 Setup
#include <Wire.h>
#include <Adafruit_TSL2591.h>
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
uint16_t lux = 0;


// DHT Setup
#include "DHT.h"
#define DHTTYPE DHT22
//#define DHTTYPE DHT11
//#define DHTTYPE DHT21
uint8_t DHTPin = D6;
DHT dht(DHTPin, DHTTYPE);
float Temperature = 0;
float Humidity = 0;

// Board and WiFi Info
const char* ssid = "****"; // WLAN Name
const char* passwd = "****"; // WLAN Pass
const String station = "Wethearstation"; // Station Name
const char* host = "https://***"; // Address to post

// ESP Setup
ESP8266WebServer server(80);
int sleeptime = 900000;

void setup() {
  Serial.begin(115200);
  
  // Main Logic
  wifi_routine();
  tsl_read();
  dht_read();
  postdata();

  // Starts DeepSleepMode
  ESP.deepSleep(sleeptime);  
}

void loop() {}

void wifi_routine() {
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) delay(1);
}

void tsl_read() {
  if (tsl.begin()) {
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);

  uint32_t lum = tsl.getFullLuminosity();
  lux = lum & 0xFFFF;
  }
}

void dht_read() {
  pinMode(DHTPin, INPUT);
  dht.begin();
  delay(100);
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
}

void postdata(){
  HTTPClient http;
  
  String postData = "station=" + station 
  + "&temp=" + (String)Temperature
  + "&luft=" + (String)Humidity;

  // Post to Host
  http.begin(host);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);
  String payload = http.getString();

  http.end();
}
