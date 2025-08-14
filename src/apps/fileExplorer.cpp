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
		} if (filename == "|..|") {
			viewPath = viewPath.substring(0, viewPath.lastIndexOf("/"));
		} else {
			if (filename.startsWith("|") && filename.endsWith("|")) {
				filename = filename.substring(1, filename.length()-1);
			}
			File selectedFile = SD.open(viewPath + "/" + filename);
			if (!selectedFile.isDirectory()) {
				String text = selectedFile.readString();
				selectedFile.close();
				scrollText(text, true);
			} else {
				viewPath += "/" + filename;
			}
		}
		debounceKeyboard();
	}
}