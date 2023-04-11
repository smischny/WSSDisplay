#include <WiFi.h>
#include "Arduino.h"
#include <WSSDisplayServer.h>
#include <WSSDisplay.h>

WSSDisplay         display;
WSSDisplayServer   displayServer(display);

// Uncomment and fill in your Wifi SID/Password here
//#define WIFI_SID        "YOUR_SID"
//#define WIFI_PASSWORD   "YOUR_PASSWORD"

#if !defined(WIFI_SID) || !defined(WIFI_PASSWORD)
    #error You must define WIFI_SID and WIFI_PASSWORD
#endif

void initDisplay(uint8_t num) {

  display.fillScreen(TFT_BLACK);

  display.drawRect(10, 
                   10, 
                   display.width() - 20, 
                   display.height() - 20, 
                   TFT_WHITE);

  display.setTextColor(TFT_WHITE);
  display.setTextSize(2);
  display.setCursor(114,152);
  display.print("WSSDisplay: Hello World");
  display.flush();

}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting");

  WiFi.enableAP(false);
  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SID, WIFI_PASSWORD);

  while ((WiFi.status() == WL_IDLE_STATUS) || (WiFi.status() == WL_DISCONNECTED)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if ((WiFi.status() == WL_NO_SSID_AVAIL) ||
      (WiFi.status() == WL_CONNECT_FAILED))  {
    Serial.println("Could not connect to WiFi");
    delay(10 * 60 * 1000);

    ESP.restart();
  }

  Serial.println("Connected to Wifi");
  
  Serial.print("Browse to http:\\\\");
  Serial.println(WiFi.localIP());

  display.refreshDisplayFunction(initDisplay);

  displayServer.begin();


}

void loop() {

  displayServer.handleClient();

  delay(50);
}
