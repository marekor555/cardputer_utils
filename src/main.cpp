#include <M5Cardputer.h>
#include <vector>
#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.h>
#include <driver/uart.h>
#include "utils.h"
#include "config.h"
#include "apps.h"


std::vector<String> commandList = {
    "help",
    "snake",
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
    "open-meteo",
    "snow",
    "clicker",
};

String text = "", lastCmd = "";
int timer = 0, selectedCmd = 0;
bool change = true;

std::vector<String> available = {};


std::vector<String> getAvailableCommands(String command, std::vector<String> cmdList) {
    if (command.length() == 0 || command.isEmpty()) return cmdList;
    std::vector<String> availableCommands;
    for (const auto i : cmdList) {
        if (i.startsWith(command)) availableCommands.push_back(i);
    }
    if (selectedCmd >= availableCommands.size())
        selectedCmd = availableCommands.size() - 1;
    return availableCommands;
}

void drawCommands() {
    if (selectedCmd == -1 && available.size() > 0) {
        selectedCmd = 0;
    }
    int shift = 0;
    if (selectedCmd > 8) {
        shift = (selectedCmd - 8) * -10;
    }
    M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
    M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
    for (int i = 0; i < available.size(); i++) {
        if (selectedCmd == i) {
            M5Cardputer.Lcd.fillRect(10, 30+i*10+shift, 100, 10, TFT_DARKGREY);
        }
        M5Cardputer.Lcd.drawString(available[i], 10, 30+i*10+shift);
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
            debounceKeyboard();
        }
    }
}

void setup() {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
    IrSender.begin(IR_PIN);
    WiFi.mode(WIFI_STA);
    Serial.begin(BAUD);
    const auto cfg = m5::M5Unified::config();
    M5Cardputer.begin(cfg, true);

    M5Cardputer.Lcd.setBrightness(BRIGHTNESS);
    M5Cardputer.Lcd.setRotation(1);
}

void loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isPressed()) {
        const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        for (const auto i: status.word) {
            switch (i) {
                case ';':
                    if (selectedCmd > 0) {
                        selectedCmd--;
                        change = true;
                    }
                    break;
                case '.':
                    if (selectedCmd < commandList.size() - 1) {
                        selectedCmd++;
                        change = true;
                    }
                    break;
                default:
                    text += i;
                    change = true;
            }
        }
        if (status.del) {
            text.remove(text.length() - 1);
            change = true;
        }
        if (status.fn) {
            text = lastCmd;
            change = true;
        }
        if (status.tab) {
            text = available[selectedCmd];
            change = true;
        }
        if (status.enter) {
            text = available[selectedCmd];
            lastCmd = text;
            change = true;

            M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
            M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
            if (text == "scan") {
                scanAndConnectNetwork();
            } else if (text == "req") {
                webRequest();
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
            } else if (text == "open-meteo") {
                openMeteo();
            } else if (text == "snake") {
                snakeGame();
            } else if (text == "snow") {
                snowAnimation();
            } else if (text == "clicker") {
                clicker();
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
            text = "";
        }
        debounceKeyboard();
        timer = 0;
    }
    if (change) {
        M5Cardputer.Lcd.setTextColor(PRIM_FONT_COLOR);
        M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
        M5Cardputer.Lcd.fillScreen(TFT_BLACK);
        available = getAvailableCommands(text, commandList);
        drawCommands();
        M5Cardputer.Lcd.fillRect(0, 0, 128, 25, TFT_BLACK);
        M5Cardputer.Lcd.drawString(PROMPT + text, 10, 10);
        change = false;
    }
    if (timer > SLEEP_TIME) {
        asleep();
        timer = 0;
        change = true;
    }

    timer++;
    delay(1);
}
