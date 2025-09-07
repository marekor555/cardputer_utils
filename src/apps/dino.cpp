#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>

#include "utils.h"
#include "config.h"
#include "apps.h"

#define PLAYER_HEIGHT 15
#define PLAYER_WIDTH 10
#define PLAYER_SPEED 0.75
#define PLAYER_JUMP 30

#define SPIKE_HEIGHT 8
#define SPIKE_WIDTH 13

#define PLATFORM_HEIGHT 40

struct Position {
	float x;
	float y;
};

struct Spike {
	Position position;
	Position lastPosition;
};

float playerY = 0;
float lastPlayerY = 0;
bool jump = false;
Spike spikes[4];

bool spikeGen(Spike spike) {
	for (const auto sp : spikes) {
		if (sp.position.x == spike.position.x) continue;
		if (abs(sp.position.x - spike.position.x) < 70) return false;
	}
	return true;
}


void dino() {
	debounceKeyboard();

	for (int i = 0; i < 4; i++) {
		spikes[i].position.x = 250+i*100+random(100, 240);
		spikes[i].position.y = 135-PLATFORM_HEIGHT-SPIKE_HEIGHT;
	}
	spikes[0].position.x = 210;


	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	M5Cardputer.Lcd.fillRect(0, 135-PLATFORM_HEIGHT, 240, PLATFORM_HEIGHT, M5Cardputer.Lcd.color565(10, 20, 10));
	M5Cardputer.Lcd.fillRect(20, 135-playerY-PLATFORM_HEIGHT-PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, TFT_DARKGREEN);

	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
			switch (status.word[0]) {
				case ' ':
					if (playerY == 0) jump = true;
					break;
			}
		}

		if (playerY >= PLAYER_JUMP) jump = false;

		lastPlayerY = playerY;
		if (jump && playerY <= PLAYER_JUMP) playerY += PLAYER_SPEED;
		else if (playerY >= 0) playerY -= PLAYER_SPEED*1.5;
		if (playerY < 0) playerY = 0;

		for (Spike &spike : spikes) {
			if (spike.position.x <= 0) {
				spike.lastPosition = spike.position;
				spike.position.x = 250+random(20, 60);
				do {
					spike.position.x+=10;
				} while (!spikeGen(spike));
			}
			spike.lastPosition = spike.position;
			spike.position.x -= 1;


			if (spike.position.x >= 25+PLAYER_WIDTH/3 && spike.position.x <= 35+PLAYER_WIDTH/3 && playerY <= SPIKE_HEIGHT/2) {
				wait("GAME OVER", true);
				return;
			}
		}

		if (playerY != lastPlayerY) {
			M5Cardputer.Lcd.fillRect(20, 135-lastPlayerY-PLATFORM_HEIGHT-PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, TFT_BLACK);
			M5Cardputer.Lcd.fillRect(20, 135-playerY-PLATFORM_HEIGHT-PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, TFT_DARKGREEN);
		}

		M5Cardputer.Lcd.fillRect(0, 135-PLATFORM_HEIGHT-SPIKE_HEIGHT, SPIKE_WIDTH, SPIKE_HEIGHT, TFT_BLACK);
		for (const Spike spike : spikes) {
			M5Cardputer.Lcd.fillTriangle(spike.lastPosition.x-SPIKE_WIDTH/2, spike.lastPosition.y+SPIKE_HEIGHT-1, spike.lastPosition.x, spike.lastPosition.y, spike.lastPosition.x+SPIKE_WIDTH/2, spike.lastPosition.y+SPIKE_HEIGHT-1, TFT_BLACK);
			M5Cardputer.Lcd.fillTriangle(spike.position.x-SPIKE_WIDTH/2, spike.position.y+SPIKE_HEIGHT-1, spike.position.x, spike.position.y, spike.position.x+SPIKE_WIDTH/2, spike.position.y+SPIKE_HEIGHT-1, TFT_RED);
		}

		delay(10);
	}
}