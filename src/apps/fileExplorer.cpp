#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <SPI.h>
#include <SD.h>

#include "utils.h"
#include "config.h"
#include "apps.h"

std::vector<String> files;

String viewPath = "/";
String filename = "";

void playMusic() {
	File musicFile = SD.open(viewPath + "/" + filename);
	uint32_t sample_rate = 16000;
	musicFile.seek(24);
	musicFile.readBytes(reinterpret_cast<char *>(&sample_rate), 4);
	musicFile.seek(0);

	auto cfg = M5Cardputer.Speaker.config();
	cfg.sample_rate = sample_rate;
	M5Cardputer.Speaker.config(cfg);

	M5Cardputer.Speaker.begin();
	int volume = 40;
	M5Cardputer.Speaker.setVolume(volume);


	musicFile.seek(44);
	uint8_t buffer[1024];
	bool update = true;

	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	M5Cardputer.Lcd.drawString("Playing: " + filename, 10, 10);
	M5Cardputer.Lcd.drawString("Volume: ", 10, 20);
	M5Cardputer.Lcd.drawString("Press ';' to increase volume", 10, 60);
	M5Cardputer.Lcd.drawString("Press '.' to decrease volume", 10, 70);
	while (musicFile.available()) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
				for (const auto i: status.word) {
					switch (i) {
						case ';':
							volume++;
							if (volume > 200) {
								volume = 200;
							}
							M5Cardputer.Speaker.setVolume(volume);
							update = true;
							break;
						case '.':
							volume--;
							if (volume < 0) {
								volume = 0;
							}
							M5Cardputer.Speaker.setVolume(volume);
							update = true;
							break;
					}
				}
			}
			if (update) {
				M5Cardputer.Lcd.fillRect(10, 30, 100, 10, TFT_BLACK);
				M5Cardputer.Lcd.drawString(String(volume), 10, 30);
				update = false;
			}


			int bytesRead = musicFile.read(buffer, sizeof(buffer));
			if (bytesRead <= 0) break;
			bytesRead /= 2;
			const auto ptr = reinterpret_cast<int16_t *>(buffer);
			M5Cardputer.Speaker.playRaw(ptr, bytesRead, sample_rate);
			while (M5Cardputer.Speaker.isPlaying()) {}
		}
	musicFile.close();
	M5Cardputer.Speaker.end();
	wait("Finished playing", true);
}

void textEdit(const String path) {
	File file = SD.open(path);
	int xPos = 0, yPos = 0, timer = 0;
	bool update = true, viewMode = false;

	std::vector<String> lines;
	while (file.available()) {
		lines.push_back(file.readStringUntil('\n'));
	}
	file.close();
	if (lines.empty()) lines.push_back("");

	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
			if (status.enter) {
				String line = lines[yPos].substring(xPos);
				lines[yPos] = lines[yPos].substring(0, xPos);
				lines.insert(lines.begin() + yPos + 1, line);
				yPos++;
				xPos = 0;
				update = true;
			}
			if (status.del) {
				if (xPos > 0) {
					lines[yPos].remove(xPos - 1, 1);
					xPos--;
				} else if (yPos > 0) {
					xPos = lines[yPos - 1].length();
					lines[yPos - 1] += lines[yPos];
					lines.erase(lines.begin() + yPos);
					yPos--;
				}
				update = true;
			}
			if (status.fn) {
				viewMode = !viewMode;
				update = true;
			}
			for (const auto i : status.word) {
				if (viewMode) {
					switch (i) {
						case '.':
							if (yPos < lines.size() - 1) {
								yPos++;
							}
							break;
						case ';':
							if (yPos > 0 ) {
								yPos--;
							}
							break;
						case ',':
							if (xPos > 0) {
								xPos--;
							}
							break;
						case '/':
							if (xPos < lines[yPos].length()) {
								xPos++;
							}
							break;
					}
				} else {
					lines[yPos] = lines[yPos].substring(0, xPos) + i + lines[yPos].substring(xPos);
					xPos++;
				}
				update = true;
			}
			debounceKeyboard();
			timer = 0;
		}
		if (update) {
			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString(path + " " + viewMode, 10, 10);
			xPos = min(xPos, static_cast<int>(lines[yPos].length()));
			for (int i = 0; i < lines.size(); i++) {
				String line = lines[i];
				if (yPos == i) {
					if (xPos < line.length())
						line.setCharAt(xPos, '*');
					else line += "*";
				}
				M5Cardputer.Lcd.drawString(line, 10, 20 + i * 10);
			}
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

	const bool save = yesNoPopup("Save changes?");
	if (save) {
		file = SD.open(path, FILE_WRITE);
		for (const auto line : lines) {
			file.println(line);
		}
		file.close();
		wait("Saved", true);
	}
}

void fileExplorer() {
	debounceKeyboard();
	SPI.begin(40, 39, 14, 12);
	while (!SD.begin(12, SPI)) {}
	while (true) {
		File root = SD.open(viewPath);
		File file = root.openNextFile();

		files.clear();
		if (viewPath == "/") files.push_back("||-exit-||");
		if (viewPath != "/") files.push_back("|..|");
		files.push_back("||-new file-||");
		while (file) {
			if (file.isDirectory()) {
				files.push_back("|"+String(file.name())+"|");
			}
			file = root.openNextFile();
		}

		root = SD.open(viewPath);
		file = root.openNextFile();
		while (file) {
			if (!file.isDirectory()) {
				files.push_back(String(file.name()));
			}
			file = root.openNextFile();
		}

		filename = scrollTextArrHighlight(files, true, SEC_FONT_COLOR, TFT_BLUE);
		if (filename == "||-exit-||") {
			SD.end();
			return;
		} if (filename == "||-new file-||") {
			debounceKeyboard();
			String newFile = prompt("New file name: ");
			File newFileObj = SD.open(viewPath + "/" + newFile, FILE_WRITE);
			newFileObj.close();
			wait("Created", true);
		}else if (filename == "|..|") {
			viewPath = viewPath.substring(0, viewPath.lastIndexOf("/"));
		} else {
			if (filename.startsWith("|") && filename.endsWith("|")) {
				filename = filename.substring(1, filename.length()-1);
			}
			File selectedFile = SD.open(viewPath + "/" + filename);
			if (!selectedFile.isDirectory()) {
				std::vector<String> options = {
					"|back|",
					"View",
					"Delete",
					"Edit",
				};
				if (filename.endsWith(".wav")) {
					options.push_back("Play music");
				}

				debounceKeyboard();
				const String selected = scrollTextArrHighlight(options, false, SEC_FONT_COLOR, TFT_BLUE);
				if (selected == "View") {
					const String text = selectedFile.readString();
					selectedFile.close();
					scrollText(text, true);
				} else if (selected == "Play music") {
					playMusic();
				} else if (selected == "Delete") {
					if (yesNoPopup("Are you sure?")) {
						selectedFile.close();
						if (SD.exists(viewPath + "/" + filename)) {
							if (SD.remove(viewPath + "/" + filename)) {
								wait("Deleted", true);
							} else {
								wait("Failed to delete", true);
							}
						}
					}
				} else if (selected == "Edit") {
					textEdit(viewPath + "/" + filename);
				}
			} else {
				viewPath += "/" + filename;
			}
		}
		debounceKeyboard();
	}
}