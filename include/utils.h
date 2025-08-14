#ifndef UTILS_H
#define UTILS_H

/**
 * prompt the user to input text
 * @param msg prompt message
 * @return
 */
String prompt(String msg);

/**
 * wait until user action
 * @param msg wait message
 * @param clearScreen the screen can be left uncleared
 */
void wait(String msg, bool clearScreen);

/**
 * show information
 * @param msg message
 */
void info(String msg);

/**
 * scroll singular text
 * @param msg message to scroll
 * @param scrollX can scroll sideways
 */
void scrollText(String msg, bool scrollX);

/**
 * scroll through an array of texts
 * @param msg message to scroll
 * @param scrollX can scroll sideways
 */
void scrollTextArr(std::vector<String> msg, bool scrollX);

/**
 * scroll through array of a text and return selected
 * @param msg message to scroll
 * @param scrollX can scroll sideways
 * @param mainColor main(regular) color
 * @param extraColor extra(highlight) color
 * @return highlighted text on exit
 */
String scrollTextArrHighlight(std::vector<String> msg, bool scrollX, int mainColor, int extraColor);

/**
 * Ask a yes / no question
 * @param msg question
 */
bool yesNoPopup(String msg);

/**
 * debounce keyboard
 */
void debounceKeyboard();

#endif
