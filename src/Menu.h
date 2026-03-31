#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Pong.h"
#include "SpaceDrop.h"

extern Adafruit_SSD1306 display;

Pong pong; 
SpaceDrop spacedrop;

//Klasa zarządzająca menu i wyborem gier
class Menu {
    private:
        int button_choice, button1, button2, potentiometer, buzzer, current_game;
        int screen_width, screen_height;
        unsigned long int button_time;
        unsigned long const press_interval = 200;

    public:
        void init(int btn1, int btn2, int pot, int buz, int width, int height) {
            current_game = -1;
            button_choice = 0;
            button1 = btn1;
            button2 = btn2;
            potentiometer = pot;
            buzzer = buz;

            screen_width = width;
            screen_height = height;

            pong.init(potentiometer, buzzer, screen_width, screen_height);
            spacedrop.init(potentiometer, button1, buzzer, screen_width, screen_height);
        }
    
    //Funkcja sprawdzająca wybór gry
    int check_input() {
        if (digitalRead(button2) == LOW && millis() - button_time >= press_interval) {
            button_choice++;
            button_time = millis();
        }

        if (digitalRead(button1) == LOW) {
            if (button_choice % 2 == 0) { return 0; }
            else if (button_choice % 2 != 0) { return 1; }
        }
        
        return -1;
    }

    //Funkcja obsługująca działanie menu
    void update() {
        if (current_game == -1) {
            current_game = check_input();
            draw();
        }
        else if (current_game == 0) {
            if (!pong.is_game_over()) {
                pong.update();
                pong.draw();
            }
            else {
                pong.draw();
                if (digitalRead(button2) == LOW) {
                    pong.init(potentiometer, buzzer, screen_width, screen_height);
                    current_game = -1;
                    delay(200);
                }
            }
        }
        else if (current_game == 1) {
            if (!spacedrop.is_game_over()) {
                spacedrop.update();
                spacedrop.draw();
            }
            else {
                spacedrop.draw();
                if (digitalRead(button2) == LOW) {
                    spacedrop.init(potentiometer, button1, buzzer, screen_width, screen_height);
                    current_game = -1;
                    delay(200);
                }
            }
        }
    }

    //Funkcja renderująca grafikę menu na ekranie OLED
    void draw() {
        display.clearDisplay();
        display.setTextSize(1);

        if (button_choice % 2 == 0) {
            display.fillRect(0, 0, screen_width, 32, WHITE);
            display.setTextColor(BLACK);
            display.setCursor(25, 12);
            display.println("PONG");

            display.setTextColor(WHITE);
            display.setCursor(25, 44);
            display.println("SpaceDrop");
        }
        else {
            display.setTextColor(WHITE);
            display.setCursor(25, 12);
            display.println("PONG");

            display.fillRect(0, 33, screen_width, 32, WHITE);
            display.setTextColor(BLACK);
            display.setCursor(25, 44);
            display.println("SpaceDrop");
        }

        display.display();
    }
};
