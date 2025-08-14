#include <M5Cardputer.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.h>
#include "utils.h"
#include "config.h"
#include "apps.h"


std::vector<String> commandList = {
    "help",
    "scan",
    "req",
    "music",
    "irNec",
    "irSam",
    "tvOff",
    "rem",
    "linear",
    "wave",
    "quadratic",
    "battery",
    "files",
};

String text = "", lastCmd = "";
int timer = 0;

std::vector<String> getAvaiableCommands(String command, std::vector<String> cmdList) {
    if (command.length() == 0 || command.isEmpty()) return cmdList;
    std::vector<String> avaiableCommands;
    for (const auto i : cmdList) {
        if (i.startsWith(command)) avaiableCommands.push_back(i);
    }

    return avaiableCommands;
}

void drawCommands() {
    M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
    M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
    std::vector<String> available = getAvaiableCommands(text, commandList);
    for (int i = 0; i < available.size(); i++) {
        M5Cardputer.Lcd.drawString(available[i], 10, 30+i*10);
    }
    M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
    M5Cardputer.Lcd.setTextColor(PRIM_FONT_COLOR);
}

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
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
    IrSender.begin(IR_PIN);
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    const auto cfg = m5::M5Unified::config();
    M5Cardputer.begin(cfg, true);

    M5Cardputer.Lcd.setRotation(1);
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.setTextColor(PRIM_FONT_COLOR);
    M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
    M5Cardputer.Lcd.drawString(PROMPT, 10, 10);
    drawCommands();
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
        if (status.fn) {
            text = lastCmd;
        }
        if (status.enter) {
            lastCmd = text;
            M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
            M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
            if (text == "scan") {
                info("Scanning network");
                const int n = WiFi.scanNetworks();
                M5Cardputer.Lcd.fillScreen(TFT_BLACK);
                std::vector<String> networks;
                networks.push_back("|back|");
                for (int i = 0; i < n; i++) {
                    networks.push_back(WiFi.SSID(i).c_str());
                }
                String selectedNetwork = scrollTextArrHighlight(networks, false, SEC_FONT_COLOR, PRIM_FONT_COLOR);
                if (selectedNetwork != "|back|") {
                    const String ssid = selectedNetwork;
                    debounceKeyboard();
                    const String passwd = prompt("Password: ");
                    WiFi.begin(ssid, passwd);
                    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
                    M5Cardputer.Lcd.drawString("Connecting", 10, 15);
                    unsigned long startAttemptTime = millis();
                    while (WiFi.status() != WL_CONNECTED) {
                        if (M5Cardputer.Keyboard.isPressed()) {
                            const auto status = M5Cardputer.Keyboard.keysState();
                            if (status.opt) break;
                        }
                        if (millis() - startAttemptTime >= TIMEOUT) {
                            wait("TIMEOUT!", true);
                            break;
                        }
                    }
                }
            } else if (text == "req") {
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
                linear();
            } else if (text == "wave") {
                delay(250);
                wave();
            } else if (text == "quadratic") {
                delay(250);
                quadratic();
            } else if (text == "battery") {
                wait("Level: " + String(M5Cardputer.Power.getBatteryLevel()), true);
            } else if (text == "files") {
                fileExplorer();
            } else if (text == "help") {
                std::vector<String> options = {
                    "scan      - scan and connect network",
                    "req       - get request",
                    "music     - play frequencies",
                    "irNec     - send IR",
                    "irSam     - send IR (samsung TV)",
                    "tvOff     - turn off samsung TV",
                    "rem       - remote for samsung TV",
                    "linear    - plot linear function",
                    "wave      - plot wave(sine) function",
                    "quadratic - plot quadratic function",
                    "battery   - check battery percentage",
                    "files     - browse files on sd card",
                };
                scrollTextArr(options, true);
            } else {
                Serial.println("Command not recognized");
                wait("Command not recognized", true);
            }

            M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
            M5Cardputer.Lcd.setTextColor(PRIM_FONT_COLOR);
            text = "";
        }
        M5Cardputer.Lcd.fillScreen(TFT_BLACK);
        M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
        drawCommands();
        debounceKeyboard();
        timer = 0;
    }
    if (timer > 10000) esp_deep_sleep_start();

    timer++;
    delay(1);
}
