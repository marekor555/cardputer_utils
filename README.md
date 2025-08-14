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
- [SOON] file operations (edit, create, delete)

---

## 🔧 Configuration

Every configuration value is defined in `config.h`. To see usages of these values consider looking into code.
Documentation is in progress.

## 🚀 Flashing the Project

```bash
platformio run --target upload
```

## 🧠 Notes

- The screen auto-sleeps after ~5 seconds of inactivity
- Wakes up on any key change
- Code uses `M5Unified` for screen and keyboard handling, which uses MIT license
- WARNING: this code is not open source, you can only share with others include/config.h, other parts aren't allowed to
  be shared.