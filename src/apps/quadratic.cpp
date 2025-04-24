#include <Arduino.h>
#include <M5Cardputer.h>
#include "utils.h"
#include "config.h"
#include "apps.h"


float quadratic_equation(const float a, const float b, const float c, const float x) {
	return a*x*x + b*x + c;
}

void drawQuadratic(const float a, const float b, const float c) {
	int positions[240];
	for (int i = 0; i < 240; i++) {
		positions[i] = quadratic_equation(-a,-b,c,(i-120)/STEP) * STEP;
	}

	constexpr int y_cent = 67;

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
	M5Cardputer.Lcd.drawString("A="+String(a), 10, 10);
	M5Cardputer.Lcd.drawString("B="+String(b), 10, 20);
	M5Cardputer.Lcd.drawString("C="+String(c), 10, 30);
}

void quadratic() {
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	float a = 1;
	float b = 0;
	float c = 0;

	drawQuadratic(a,b,c);

	// M5Cardputer.Lcd.drawLine(POINT1_X+, point1_y, POINT2_X, point2_y, TFT_RED);
	bool update = false;
	while (!M5Cardputer.BtnA.wasClicked()) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
			if (status.opt) {
				break;
			}
			if (status.word.size() > 0) {
				switch (status.word[0]) {
					case ';':
						c -= 0.25;
						update = true;
						break;
					case '.':
						c += 0.25;
						update = true;
						break;
					case ',':
						a -= 0.01;
						update = true;
						break;
					case '/':
						a += 0.01;
						update = true;
						break;
					case 'l':
						b += 0.1;
						update = true;
						break;
					case '\'':
						b -= 0.1;
						update = true;
						break;
					default:
						break;
				}
				if (update) {
					drawQuadratic(a,b,c);
					update = false;
				}
			}
		}
	}
}
