#include <M5Cardputer.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.h>
#include "utils.h"
#include "config.h"


String text = "";
std::vector<float> tones;
int speed = 1;
int timer = 0;



/**
 * play music
 */
void music() {
    String text = "";
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
                        M5Cardputer.Speaker.tone(i, 100 * speed);
                        delay(100 * speed);
                    }
                }
                if (text == "exit") {
                    return;
                }
                const float tone = text.toFloat();
                if (tone != 0) {
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
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
            int yMove = 0;
            for (const auto i: std::vector<float>(tones.rbegin(), tones.rend())) {
                M5Cardputer.Lcd.drawString(String(i), 10, 30 + yMove);
                yMove += 10 * SEC_FONT_SIZE;
            }
            delay(200);
        }
    }
}

/**
 * samsung IR remote
 */
void irSam() {
    Serial.println("Initializing ir");
    String options[] = {
        "; - vol+",
        ". - vol-",
        "/ - prog+",
        ", - prog-",
        "m - mute",
        "o - play",
        "p - pause",
    };
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    int yMove = 0;
    for (const auto i: options) {
        M5Cardputer.Lcd.drawString(i, 10, 30 + yMove);
        yMove += 10 * SEC_FONT_SIZE;
    }
    Serial.println("Starting ir");
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.word.size() > 0) {
                switch (status.word[0]) {
                    case ';':
                        IrSender.sendSamsungMSB(0xE0E0E01F, 32);
                        break;
                    case '.':
                        IrSender.sendSamsungMSB(0xE0E0D02F, 32);
                        break;
                    case ',':
                        IrSender.sendSamsungMSB(0xE0E008F7, 32);
                        break;
                    case '/':
                        IrSender.sendSamsungMSB(0xE0E048B7, 32);
                        break;
                    case 'm':
                        IrSender.sendSamsungMSB(0xE0E0F00F, 32);
                        break;
                    case 'o':
                        IrSender.sendSamsungMSB(0xE0E0E21D, 32);
                        break;
                    case 'p':
                        IrSender.sendSamsungMSB(0xE0E052AD, 32);
                        break;
                    default:
                        break;
                }
            }
            if (status.opt) {
                Serial.println("Exiting ir");
                break;
            }
            delay(200);
        }
    }
}

void setup() {
    IrSender.begin(IR_PIN);
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    const auto cfg = m5::M5Unified::config();
    M5Cardputer.begin(cfg, true);

    M5Cardputer.Lcd.setRotation(1);
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.setTextColor(TFT_GOLD);
    M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
    M5Cardputer.Lcd.drawString(PROMPT, 10, 10);
}

void loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isPressed()) {
        const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        for (const auto i: status.word) {
            text += i;
        }
        if (status.del) {
            text.remove(text.length() - 1);
        }
        if (status.enter) {
            M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
            M5Cardputer.Lcd.setTextColor(TFT_CYAN);
            if (text == "scan") {
                info("Scanning network");
                const int n = WiFi.scanNetworks();
                M5Cardputer.Lcd.fillScreen(TFT_BLACK);
                std::vector<String> networks;
                for (int i = 0; i < n; i++) {
                    networks.push_back(WiFi.SSID(i).c_str());
                }
                scrollTextArr(networks, false);
            } else if (text == "conn") {
                delay(500);
                if (!WiFi.isConnected()) {
                    const String ssid = prompt("SSID: ");
                    const String passwd = prompt("Password: ");
                    WiFi.begin(ssid, passwd);
                    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
                    M5Cardputer.Lcd.drawString("Connecting", 10, 15);
                    unsigned long startAttemptTime = millis();
                    while (WiFi.status() != WL_CONNECTED) {
                        if (millis() - startAttemptTime >= TIMEOUT) {
                            wait("TIMEOUT!", true);
                            break;
                        }
                        delay(200);
                    }
                    wait("Connected successfully", true);
                } else {
                    wait("Already connected", true);
                }
            } else if (text == "req") {
                if (WiFi.isConnected()) {
                    WiFiClient client;
                    HTTPClient http;
                    const String url = prompt("URL: ");
                    info("Getting request");
                    http.begin(client, "http://www." + url);
                    http.setTimeout(TIMEOUT);
                    const int httpCode = http.GET();

                    if (httpCode > 0) {
                        String response = http.getString();
                        if (response.length() > 0)
                            scrollText(response.c_str(), true);
                        else wait(http.errorToString(httpCode), true);
                    } else wait(http.errorToString(httpCode), true);
                    http.end();
                } else {
                    wait("Not connected", true);
                }
            } else if (text == "music") {
                music();
            } else if (text == "irNec") {
                String codeIn = prompt("Code: ");
                unsigned long code = strtoul(codeIn.c_str(), nullptr, 16);
                IrSender.sendNECMSB(code, 32);
                wait("Sent", true);
            } else if (text == "irSam") {
                String codeIn = prompt("Code: ");
                unsigned long code = strtoul(codeIn.c_str(), nullptr, 16);
                IrSender.sendSamsungMSB(code, 32);
                wait("Sent", true);
            } else if (text == "tvOff") {
                IrSender.sendSamsungMSB(0xE0E040BF, 32);
                wait("Sent", true);
            } else if (text == "rem") {
                irSam();
            } else if (text == "linear") {
                delay(250);
                graph();
            } else if (text == "test") {
                scrollText("Test:", true);
            } else if (text == "help") {
                std::vector<String> options = {
                    "scan - scan network",
                    "conn - connect to network",
                    "req - get request",
                    "music - play frequencies",
                    "irNec - send IR",
                    "irSam - send IR (samsung TV)",
                    "tvOff - turn off samsung TV",
                    "rem - remote for samsung TV",
                    "linear - plot linear function",
                };
                scrollTextArr(options, false);
            } else {
                wait("Command not recognized", true);
            }

            M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
            M5Cardputer.Lcd.setTextColor(TFT_GOLD);
            text = "";
        }
        M5Cardputer.Lcd.fillScreen(TFT_BLACK);
        M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
        delay(200);
        timer = 0;
    }
    if (timer > 5000) {
        M5Cardputer.Lcd.sleep();
        while (!M5Cardputer.Keyboard.isChange()) M5Cardputer.update();
        M5Cardputer.Lcd.wakeup();
        timer = 0;
        delay(200);
    }
    timer++;
    delay(1);
}
