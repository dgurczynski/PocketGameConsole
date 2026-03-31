#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

extern Adafruit_SSD1306 display;

//Klasa zarządzająca grą PONG
class Pong {
  private:
    float b_x, b_vx, b_y, b_vy;
    float p1_y, p2_y;
    int p1_score, p2_score;
    int buzzer, potentiometer;
    int screen_width, screen_height;
    const float speed = 2.915;

    //Funkcja przywracająca piłkę na pozycję startową
    void reset_ball(int dir) {
        b_x = 63;
        b_y = (p1_score + p2_score) % 2 == 0 ? 16 : 47;

        b_vx = -dir * 2.5;
        b_vy = 1.5;
    }

  public:
    void init(int pot, int buz, int width, int height) {
        p1_score = 0;
        p2_score = 0;
        buzzer = buz;
        potentiometer = pot;
        
        screen_width = width;
        screen_height = height;
        reset_ball(1);
    }

    //Funkcja logiki gry
    void update() {
        p1_y = map(analogRead(potentiometer), 0, 4095, 1, 47);

        float bot_speed = 1.8;
        if (p2_y + 8 < b_y) {
            p2_y += bot_speed;
        } else if (p2_y + 8 > b_y) {
            p2_y -= bot_speed;
        }

        p2_y = constrain(p2_y, 1, 47);

        float ball_how_far = 0;
        float bounce_angle = 0;
        float direction = 0;

        if (b_x >= 121.0 && b_x <= 124.0 && b_y >= p2_y && b_y <= p2_y + 16) {
            ball_how_far = (b_y - (p2_y + 8)) / 8;
            direction = -1.0;
            b_x = 120.0;
        } else if (b_x >= 4.0 && b_x <= 7.0 && b_y >= p1_y && b_y <= p1_y + 16) {
            ball_how_far = (b_y - (p1_y + 8)) / 8;
            direction = 1.0;
            b_x = 8.0;
        }

        if (direction != 0) {
            bounce_angle = ball_how_far * (M_PI / 3);
            b_vx = direction * speed * cos(bounce_angle);
            b_vy = speed * sin(bounce_angle);
            tone(buzzer, 3000, 10);
        }

        if (b_y > 62 || b_y < 1) {
            b_vy *= -1.0;
            tone(buzzer, 2000, 10);
        }

        b_x += b_vx;
        b_y += b_vy;

        if (b_x > 127) {
            p1_score++;
            tone(buzzer, 250, 150);
            if (!is_game_over()) {
                reset_ball(-1);
                delay(1000);
            };
        } else if (b_x < 0) {
            p2_score++;
            tone(buzzer, 250, 150);
            if (!is_game_over()) {
                reset_ball(1);
                delay(1000);
            }
        }
    }

    bool is_game_over() { return (p1_score >= 11 || p2_score >= 11); }

    //Funkcja renderująca grafikę gry na ekranie OLED
    void draw() {
        display.clearDisplay();
        display.drawFastHLine(0, 0, screen_width, WHITE);
        display.drawFastHLine(0, screen_height - 1, screen_width, WHITE);
        display.fillRect(4, (int)p1_y, 3, 16, WHITE);
        display.fillRect(121, (int)p2_y, 3, 16, WHITE);
        display.fillCircle((int)b_x, (int)b_y, 2, WHITE);

        for (int i = 0; i < screen_height; i += 3) {
            display.drawPixel((screen_width / 2) - 1, i, WHITE);
        }

        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(45, 5);
        display.print(p1_score);
        display.setCursor(78, 5);
        display.print(p2_score);

        if (is_game_over()) {
            display.setCursor(25, 31);
            display.print(p1_score == 11 ? "PLAYER 1 WON!" : "PLAYER 2 WON!");
        }

        display.display();
    }
};
