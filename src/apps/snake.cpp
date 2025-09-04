#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>

#include "utils.h"
#include "config.h"
#include "apps.h"


enum GridState {
	EMPTY,
	SNAKE,
	FOOD
};

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

#define GRID_SIZE 13
#define PIXEL_SIZE 10

int grid[GRID_SIZE][GRID_SIZE];

void snakeGame() {
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	Direction direction = RIGHT;
	Direction frameDirection = RIGHT;
	int snakeLength = 1;
	std::vector<std::array<int, 2>> positions;
	std::array<int, 2> snakeHead = {GRID_SIZE/4, GRID_SIZE/2};

	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			grid[i][j] = EMPTY;
		}
	}
	grid[10][10] = FOOD;

	unsigned long timer = millis();
	bool update = true, paused = false;
	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
			for (const auto i: status.word) {
				switch (i) {
					case ';':
						if (frameDirection != DOWN) direction = UP;
						break;
					case '.':
						if (frameDirection != UP) direction = DOWN;
						break;
					case ',':
					case 'a':
						if (frameDirection != RIGHT) direction = LEFT;
						break;
					case '/':
					case 's':
						if (frameDirection != LEFT) direction = RIGHT;
						break;
					case 'p':
					case '\'':
						paused = !paused;
						debounceKeyboard();
						break;
				}
			}
		}

		if (paused) {
			timer = millis();
			continue;
		}

		if (millis() - timer < 200) {
			continue;
		}
		frameDirection = direction;
		timer = millis();

		// draw
		for (int i = 0; i < GRID_SIZE; i++) {
			for (int j = 0; j < GRID_SIZE; j++) {
				int color = TFT_DARKGRAY;
				switch (grid[i][j]) {
					case EMPTY:
						color = TFT_DARKGRAY;
						break;
					case FOOD:
						color = TFT_RED;
						break;
					case SNAKE:
						if (i == snakeHead[0] && j == snakeHead[1]) color = TFT_GREENYELLOW;
						else color = TFT_GREEN;
						break;
				}
				M5Cardputer.Lcd.fillRect(i*PIXEL_SIZE, j*PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE, color);
			}
			if (update) {
				M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
				M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
				M5Cardputer.Lcd.fillRect(GRID_SIZE*PIXEL_SIZE, 0, 240-GRID_SIZE*PIXEL_SIZE, 135, TFT_BLACK);
				M5Cardputer.Lcd.drawString("SCORE: " + String(snakeLength), GRID_SIZE*PIXEL_SIZE+10, 10);
				update = false;
			}
		}

		// update
		switch (direction) {
			case UP:
				snakeHead[1]--;
				break;
			case DOWN:
				snakeHead[1]++;
				break;
			case LEFT:
				snakeHead[0]--;
				break;
			case RIGHT:
				snakeHead[0]++;
				break;
		}

		if (snakeHead[0] < 0) snakeHead[0] = GRID_SIZE-1;
		if (snakeHead[1] < 0) snakeHead[1] = GRID_SIZE-1;
		if (snakeHead[0] >= GRID_SIZE) snakeHead[0] = 0;
		if (snakeHead[1] >= GRID_SIZE) snakeHead[1] = 0;

		positions.push_back(snakeHead);

		if (positions.size() > snakeLength) {
			grid[positions[0][0]][positions[0][1]] = EMPTY;
			positions.erase(positions.begin());
		}

		switch (grid[snakeHead[0]][snakeHead[1]]) {
			case EMPTY:
				grid[snakeHead[0]][snakeHead[1]] = SNAKE;
				break;
			case FOOD:
				snakeLength++;
				update=true;
				grid[snakeHead[0]][snakeHead[1]] = SNAKE;
			{
				long newX = random(0, GRID_SIZE-1), newY = random(0, GRID_SIZE-1);
				while (grid[newX][newY] != EMPTY) newX = random(0, GRID_SIZE-1), newY = random(0, GRID_SIZE-1);
				grid[newX][newY] = FOOD;
			}
				break;
			case SNAKE:
				wait("GAME OVER", true);
				return;
		}
	}
}