#ifndef UTILS_H
#define UTILS_H

/**
 * prompt user to input text
 * @param msg prompt message
 * @return
 */
String prompt(String msg);

/**
 * wait until user action
 * @param msg wait message
 * @param clearScreen screen can be left uncleared
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
 * scroll through array of texts
 * @param msg message to scroll
 * @param scrollX can scroll sideways
 */
void scrollTextArr(std::vector<String> msg, bool scrollX);

void graph();

#endif
