#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>

#include "utils.h"
#include "config.h"
#include "apps.h"

#define SPEED 16 // update snowflakes speed

int screen[240][135] = {0};


void snowAnimation() {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);

    while (true) {
        const int frameTime = millis();
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const auto status = M5Cardputer.Keyboard.keysState();
            if  (status.opt) break;
        }

        for (int i = 239; i >= 0; i--) {
            for (int j = 133; j >= 0 ; j--) {
                if (screen[i][j] == 1 && (screen[i][j+1] == 0 || screen[constrain(i-1, 0, 239)][j+1] == 0 || screen[constrain(i-1, 0, 239)][j+1] == 0)) {
                    const int offset = random(-2, 3);
                    if (screen[i+offset][j+1] != 1) {
                        screen[i+offset][j+1] = 1;
                        screen[i][j] = 0;
                        M5Cardputer.Lcd.drawPixel(i, j, TFT_BLACK);
                        M5Cardputer.Lcd.drawPixel(i+offset, j+1, TFT_WHITE);
                    }
                }
            }
        }

        bool lastLineFilled = true;
        for (int i = 0; i < 240; i++) {
            if (screen[i][134] == 0) lastLineFilled = false;
        }
        if (lastLineFilled) {
            for (int i = 0; i < 240; i++) {
                screen[i][134] = 0;
            }
        }

        for (int i = 0; i < 3; i++) {
            screen[random(0, 240)][0] = 1;
        }
        if (SPEED > (millis() - frameTime)) delay(SPEED - (millis() - frameTime));
    }
}
