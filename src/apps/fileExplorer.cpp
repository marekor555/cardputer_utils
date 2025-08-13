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
	delay(300);
	SPI.begin(40, 39, 14, 12);
	while (!SD.begin(12, SPI)) {}
	while (true) {
		File root = SD.open(viewPath);
		File file = root.openNextFile();

		files.clear();
		files.push_back("|exit|");
		files.push_back("|..|");
		while (file) {
			if (file.isDirectory()) {
				files.push_back("|"+String(file.name()));
			} else {
				files.push_back(file.name());
			}
			file = root.openNextFile();
		}

		filename = scrollTextArrHighlight(files, true);
		if (filename == "|exit|") {
			SD.end();
			return;
		} if (filename == "|..|") {
			viewPath = viewPath.substring(0, viewPath.lastIndexOf("/"));
		} else {
			if (filename.startsWith("|")) {
				filename = filename.substring(1);
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
		delay(300);
	}
}