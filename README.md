
# 📄 Licensing

cardputer_utils  © 2025 by Marekor555 is licensed under CC BY-SA 4.0. To view a copy of this license, visit https://creativecommons.org/licenses/by-sa/4.0/

This software is provided "as-is", without any warranty. The author is not responsible for any damages, data loss, or other issues arising from its use.


This project uses third-party libraries.  
Their licenses can be found in the library_licenses/ folder.

# 📟 M5Cardputer Command Suite

A multifunctional firmware for the **M5Stack Cardputer**, written for PlatformIO.  
Includes tools for network scanning, HTTP requests, playing musical tones, sending IR commands (Samsung/NEC), and even a
basic remote interface.

The project is still in development, but it is functional. The docs may be incomplete or wrong.

## 🛠 Requirements

- **Hardware**: [M5Stack Cardputer](https://shop.m5stack.com/products/m5cardputer-esp32-s3)
- **Software**: [PlatformIO](https://platformio.org/) with ESP32 support

---

## 📁 Project Structure

This project focuses on the `main.cpp` file, which includes:

- **Wi-Fi utilities**: Scan and connect to Wi-Fi networks
- **HTTP client**: Send simple GET requests
- **Music player**: Play a sequence of tones
- **IR controller**: Send Samsung/NEC IR signals
- **Graphing**: Placeholder for linear graph visualization

## ⌨️ Available Commands

Enter these keywords via the Cardputer’s keyboard:

|  Command  |           Description           |
|:---------:|:-------------------------------:|
|  `scan`   |    Scan nearby WiFi networks    |
|   `req`   |   Send a GET request to a URL   |
|  `music`  |   Input and play frequencies    |
|  `irNec`  |      Send raw NEC IR code       |
|  `irSam`  |    Send raw Samsung IR code     |
|  `tvOff`  | Send Samsung "Power Off" signal |
|   `rem`   |   Samsung TV remote emulator    |
| `linear`  | Graph plotting (if implemented) |
| `battery` |       get battery charge        |
|  `files`  |      list files on SD card      |
|  `help`   |      Show list of commands      |

---

## 🧪 Special Modes

### 🎵 Music Mode

Enter frequencies as numbers, then type:

- `play` — to play the list
- `exit` — to exit
- `tab` — delete last frequency
- `del` — delete last character

### 📈 Graph Plotting

Use the `linear` command to graph a linear function on the screen.  
You will be prompted to enter the slope (`A`) and y-intercept (`B`) values.  
The graph will display with axes and a grid.

### 📺 Samsung Remote

Keys:

- `;` → Volume +
- `.` → Volume -
- `/` → Channel +
- `,` → Channel -
- `m` → Mute
- `o` → Play
- `p` → Pause

### 🌐 HTTP Requests

- Accepts URLs like `example.com`
- Response content is scrollable on screen

### 🗃️ File browser for SD card

- browse files
- read files
- playing audio files (16bit signed, WAV format)
- file operations (edit, create, delete)

#### Audio player keybinds
| Button  | Action         |
|---------|----------------|
| opt     | exit file      |
| Up/Down | volume up/down |

#### how to edit files
| Button | Action                              |
|--------|-------------------------------------|
| opt    | exit file                           |
| fn     | enter move mode( move with arrows ) |
| del    | delete character                    |

### 📻 Morse code generator

- enter text to convert into morse code
- press ``enter`` to play morse code

### ❄️ Snow animation

- snowflake falling animation
- press ``opt`` to stop animation

### 🧮 Counter clicker

- press ``Up`` to increase the value
- press ``Down`` to decrease the value
- press ``opt`` to exit 

### 🐍 Snake game

- press ``Up`` to move up
- press ``Down`` to move down
- press ``Left`` or A to move left
- press ``Right`` or S to move right
- press P or ' to pause
- press `opt` to exit

---

## 🔧 Configuration

Almost every configuration value is defined in `config.h`. To see usages of these values, consider looking into code.
Some of these values should not be changed.
Documentation is in progress.

## 🚀 Flashing the Project

```bash
platformio run --target upload
```

## 🧠 Notes

- The screen auto-sleeps after ~5 seconds of inactivity
- Wakes up on any key change