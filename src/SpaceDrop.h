#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

class SpaceDrop {
   private:
    int p_x, score, high_score, lifes;
    int potentiometer, buzzer, button;
    const int p_y = 52;

    unsigned long fire_time = 0;
    const unsigned long fire_cooldown = 400;

    struct Bullet {
        float x, y;
        bool active = false;
    };

    struct Enemy {
        float x, y;
        bool active = false;
    };

    struct Explosion {
        float x, y;
        int timer;
        bool active = false;
    };

    Enemy enemies[6];
    Bullet bullets[5];
    Explosion explosions[6];

   public:
    void init(int pot, int btn1, int buz) {
        score = 0;
        fire_time = 0;
        lifes = 3;
        buzzer = buz;
        potentiometer = pot;
        button = btn1;

        for (int i = 0; i < 5; i++) {
            bullets[i].active = false;
        }
        for (int i = 0; i < 6; i++) {
            enemies[i].active = false;
            explosions[i].active = false;
        }
    }

    void spawn_enemy() {
        for (int i = 0; i < 6; i++) {
            if (!enemies[i].active) {
                enemies[i].active = true;
                enemies[i].x = random(2.5, 125.5);
                enemies[i].y = -5;
                break;
            }
        }
    }

    void update() {
        if (is_game_over()) { return; }

        p_x = map(analogRead(potentiometer), 0, 4095, 4, 122);

        if (digitalRead(button) == LOW && (millis() - fire_time >= fire_cooldown)) {
            for (int i = 0; i < 5; i++) {
                if (!bullets[i].active) {
                    bullets[i].active = true;
                    bullets[i].x = (float)p_x;
                    bullets[i].y = (float)p_y - 6;
                    tone(buzzer, 4000, 20);
                    fire_time = millis();
                    break;
                }
            }
        }

        for (int i = 0; i < 5; i++) {
            if (bullets[i].active) {
                bullets[i].y -= 2.0;
                if (bullets[i].y < 0) { bullets[i].active = false; }
            }
        }

        static unsigned long spawn_time = 0;
        if (millis() - spawn_time >= 1150) {
            spawn_enemy();
            spawn_time = millis();
        }

        for (int i = 0; i < 6; i++) {
            if (enemies[i].active) {
                enemies[i].y += 0.5;
                if (enemies[i].y > 64) { enemies[i].active = false; }
            }
        }

        for (int i = 0; i < 6; i++) {
            if (explosions[i].active) {
                explosions[i].timer--;
                if (explosions[i].timer <= 0) { explosions[i].active = false; }
            }
        }

        detect_collision();
        subtract_life();
    }

    void detect_collision() {
        for (int i = 0; i < 5; i++) {
            if (bullets[i].active) {
                for (int j = 0; j < 6; j++) {
                    if (enemies[j].active) {
                        if (abs(bullets[i].x - enemies[j].x) < 4 && abs(bullets[i].y - enemies[j].y) < 4) {
                            if (!explosions[j].active) {
                                explosions[j].active = true;
                                explosions[j].x = enemies[j].x;
                                explosions[j].y = enemies[j].y;
                                explosions[j].timer = 10;
                            }

                            bullets[i].active = false;
                            enemies[j].active = false;
                            tone(buzzer, 200, 30);
                            score++;
                            break;
                        }
                    }
                }
            }
        }
    }

    void subtract_life() {
        for (int i = 0; i < 6; i++) {
            if (enemies[i].active && enemies[i].y > p_y + 1) {
                enemies[i].active = false;
                tone(buzzer, 500, 30);
                lifes--;
                break;
            }
        }
    }

    void explosion(float x, float y) {
        for (int i = 0; i < 7; i++) {
            display.drawPixel(x + i, y, WHITE);
            display.drawPixel(x - i, y, WHITE);
            display.drawPixel(x, y + i, WHITE);
            display.drawPixel(x, y - i, WHITE);
            display.drawPixel(x + i, y + i, WHITE);
            display.drawPixel(x - i, y - i, WHITE);
            display.drawPixel(x - i, y + i, WHITE);
            display.drawPixel(x + i, y - i, WHITE);
        }
    }

    bool is_game_over() {
        return (lifes <= 0);
    }

    void draw() {
        display.clearDisplay();
        display.drawFastHLine(0, p_y + 1, 128, WHITE);
        display.drawTriangle(p_x, p_y - 5, p_x - 5, p_y, p_x + 5, p_y, WHITE);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(60, 57);
        display.print(score);

        for (int i = 0; i < 5; i++) {
            if (bullets[i].active == true) { display.fillRect(bullets[i].x, bullets[i].y, 1, 4, WHITE); }
        }

        for (int i = 0; i < 6; i++) {
            if (enemies[i].active == true) { display.fillCircle(enemies[i].x - 2, enemies[i].y - 2, 2.5, WHITE); }
        }

        for (int i = 0; i < 6; i++) {
            if (explosions[i].active) { explosion(explosions[i].x, explosions[i].y); }
        }

        for (int i = 0; i < lifes; i++) {
            display.fillRect(118 - i * 9, 57, 7, 7, WHITE);
        }

        for (int i = 0; i < 3 - lifes; i++) {
            display.drawRect(100 + i * 9, 57, 7, 7, WHITE);
        }

        if(is_game_over()) {
            display.setCursor(32, 16);
            display.println("GAME OVER");
            display.setCursor(25, 32);
            display.print("Your Score: ");
            display.println(score);
        }

        display.display();
    }
};
