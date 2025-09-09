#include <Arduino.h>
#include <M5Cardputer.h>
#include "utils.h"
#include "config.h"
#include "apps.h"

enum MathOperation {
	NONE = ' ',
	ADD = '+',
	SUB = '-',
	MUL = '*',
	DIV = '/',
	POW = '^',
};

void calc() {
	debounceKeyboard();

	MathOperation currentOperation = NONE;
	double currentValue = 0;
	double previousValue = 0;
	String input = "";
	double memory = 0;

	bool update = true;

	auto next_value = [&currentValue,& previousValue, &input]{
		previousValue = currentValue;
		currentValue = 0;
		input = "";
	};
	auto last_value = [&currentValue,& previousValue, &currentOperation] {
		currentValue = previousValue;
		previousValue = 0;
		currentOperation = NONE;
	};

	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			auto status = M5Cardputer.Keyboard.keysState();
			for (const char word : status.word) {
				switch (status.word[0]) {
					case '+':
						currentOperation = ADD;
						next_value();
						break;
					case '-':
						currentOperation = SUB;
						next_value();
						break;
					case '/':
						currentOperation = DIV;
						next_value();
						break;
					case '*':
						currentOperation = MUL;
						next_value();
						break;
					case 'r':
						currentValue = sqrt(currentValue);
						input = String(currentValue, 4);
						break;
					case 'p':
						currentOperation = POW;
						next_value();
						break;
					case 'a':
						memory += currentValue;
						break;
					case 's':
						memory -= currentValue;
						currentOperation = NONE;
						break;
					case 'g':
						currentValue = memory;
						input = String(memory, 4);
						break;
					case 'c':
						memory = 0;
						break;
					default:
						if ((word >= '0' && word <= '9') || word == '.') {
							input += word;
							currentValue = input.toDouble();
						}
				}
				while (M5Cardputer.Keyboard.isPressed()) M5Cardputer.update();
			}

			if (status.enter && currentOperation != NONE) {
				switch (currentOperation) {
					case NONE:
						break;
					case ADD:
						previousValue += currentValue;
						last_value();
						break;
					case SUB:
						previousValue -= currentValue;
						last_value();
						break;
					case MUL:
						previousValue *= currentValue;
						last_value();
						break;
					case DIV:
						previousValue /= currentValue;
						last_value();
						break;
					case POW:
						previousValue = pow(currentValue, 2);
						last_value();
						break;
				}
				input = String(currentValue, 4);
			}

			if (status.del) {
				input.remove(input.length() - 1);
				currentValue = input.toDouble();
			}
			if (status.opt) return;
			update = true;

			debounceKeyboard();
		}


		if (update) {
			M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
			M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString("> " + input, 10, 10);
			if (currentOperation != NONE) {
				M5Cardputer.Lcd.drawString("OP: " + String(static_cast<char>(currentOperation)), 10, 20);
				M5Cardputer.Lcd.drawString("# " + String(previousValue, 4), 10, 30);
			}
			M5Cardputer.Lcd.drawString("MEM> " + String(memory, 4), 10, 120);

			update = false;
		}

	}
}