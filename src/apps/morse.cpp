#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>
#include <map>


#include "utils.h"
#include "config.h"
#include "apps.h"

std::map<char, String> morseDict = {
	{'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
	{'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
	{'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
	{'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
	{'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
	{'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
	{'Y', "-.--"},  {'Z', "--.."},

	{'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
	{'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
	{'8', "---.."}, {'9', "----."},

	{'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."}, {'\'', ".----."},
	{'!', "-.-.--"}, {'/', "-..-."},  {'(', "-.--."},  {')', "-.--.-"},
	{'&', ".-..."},  {':', "---..."}, {';', "-.-.-."}, {'=', "-...-"},
	{'+', ".-.-."},  {'-', "-....-"}, {'_', "..--.-"}, {'\"', ".-..-."},
	{'$', "...-..-"},{'@', ".--.-."}, {' ', "|"}
};


String charToMorse(const String& morse) {
	String result = "";
	for (char letter : morse) {
		letter = toupper(letter);
		result += morseDict[letter];
		result += " ";
	}
	return result;
}

void playMorse(const String morseCode) {
	M5Cardputer.Speaker.setVolume(40);
	for (const char i : morseCode) {
		switch (i) {
			case '.':
				M5Cardputer.Speaker.tone(1300, 80);
				M5Cardputer.Speaker.tone(1200, 20);
				delay(200);
				break;
			case '-':
				M5Cardputer.Speaker.tone(1000, 160);
				M5Cardputer.Speaker.tone(900, 40);
				delay(400);
				break;
			case ' ':
				delay(200);
				break;
			case '|':
				delay(400);
				break;
		}
	}
}

void morse() {
	String input = "";
	bool update = true;
	int timer = 0;
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
			if (status.del) {
				input = input.substring(0, input.length() - 1);
			}
			if (status.enter) {
				playMorse(charToMorse(input));
			}
			for (const auto i: status.word) {
				input += i;
			}
			update = true;
			timer = 0;
			debounceKeyboard();
		}
		if (update) {
			M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
			M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString(PROMPT + input, 10, 10);
			M5Cardputer.Lcd.setTextColor(TFT_CYAN);
			M5Cardputer.Lcd.drawString("Press enter to play", 10, 30);
			M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
			M5Cardputer.Lcd.drawString(charToMorse(input), 10, 50);
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