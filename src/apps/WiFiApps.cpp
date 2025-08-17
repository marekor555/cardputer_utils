#include <Arduino.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "utils.h"
#include "config.h"
#include "apps.h"


void scanAndConnectNetwork() {
	info("Scanning network");
	const int n = WiFi.scanNetworks();
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);
	std::vector<String> networks;
	networks.push_back("|back|");
	for (int i = 0; i < n; i++) {
		networks.push_back(WiFi.SSID(i).c_str());
	}
	const String selectedNetwork = scrollTextArrHighlight(networks, false, SEC_FONT_COLOR, PRIM_FONT_COLOR);
	if (selectedNetwork != "|back|") {
		const String ssid = selectedNetwork;
		debounceKeyboard();
		const String passwd = prompt("Password: ");
		WiFi.begin(ssid, passwd);
		M5Cardputer.Lcd.fillScreen(TFT_BLACK);
		M5Cardputer.Lcd.drawString("Connecting...", 10, 15);
		while (WiFi.status() != WL_CONNECTED) {
			M5Cardputer.update();
			if (M5Cardputer.Keyboard.isPressed()) {
				const auto status = M5Cardputer.Keyboard.keysState();
				if (status.opt) break;
			}
			if (WiFi.status() == WL_CONNECT_FAILED) {
				wait("Connection failed", true);
				break;
			}
			if (WiFi.status() == WL_NO_SSID_AVAIL) {
				wait("No network found", true);
			}
			if (WiFi.status() == WL_DISCONNECTED) {
				wait("Disconnected", true);
			}
		}
	}
	wait("Connected successfully", true);
}

void webRequest() {
	if (WiFi.isConnected()) {
		WiFiClient client;
		HTTPClient http;
		const String url = prompt("URL: http://www.");
		info("Getting request");
		http.begin(client, "http://www." + url);
		http.setTimeout(TIMEOUT);
		const int httpCode = http.GET();

		if (httpCode > 0) {
			String response = http.getString();
			if (response.length() > 0) {
				String parsedResponse = "";
				for (int i = 0; i < response.length(); i++) {
					if (i % 38 == 0) parsedResponse += "\n";
					parsedResponse += response[i];
				}
				scrollText(parsedResponse.c_str(), true);
			}
			else wait(http.errorToString(httpCode), true);
		} else wait(http.errorToString(httpCode), true);
		http.end();
	} else {
		wait("Not connected", true);
	}
}