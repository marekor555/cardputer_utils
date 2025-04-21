#include <Arduino.h>
#include <M5Cardputer.h>
#include "config.h"
#include "apps.h"
std::vector<float> tones;

void music() {
	String text = "";
	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
			for (const auto i: status.word) {
				text += i;
			}
			if (status.enter) {
				if (text == "play") {
					for (const auto i: tones) {
						M5Cardputer.Speaker.tone(i, 100 * MUSIC_SPEED);
						delay(100 * MUSIC_SPEED);
					}
				}
				if (text == "exit") {
					return;
				}
				const float tone = text.toFloat();
				if (tone != 0) {
					tones.push_back(tone);
					text = "";
					M5Cardputer.Speaker.tone(tone, 100);
				}
			}
			if (status.del) {
				text.remove(text.length() - 1);
			}
			if (status.tab) {
				if (!tones.empty()) {
					tones.pop_back();
				}
			}
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
			int yMove = 0;
			for (const auto i: std::vector<float>(tones.rbegin(), tones.rend())) {
				M5Cardputer.Lcd.drawString(String(i), 10, 30 + yMove);
				yMove += 10 * SEC_FONT_SIZE;
			}
			delay(200);
		}
	}
}