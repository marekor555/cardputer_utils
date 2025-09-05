#include <Arduino.h>
#include <M5Cardputer.h>
#include "utils.h"
#include "config.h"
#include "apps.h"



void drawWave(const float freq, const float amp) {
	int positions[240];
	for (int i = 0; i < 240; i++) {
		positions[i] = -sin((i - 120) * freq) * amp;
	}

	constexpr int y_cent = 67;

	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	// 240x135
	// center: (120; 67)
	// y axis
	M5Cardputer.Lcd.drawLine(120, 0, 120, 135, TFT_WHITE);
	for (int i = -12; i <= 12; i++) {
		if (i == 0) continue;
		const int x = 120 + i * STEP_SIN;
		M5Cardputer.Lcd.drawLine(x, 0, x, 135, TFT_DARKGRAY);
	}

	// x axis
	M5Cardputer.Lcd.drawLine(0, 67, 240, 67, TFT_WHITE);
	for (int i = -6; i <= 6; i++) {
		if (i == 0) continue;
		const int y = 67 + i * STEP_SIN;
		M5Cardputer.Lcd.drawLine(0, y, 240, y, TFT_DARKGRAY);
	}

	// draw sin
	int last_y = y_cent + positions[0];
	int last_x = 0;
	M5Cardputer.Lcd.drawPixel(0, y_cent + positions[0], TFT_CYAN);
	for (int i = 1; i < 240; i++) {
		const int x = i;
		const int y = y_cent + positions[i];

		M5Cardputer.Lcd.drawLine(last_x, last_y, x, y, TFT_CYAN);
		last_x = x;
		last_y = y;
	}
	M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
	M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
	M5Cardputer.Lcd.drawString("FREQ="+String(freq*STEP_SIN), 10, 10);
	M5Cardputer.Lcd.drawString("AMP="+String(amp/STEP_SIN), 10, 20);
}

void wave() {
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	const String amp_string = prompt("Enter amplitude:");
	const String freq_string = prompt("Enter frequency:");
	float freq = freq_string.toFloat();
	float amp = amp_string.toFloat();

	drawWave(freq/STEP_SIN, amp*STEP_SIN);
	bool update = false;
	bool debounce = false;
	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
			if (status.opt) {
				break;
			}
			if (!status.word.empty()) {
				switch (status.word[0]) {
					case ';':
						amp += 0.1;
						update = true;
						break;
					case '.':
						amp -= 0.1;
						update = true;
						break;
					case ',':
						freq -= 0.1;
						update = true;
						break;
					case '/':
						freq += 0.1;
						update = true;
						break;
					case 'd':
						debounce = !debounce;
						break;
					default:
						break;
				}
				if (debounce) {
					debounceKeyboard();
				}
				if (update) {
					drawWave(freq/STEP_SIN, amp*STEP_SIN);
					update = false;
				}
			}
		}
	}
}
