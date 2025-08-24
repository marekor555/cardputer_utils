#include <Arduino.h>
#include <M5Cardputer.h>
#include <map>

#include "config.h"
#include "apps.h"
#include "utils.h"

std::vector<float> tones;

// so much notes
std::map<String, float> notes = {
    // Octave 0
    {"c0", 16.35f}, {"cs0", 17.32f}, {"d0", 18.35f}, {"ds0", 19.45f}, {"e0", 20.60f},
    {"f0", 21.83f}, {"fs0", 23.12f}, {"g0", 24.50f}, {"gs0", 25.96f}, {"a0", 27.50f},
    {"as0", 29.14f}, {"b0", 30.87f},

    // Octave 1
    {"c1", 32.70f}, {"cs1", 34.65f}, {"d1", 36.71f}, {"ds1", 38.89f}, {"e1", 41.20f},
    {"f1", 43.65f}, {"fs1", 46.25f}, {"g1", 49.00f}, {"gs1", 51.91f}, {"a1", 55.00f},
    {"as1", 58.27f}, {"b1", 61.74f},

    // Octave 2
    {"c2", 65.41f}, {"cs2", 69.30f}, {"d2", 73.42f}, {"ds2", 77.78f}, {"e2", 82.41f},
    {"f2", 87.31f}, {"fs2", 92.50f}, {"g2", 98.00f}, {"gs2", 103.83f}, {"a2", 110.00f},
    {"as2", 116.54f}, {"b2", 123.47f},

    // Octave 3
    {"c3", 130.81f}, {"cs3", 138.59f}, {"d3", 146.83f}, {"ds3", 155.56f}, {"e3", 164.81f},
    {"f3", 174.61f}, {"fs3", 185.00f}, {"g3", 196.00f}, {"gs3", 207.65f}, {"a3", 220.00f},
    {"as3", 233.08f}, {"b3", 246.94f},

    // Octave 4
    {"c4", 261.63f}, {"cs4", 277.18f}, {"d4", 293.66f}, {"ds4", 311.13f}, {"e4", 329.63f},
    {"f4", 349.23f}, {"fs4", 369.99f}, {"g4", 392.00f}, {"gs4", 415.30f}, {"a4", 440.00f},
    {"as4", 466.16f}, {"b4", 493.88f},

    // Octave 5
    {"c5", 523.25f}, {"cs5", 554.37f}, {"d5", 587.33f}, {"ds5", 622.25f}, {"e5", 659.25f},
    {"f5", 698.46f}, {"fs5", 739.99f}, {"g5", 783.99f}, {"gs5", 830.61f}, {"a5", 880.00f},
    {"as5", 932.33f}, {"b5", 987.77f},

    // Octave 6
    {"c6", 1046.50f}, {"cs6", 1108.73f}, {"d6", 1174.66f}, {"ds6", 1244.51f}, {"e6", 1318.51f},
    {"f6", 1396.91f}, {"fs6", 1479.98f}, {"g6", 1567.98f}, {"gs6", 1661.22f}, {"a6", 1760.00f},
    {"as6", 1864.66f}, {"b6", 1975.53f},

    // Octave 7
    {"c7", 2093.00f}, {"cs7", 2217.46f}, {"d7", 2349.32f}, {"ds7", 2489.02f}, {"e7", 2637.02f},
    {"f7", 2793.83f}, {"fs7", 2959.96f}, {"g7", 3135.96f}, {"gs7", 3322.44f}, {"a7", 3520.00f},
    {"as7", 3729.31f}, {"b7", 3951.07f},

    // Octave 8
    {"c8", 4186.01f}, {"cs8", 4434.92f}, {"d8", 4698.63f}, {"ds8", 4978.03f}, {"e8", 5274.04f},
    {"f8", 5587.65f}, {"fs8", 5919.91f}, {"g8", 6271.93f}, {"gs8", 6644.88f}, {"a8", 7040.00f},
    {"as8", 7458.62f}, {"b8", 7902.13f}
};

void music() {
	debounceKeyboard();
	M5Cardputer.Speaker.setVolume(200);
	String text = "";
	bool update = true;
	int timer = 0;
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
				} else if (text == "exit") {
					return;
				} else {
					float tone = text.toFloat();
					if (tone == 0) {
						tone = notes[text];
					}
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
			update = true;
			debounceKeyboard();
		}
		if (update) {
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
			int yMove = 0;
			for (const auto i: std::vector<float>(tones.rbegin(), tones.rend())) {
				M5Cardputer.Lcd.drawString(String(i), 10, 30 + yMove);
				yMove += 10 * SEC_FONT_SIZE;
			}
			update = false;
		}
	}
}