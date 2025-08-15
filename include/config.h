/*
 * This file may be shared.
 * By MAREKOR555
 * Config file for cardputer_utils
 */
#pragma once
#include <M5Cardputer.h>

#ifndef CONFIG_H
#define CONFIG_H

#define BRIGHTNESS 255
#define SLEEP_BRIGHTNESS 0

// Serial baud rate
#define BAUD 115200

// Time until cardputer enters sleep
#define SLEEP_TIME 5000

// Command prompt symbol used in the text interface
#define PROMPT "> "

// Primary font size used for main text and UI elements
#define PRIM_FONT_SIZE 2

// Primary font color used for main text and UI elements
#define PRIM_FONT_COLOR TFT_GOLD

// Secondary font size used for additional information and smaller UI elements
#define SEC_FONT_SIZE 1

// Secondary font color used for additional information and smaller UI elements
#define SEC_FONT_COLOR TFT_WHITE

// Global timeout in milliseconds for network operations and connections
#define TIMEOUT 10000

// Graph plotting configuration
// Left-edge X-coordinate for graph plotting (virtual coordinate space)
#define POINT1_X -240

// Right-edge X-coordinate for graph plotting (virtual coordinate space)
#define POINT2_X 240

// Grid size and scaling factor for graph plotting
// Controls the visual spacing between grid lines and coordinate scaling
#define STEP 10.0

// Grid size and scaling factor for sine plotting
// Controls the visual spacing between grid lines and coordinate scaling
#define STEP_SIN 20.0

// Pin for the Infrared (IR) transmitter
// Used for remote control functionality
#define IR_PIN 44

// Value used for control of music speed
#define MUSIC_SPEED 1

#endif
