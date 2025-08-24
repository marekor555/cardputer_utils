#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "utils.h"
#include "config.h"
#include "apps.h"

String getWeatherDescription(const int weather_code) {
	switch (weather_code) {
		case 0: return "Clear sky";
		case 1: return "Mainly clear";
		case 2: return "Partly cloudy";
		case 3: return "Overcast";
		case 45: return "Fog";
		case 48: return "Depositing rime fog";
		case 51: return "Drizzle: light";
		case 53: return "Drizzle: moderate";
		case 55: return "Drizzle: dense";
		case 56: return "Freezing drizzle: light";
		case 57: return "Freezing drizzle: dense";
		case 61: return "Rain: slight";
		case 63: return "Rain: moderate";
		case 65: return "Rain: heavy";
		case 66: return "Freezing rain: light";
		case 67: return "Freezing rain: heavy";
		case 71: return "Snow fall: slight";
		case 73: return "Snow fall: moderate";
		case 75: return "Snow fall: heavy";
		case 77: return "Snow grains";
		case 80: return "Rain showers: slight";
		case 81: return "Rain showers: moderate";
		case 82: return "Rain showers: violent";
		case 85: return "Snow showers: slight";
		case 86: return "Snow showers: heavy";
		case 95: return "Thunderstorm: moderate";
		case 96: return "Thunderstorm with slight hail";
		case 99: return "Thunderstorm with heavy hail";
		default: return "Unknown weather";
	}
}


void openMeteo() {
	debounceKeyboard();
	if (WiFi.status() == WL_CONNECTED) {
		const String cityName = prompt("Enter city name:");
		WiFiClient client;
		HTTPClient http;
		info("Getting request");
		http.begin(client, "http://geocoding-api.open-meteo.com/v1/search?name="+cityName);
		http.setTimeout(TIMEOUT);
		http.GET();
		String text = http.getString();
		JsonDocument response;
		deserializeJson(response, text);

		double lat = response["results"][0]["latitude"];
		double lon = response["results"][0]["longitude"];
		String url = "http://api.open-meteo.com/v1/forecast?latitude="+ String(lat) + "&longitude=" + String(lon) + "&current=weather_code,temperature_2m,wind_speed_10m&forecast_days=1";
		info("Getting forecast");
		http.end();
		http.begin(client, url);
		http.setTimeout(TIMEOUT);
		http.GET();
		text = http.getString();
		deserializeJson(response, text);

		const String time = response["current"]["time"];
		const int weather_code = response["current"]["weather_code"];
		const int temperature = response["current"]["temperature_2m"];
		const int wind_speed = response["current"]["wind_speed_10m"];
		String temperature_unit = response["current_units"]["temperature_2m"];
		temperature_unit.replace("°", "*");
		const String wind_speed_unit = response["current_units"]["wind_speed_10m"];

		debounceKeyboard();
		bool update = true;
		int timer = 0;
		while (true) {
			M5Cardputer.update();
			if (M5Cardputer.Keyboard.isPressed()) {
				const auto status = M5Cardputer.Keyboard.keysState();
				if (status.opt) {
					break;
				}
				timer = 0;
			}

			if (update) {
				M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
				M5Cardputer.Lcd.fillScreen(TFT_BLACK);
				M5Cardputer.Lcd.setTextColor(TFT_CYAN);
				M5Cardputer.Lcd.drawString(time + " " + cityName, 10, 10);
				M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
				M5Cardputer.Lcd.drawString(getWeatherDescription(weather_code), 10, 20);
				M5Cardputer.Lcd.drawString(String(temperature) + " " + temperature_unit, 10, 30);
				M5Cardputer.Lcd.drawString(String(wind_speed) + " " + wind_speed_unit, 10, 40);
				update = false;
			}

			if (timer > SLEEP_TIME) {
				asleep();
				update = true;
				timer = 0;
			}
			timer++;
			delay(1);
		}
	} else {
		const bool connect = yesNoPopup("Connect to WiFi?");
		if (connect) scanAndConnectNetwork();
		debounceKeyboard();
		if (WiFi.status() == WL_CONNECTED) openMeteo();
	}
}