#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>

#include "utils.h"
#include "config.h"
#include "apps.h"


void clicker() {
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);

	int timer = 0, count = 0;
	bool update = true;
	M5Cardputer.Speaker.setVolume(35);
	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			auto status = M5Cardputer.Keyboard.keysState();
			for (auto i : status.word) {
				switch (i) {
					case ';':
						count++;
						M5Cardputer.Speaker.tone(1000, 100);
						M5Cardputer.Speaker.tone(1200, 100);
						update = true;
						break;
					case '.':
						if (count > 0) count--;
						M5Cardputer.Speaker.tone(1500, 100);
						M5Cardputer.Speaker.tone(1700, 100);
						update = true;
						break;
				}
			}
			if (status.opt) {
				return;
			}
			timer = 0;
			debounceKeyboard();
		}

		if (update) {
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.setTextSize(5);
			M5Cardputer.Lcd.setTextColor(TFT_WHITE);
			M5Cardputer.Lcd.setCursor(20, 52);
			M5Cardputer.Lcd.print(count);
			update = false;
		}

		if (timer > SLEEP_TIME) {
			asleep();
			timer = 0;
			update = true;
		}

		timer++;
		delay(1);
	}
}