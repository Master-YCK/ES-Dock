# ES-Dock

## Project Background

An embedded UI project developed using the ESP32-S3 N16R8 (16 MB Flash, 8 MB PSRAM).

> This is an unofficial physical controller for a user’s existing Spotify playback device. It does not stream, store, or play Spotify audio, and it is not a Spotify Connect device.

Development is currently based on VS Code, PlatformIO, and the Arduino framework; future goals include integrating a TFT LCD, touch functionality, and the LVGL graphical user interface.

## Features

- ESP32-S3 N16R8 board support
- 16 MB Flash configuration
- 8 MB OPI PSRAM configuration
- PlatformIO + Arduino development workflow
- Serial startup messages, function test output, and a periodic heartbeat
- Planned: LVGL user interface
- Planned: TFT LCD and touch support
- Planned: Wi-Fi configuration and device dashboard

## Hardware

| Item | Specification |
|---|---|
| MCU | ESP32-S3 |
| Module | ESP32-S3 N16R8 |
| Flash | 16 MB |
| PSRAM | 8 MB Octal/OPI PSRAM |
| Framework | Arduino Core for ESP32 |
| Build system | PlatformIO |
| IDE | Visual Studio Code |

> This repository targets a generic ESP32-S3 N16R8 development board.
> Different clone boards may use different USB-to-UART chips, pinouts, LEDs,
> and boot/reset circuits.

## Software Requirements

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode)
- A USB data cable
- ESP32-S3 N16R8 development board

No standalone Arduino IDE is required.

## Getting Started

### 1. Clone the repository

Clone this repository from GitHub:

```bash
git clone https://github.com/Master-YCK/ES-Dock.git
cd ES-Dock
```

### 2. Open with PlatformIO

1. Open the project folder in VS Code.
2. Ensure the PlatformIO IDE extension is installed.
3. PlatformIO will install the required ESP32 toolchain and Arduino framework automatically on the first build.

### 3. Connect the board

Connect the board using the USB-C port marked `UART` or `COM`.

For initial development, use the USB-UART port rather than the native USB/OTG port.

### 4. Build and upload

Use the PlatformIO toolbar in VS Code:

- **Build**: compile the firmware
- **Upload**: flash the firmware to the ESP32-S3
- **Serial Monitor**: view runtime logs

Or use the PlatformIO CLI:

```bash
pio run
pio run --target upload
pio device monitor --baud 115200
```

### 5. Verify the board

The current firmware prints startup information, tests `myFunction(2, 3)`, and
prints a heartbeat approximately once per second.

Expected output:

```text
ES-Dock serial ready.
Baud rate: 115200
Result of myFunction(2, 3): 5
Heartbeat ms=...
```

## PlatformIO Configuration

The relevant configuration in `platformio.ini` is:

```ini
[env:esp32-s3-n16r8]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

board_build.flash_size = 16MB
board_upload.flash_size = 16MB
board_build.arduino.memory_type = qio_opi
board_build.arduino.partitions = default_16MB.csv

upload_speed = 460800
monitor_speed = 115200

lib_deps =
  lvgl/lvgl@^9.2.2

build_flags =
  -DBOARD_HAS_PSRAM
  -DARDUINO_USB_MODE=1
  -DARDUINO_USB_CDC_ON_BOOT=1
  -D LV_COLOR_DEPTH=16
  -D LV_FONT_MONTSERRAT_20=1
  -D LV_FONT_MONTSERRAT_32=1
  -D LV_FONT_MONTSERRAT_48=1x
```

`esp32-s3-devkitc-1` is used as a compatible PlatformIO board profile for a
generic ESP32-S3 N16R8 board. The Flash and PSRAM settings are explicitly
overridden for the N16R8 configuration.

## Upload Troubleshooting

If upload is stuck at `Connecting...`:

1. Confirm you are using a USB data cable.
2. Use the board's `UART` / `COM` USB-C port.
3. Close Serial Monitor before uploading.
4. Hold `BOOT`.
5. Press and release `RST` / `EN`.
6. Release `BOOT` after upload begins.
7. If the issue persists, lower `upload_speed` to `115200` in `platformio.ini`.

## Planned Display Setup

The planned UI stack is:

```text
ESP32-S3 N16R8
  └── Arduino framework
      ├── Arduino_GFX or TFT_eSPI
      ├── LVGL
      ├── SPI or RGB TFT display
      └── Optional capacitive touch controller
```

Before adding display code, document the display controller, resolution, GPIO wiring, and touch controller in this README.

## Security

Do not commit Wi-Fi passwords, MQTT credentials, API keys, or other secrets.

Store local credentials in `include/secrets.h`, which is ignored by Git:

```cpp
#pragma once

#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"
```

Create a committed template at `include/secrets.example.h`:

```cpp
#pragma once

#define WIFI_SSID "replace-me"
#define WIFI_PASSWORD "replace-me"
```

## License

This project is licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).