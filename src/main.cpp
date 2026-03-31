#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Menu.h"

#define PIN_POT 34
#define PIN_BTN1 23
#define PIN_BTN2 25
#define PIN_BUZZ 27

Adafruit_SSD1306 display(128, 64, &Wire, -1);

Menu menu;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  menu.init(PIN_BTN1, PIN_BTN2, PIN_POT, PIN_BUZZ);

  randomSeed(esp_random());
}

void loop() {
  menu.update();
}

