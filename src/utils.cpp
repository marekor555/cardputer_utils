#include <M5Cardputer.h>
#include "utils.h"
#include "config.h"


String prompt(const String msg) {
    String output = "";
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.drawString(msg, 10, 10);
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange()) {
            const Keyboard_Class::KeysState status_ = M5Cardputer.Keyboard.keysState();
            for (auto i: status_.word) {
                output += i;
            }
            if (status_.enter) {
                break;
            }
            if (status_.del) {
                output.remove(output.length() - 1);
            }
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            M5Cardputer.Lcd.drawString(msg + output, 10, 10);
            debounceKeyboard();
        }
    }
    return output;
}

void wait(const String msg, const bool clearScreen) {
    if (clearScreen) {
        M5Cardputer.Lcd.fillScreen(TFT_BLACK);
        M5Cardputer.Lcd.drawString(msg, 10, 10);
    }
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const auto status = M5Cardputer.Keyboard.keysState();
            if (status.opt) {
                break;
            }
        }
    }
}

void info(const String msg) {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.drawString(msg, 10, 10);
}


void scrollText(const String msg, bool scrollX) {
    int posx = 1, posy = 0;
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.setCursor(0, 10*posy);
    M5Cardputer.Lcd.println(msg);
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.word.size() > 0) {
                switch (status.word[0]) {
                    case ';':
                        if (posy<=-1)posy++;
                        break;
                    case '.':
                        posy--;
                        break;
                    case '/':
                        if (scrollX) posx--;
                        break;
                    case ',':
                        if (scrollX && posx < 1) posx++;
                        break;
                }
            }
            if (status.opt) {
                break;
            }
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            M5Cardputer.Lcd.setCursor(0, 10*posy);
            M5Cardputer.Lcd.println(msg);
            debounceKeyboard();
        }
    }
}

void scrollTextArr(const std::vector<String> msg, bool scrollX) {
    int posx = 1, posy = 0;
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    for (int i = 0; i<msg.size(); i++) {
        M5Cardputer.Lcd.drawString(msg[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
    }
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.word.size() > 0) {
                switch (status.word[0]) {
                    case ';':
                        if (posy<=-1)posy++;
                    break;
                    case '.':
                        posy--;
                    break;
                    case '/':
                        if (scrollX) posx--;
                    break;
                    case ',':
                        if (scrollX && posx < 1) posx++;
                    break;
                }
            }
            if (status.opt) {
                break;
            }
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            for (int i = 0; i<msg.size(); i++) {
                M5Cardputer.Lcd.drawString(msg[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
            }
            debounceKeyboard();
        }
    }
}

String scrollTextArrHighlight(const std::vector<String> msg, bool scrollX, int mainColor, int extraColor) {
    M5Cardputer.Lcd.setTextColor(mainColor);
    int posx = 1, posy = 0;
    int highlight = 0;
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    for (int i = 0; i<msg.size(); i++) {
        if (i == highlight) {
            M5Cardputer.Lcd.fillRect(10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1), 10*msg[i].length(), 10 * SEC_FONT_SIZE, TFT_DARKGRAY);
        }
        if (msg[i].startsWith("|") && msg[i].endsWith("|")) {
            M5Cardputer.Lcd.setTextColor(extraColor);
            M5Cardputer.Lcd.drawString(msg[i].substring(1, msg[i].length()-1), 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
            M5Cardputer.Lcd.setTextColor(mainColor);
        } else {
            M5Cardputer.Lcd.drawString(msg[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
        }
    }
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.word.size() > 0) {
                switch (status.word[0]) {
                    case ';':
                        if (posy<=-1) {
                            posy++;
                            highlight--;
                        }
                        break;
                    case '.':
                        posy--;
                        highlight++;
                        break;
                    case '/':
                        if (scrollX) posx--;
                        break;
                    case ',':
                        if (scrollX && posx < 1) posx++;
                        break;
                }
            }
            if (status.opt || status.enter) {
                break;
            }
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            for (int i = 0; i<msg.size(); i++) {
                if (i == highlight) {
                    M5Cardputer.Lcd.fillRect(10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1), 10*msg[i].length(), 10 * SEC_FONT_SIZE, TFT_DARKGRAY);
                }
                if (msg[i].startsWith("|") && msg[i].endsWith("|")) {
                    M5Cardputer.Lcd.setTextColor(extraColor);
                    M5Cardputer.Lcd.drawString(msg[i].substring(1, msg[i].length()-1), 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
                    M5Cardputer.Lcd.setTextColor(mainColor);
                } else {
                    M5Cardputer.Lcd.drawString(msg[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1));
                }
            }
            debounceKeyboard();
        }
    }
    return msg[highlight];
}

bool yesNoPopup(const String msg) {
    debounceKeyboard();
    M5Cardputer.Lcd.setTextColor(SEC_FONT_COLOR);
    M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
    const String options[2] = {"Yes", "No"};
    int posx = 1, posy = 0;
    int highlight = 0;
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
    M5Cardputer.Lcd.drawString(msg, 10, 10);
    M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
    for (int i = 0; i<2; i++) {
        if (i == highlight) {
            M5Cardputer.Lcd.fillRect(10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1) + 25, 10*4, 10 * SEC_FONT_SIZE, TFT_DARKGRAY);
        }
        M5Cardputer.Lcd.drawString(options[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1) + 25);
    }
    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isPressed()) {
            const Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.word.size() > 0) {
                switch (status.word[0]) {
                    case ';':
                        highlight--;
                        if (highlight < 0) highlight = 0;
                        break;
                    case '.':
                        highlight++;
                        if (highlight > 1) highlight = 1;
                        break;
                }
            }
            if (status.opt || status.enter) {
                break;
            }
            M5Cardputer.Lcd.fillScreen(TFT_BLACK);
            M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
            M5Cardputer.Lcd.drawString(msg, 10, 10);
            M5Cardputer.Lcd.setTextSize(SEC_FONT_SIZE);
            for (int i = 0; i<2; i++) {
                if (i == highlight) {
                    M5Cardputer.Lcd.fillRect(10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1) + 25, 10*4, 10 * SEC_FONT_SIZE, TFT_DARKGRAY);
                }
                M5Cardputer.Lcd.drawString(options[i], 10*posx, 10 * posy + 10 * SEC_FONT_SIZE * (i + 1) + 25);
            }
            debounceKeyboard();
        }
    }

    M5Cardputer.Lcd.setTextSize(PRIM_FONT_SIZE);
    M5Cardputer.Lcd.setTextColor(PRIM_FONT_COLOR);

    if (highlight == 0) return true;

    return false;
}

void debounceKeyboard() {
    while (M5Cardputer.Keyboard.isPressed()) {
        M5Cardputer.update();
        const auto status = M5Cardputer.Keyboard.keysState();
        if (status.shift || status.del) {
            delay(100);
            break;
        }
    }
}