#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Menu.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PIN_POT 2
#define PIN_BTN1 3
#define PIN_BTN2 4
#define PIN_BUZZ 5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Menu menu;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  menu.init(PIN_BTN1, PIN_BTN2, PIN_POT, PIN_BUZZ, SCREEN_WIDTH, SCREEN_HEIGHT);

  randomSeed(esp_random());
}

void loop() {
  menu.update();
}
