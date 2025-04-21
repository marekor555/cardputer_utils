/*
 * This file may be shared.
 * By MAREKOR555
 * Config file for cardputer_utils
 */

#ifndef CONFIG_H
#define CONFIG_H

// Maximum length for recording operations (in samples)
#define RECORDING_LENGHT 200

// Command prompt symbol used in the text interface
#define PROMPT "> "

// Primary font size used for main text and UI elements
#define PRIM_FONT_SIZE 2

// Secondary font size used for additional information and smaller UI elements
#define SEC_FONT_SIZE 1

// Global timeout in milliseconds for network operations and connections
#define TIMEOUT 5000

// Graph plotting configuration
// Left-edge X-coordinate for graph plotting (virtual coordinate space)
#define POINT1_X -240

// Right-edge X-coordinate for graph plotting (virtual coordinate space)
#define POINT2_X 240

// Grid size and scaling factor for graph plotting
// Controls the visual spacing between grid lines and coordinate scaling
#define STEP 10

// Pin for the Infrared (IR) transmitter
// Used for remote control functionality
#define IR_PIN 44

// Value used for controll of music speed
#define MUSIC_SPEED 1

#endif
