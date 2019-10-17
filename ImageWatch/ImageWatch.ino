/*
 Analog Watch for M5Stack.

 'BackgroundLayer.cpp' and 'BackgroundLayer.h' are required.

 If you want to use your own image, prepare a image.png file(size 320x240).
 After that, convert to image.png.h using the following tool.(png2code.py)

 https://github.com/mongonta0716/M5Stack_Test_tools

 Based on a sketch by M5StickC's example TFT_Clock 25/9/2019
 Based on a sketch by Gilchrist 6/2/2014 1.0
 */

#include <M5Stack.h>
#include <M5StackUpdater.h>
#include <time.h>
#include <WiFi.h>
#include "BackgroundLayer.h"
#include "image1.png.h"
#include "image2.png.h"
#include "image3.png.h"

TFT_bgLayer bg = TFT_bgLayer(&M5.Lcd);

// ----------------------------------------------------------------------------------
// Initial Settings
// Array index : 0:hour 1:minute 2:second
// ----------------------------------------------------------------------------------
const char ntpServer[] = "ntp.nict.jp";                 // set URL of NTP Server

const uint8_t  brightness = 30;                         // LCD brightness(0 - 100)
const uint8_t  rotation = 1;                            // LCD Rotation(0 - 3)
const uint16_t clockx = 160;                            // clock center X
const uint16_t clocky = 120;                            // clock center Y
const uint16_t len[3] = { 50, 60, 70 };                 // Size of clock hands
const uint16_t colorhh = M5.Lcd.color565(0, 0, 0);      // Color of clock hand(hour)
const uint16_t colormm = M5.Lcd.color565(0, 0, 0);      // Color of clock hand(minute)
const uint16_t colorss = M5.Lcd.color565(255, 0, 0);    // Color of clock hand(second)

// *** Set your WiFi Settings. ***
// If you are launching from M5Stack LovyanLauncher, WiFi settings are not required.
// https://github.com/lovyan03/M5Stack_LovyanLauncher
const char ssid[] = "";        // set your SSID
const char pass[] = "";        // set your passphrase

// ----------------------------------------------------------------------------------
// End of Initial Settings
// ----------------------------------------------------------------------------------

float x[3][3], y[3][3]; // For coordinate calculation
float deg[3];           // Degree of clock hands
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hms[3] = { 0, 0, 0 };  // Get H, M, S from RTC
uint16_t handscolor[3] = {colorhh, colormm, colorss};
uint16_t ox[3] = { 0, 0, 0 } ,oy[3] = { 0, 0, 0 };           // Outer coordinates of clock hands
uint16_t cx[3][3]= { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0} }; // Center coordinates of clock hands.
uint16_t cy[3][3]= { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0} }; // Center coordinates of clock hands.


void adjustClockfromNTP() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  if (!strcmp(ssid,"")) {
    M5.Lcd.println("Connecting to the previous WiFiAP.");
    M5.Lcd.println("If you don't connect to WiFi. Check ImageWatch.ino");
    WiFi.begin();
  } else {
    M5.Lcd.printf("Connentng to SSID:%s\n", ssid);
    WiFi.begin(ssid, pass);
  }
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }
  M5.Lcd.println("\nWiFi connected");
  
  configTime(9 * 3600, 0, ntpServer);
  struct tm timeinfo;

  M5.Lcd.println("Get Time from NTP");
  while (!getLocalTime(&timeinfo)) {
    M5.Lcd.print(".");
    delay(1000);
  }
  hms[0] = (uint8_t)timeinfo.tm_hour;
  hms[1] = (uint8_t)timeinfo.tm_min;
  hms[2] = (uint8_t)timeinfo.tm_sec;

  M5.Lcd.setTextSize(3);
  M5.Lcd.println("Time adjusted");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  bg.pushLayer(0, 0);

}

void setup(void) {
  M5.begin();
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  M5.Lcd.setRotation(rotation);
  M5.Lcd.setBrightness(brightness);
  targetTime = millis() + 1000; 
  adjustClockfromNTP();
  bg.pushImage(0, 0, image_width1, image_height1, image_data1);

}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    bg.pushImage(0, 0, image_width1, image_height1, image_data1);
  }

  if (M5.BtnB.wasPressed()) {
    bg.pushImage(0, 0, image_width2, image_height2, image_data2);
  }

  if (M5.BtnC.wasPressed()) {
    bg.pushImage(0, 0, image_width3, image_height3, image_data3);
  }

  if (targetTime < millis()) {
    targetTime += 1000;
    hms[2]++;              // Advance second
    if (hms[2] == 60) {
      hms[2] = 0;
      hms[1]++;            // Advance minute
      if(hms[1] > 59) {
        hms[1] = 0;
        hms[0]++;          // Advance hour
        if (hms[0] > 23) {
          hms[0] = 0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    deg[2] = hms[2] * 6;                         // 0-59 -> 0-354
    deg[1] = hms[1] * 6  + deg[2] * 0.01666667;  // 0-59 -> 0-360 - includes seconds
    deg[0] = hms[0] * 30 + deg[1] * 0.0833333;   // 0-11 -> 0-360 - includes minutes and seconds

    float factor = 0.0174523925; // = pai / 180
    for (int i = 0; i < 3 ; i++) { // hms loop
      for (int j = 0; j < 3; j++) { // width loop
        x[i][j] = cos((deg[i] - (90 * j)) * factor);
        y[i][j] = sin((deg[i] - (90 * j)) * factor);
      }
    }
    
    for (int i = 0; i < 3; i++) {
      // Redraw old hand positions
      bg.drawBufferRect(ox[i], oy[i], cx[i][1],cy[i][1], 2);
      for (int j = 0; j < 3; j++) {
        cx[i][j] = clockx + x[i][j];
        cy[i][j] = clocky + y[i][j];
      }
      cx[i][1] = clockx;
      cy[i][1] = clocky;
      ox[i] = x[i][1] * len[i] + clockx;
      oy[i] = y[i][1] * len[i] + clocky;
    }

    // Draw new clock hands.
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        M5.Lcd.drawLine(ox[i], oy[i], cx[i][j], cy[i][j], handscolor[i]);
      }
    }

    M5.Lcd.fillCircle(clockx, clocky, 3, handscolor[2]);
  }
}
