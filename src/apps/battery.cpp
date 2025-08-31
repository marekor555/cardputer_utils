#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>

#include "utils.h"
#include "config.h"
#include "apps.h"


void battery() {
	M5Cardputer.Lcd.fillScreen(TFT_BLACK);

	int	batteryLevel, batteryVoltage;
	bool update = true;
	int timer = 0;

	while (true) {
		M5Cardputer.update();
		if (M5Cardputer.Keyboard.isPressed()) {
			const auto status = M5Cardputer.Keyboard.keysState();
			if (status.opt) break;
		}

		if (update) {
			batteryLevel = M5Cardputer.Power.getBatteryLevel(),
			batteryVoltage = M5Cardputer.Power.getBatteryVoltage(),

			M5Cardputer.Lcd.fillScreen(TFT_BLACK);
			M5Cardputer.Lcd.drawString("Battery: " + String(batteryLevel) + "%", 10, 10);
			M5Cardputer.Lcd.drawString("Voltage: " + String(batteryVoltage) + "mV", 10, 20);

			M5Cardputer.Lcd.drawString("Press 'opt' to exit", 10, 60);
			update = false;
		}

		if (timer > 1000) {
			update = true;
			timer = 0;
		}

		delay(1);
		timer++;
	}
}