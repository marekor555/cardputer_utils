#include <Arduino.h>
#include <M5Cardputer.h>
#include "utils.h"
#include "config.h"
#include "apps.h"


void drawLinear(const float a, const float b) {
	const float point1_y = a*POINT1_X + b;
	const float point2_y = a*POINT2_X + b;

	constexpr int x1_screen = 120 + POINT1_X / 2;
	const float y1_screen = 67 - point1_y / 2;
	constexpr int x2_screen = 120 + POINT2_X / 2;
	const float y2_screen = 67 - point2_y / 2;

	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	// 240x135
	// center: (120; 67)
	// y axis
	M5Cardputer.Lcd.drawLine(120, 0, 120, 135, TFT_WHITE);
	for (int i = -12; i <= 12; i++) {
		if (i == 0) continue;
		const int x = 120 + i * STEP;
		M5Cardputer.Lcd.drawLine(x, 0, x, 135, TFT_DARKGRAY);
	}

	// x axis
	M5Cardputer.Lcd.drawLine(0, 67, 240, 67, TFT_WHITE);
	for (int i = -6; i <= 6; i++) {
		if (i == 0) continue;
		const int y = 67 + i * STEP;
		M5Cardputer.Lcd.drawLine(0, y, 240, y, TFT_DARKGRAY);
	}

	// linear function
	M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
	M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
	M5Cardputer.Lcd.drawLine(x1_screen, y1_screen, x2_screen, y2_screen, TFT_CYAN);
	M5Cardputer.Lcd.drawString("A="+String(a), 10, 10);
	M5Cardputer.Lcd.drawString("B="+String(b/20), 10, 20);
}

void linear() {
  	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	const String a_string = prompt("Enter A:");
	const String b_string = prompt("Enter B:");
	float a = a_string.toFloat();
	float b = b_string.toFloat()*20;

	bool update = true;
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
						b+=0.1*20;
						update = true;
						break;
					case '.':
						b-=0.1*20;
						update = true;
						break;
					case ',':
						a+=0.1;
						update = true;
						break;
					case '/':
						a-=0.1;
						update = true;
				}
			}
		}
		if (update) {
			drawLinear(a, b);
			update = false;
		}
	}

	// M5Cardputer.Lcd.drawLine(POINT1_X+, point1_y, POINT2_X, point2_y, TFT_RED);
	wait("", false);
}