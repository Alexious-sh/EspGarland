#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "WiFiUdp.h"
#include "NTPClient.h"
#include "ArduinoOTA.h"

//typedef void (*blink_func)();
void (*effects[3])(void) = {&blink1, &blink2, blink3};

#define LED_PIN D8

unsigned int blinkTicksLeft = 0;
unsigned int blinkIdx = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Booting");

  // Setup WiFi Access Point
  IPAddress apIP(192, 168, 4, 1);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  while (!WiFi.softAP("EspGarland", "12345678")) {
    Serial.println("Connection Failed! Rebooting after 2 sec...");
    delay(2000);
    ESP.restart();
  }

  MDNS.begin("esp-garland");

  ArduinoOTA.setHostname("esp-garland");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  
  Serial.println("Ready");
}

void loop() {
  ArduinoOTA.handle();
  
  random_blink();
}

void random_blink() {
  if (blinkTicksLeft <= 0) {
    blinkIdx = random(0, 2);
    blinkTicksLeft = random(3, 5);
  }
  
  effects[blinkIdx]();

  //Serial.println("Tick!");

  blinkTicksLeft--;
}


// ============= EFFECTS =============

void blink1() {
  // Switch lights ON
  digitalWrite(LED_PIN, HIGH);
  // Wait for 500 ms
  delay(500);
  // Switch lights OFF
  digitalWrite(LED_PIN, LOW);
  delay(500);
}

void blink2() {
  // Animate LED brightness UP from 0 to 255 (MAX)
  for (int i=0; i<=255; i++) {
    // Set led brightness (0...255)
    analogWrite(LED_PIN, i);
    // Wait for 5 ms
    delay(5);
  }

  // Animate LED brightness DOWN from 255 to 0
  for (int i=255; i>=0; i--) {
    analogWrite(LED_PIN, i);
    delay(5);
  }
}

void blink3() {
  // Let's randomize some stuff
  unsigned int blinkInterval = random(200, 1000);
  digitalWrite(LED_PIN, HIGH);
  delay(blinkInterval / 2);
  digitalWrite(LED_PIN, LOW);
  delay(blinkInterval / 2);
}
