# PIKU V2 Firmware

PIKU V2 is an ESP32-based environmental inspection and monitoring robot firmware project. This repository is intended to hold the complete private firmware codebase for PIKU V2, including the current hardware-tested PlatformIO project, source modules, configuration headers, and project documentation.

The firmware supports:

- Manual control through a local ESP32 web dashboard
- Autonomous obstacle avoidance
- ST7789 TFT dashboard display
- Ultrasonic distance sensing
- DHT22 temperature and humidity readings
- MQ2 gas sensor readings
- Servo-based front/side scanning
- L298N motor control

## Environment

- Platform: PlatformIO
- Board: `esp32doit-devkit-v1`
- Framework: Arduino
- Main configuration: `platformio.ini`

Install PlatformIO in VS Code or use the PlatformIO Core CLI before building.

## Repository Structure

```text
.
|-- platformio.ini      PlatformIO environment and library dependencies
|-- src/                Firmware implementation files
|-- include/            Header files and hardware/config constants
|-- lib/                Project-local libraries, if needed later
|-- test/               PlatformIO tests, if added later
|-- docs/               Firmware architecture and project documentation
|-- README.md           Repository overview and build instructions
`-- .gitignore          Local/generated file exclusions
```

Local development folders such as `.pio/`, `.vscode/`, `backup/`, `.codex/`, and `.agents/` are kept on the computer but excluded from Git.

## Build

From this folder, run:

```sh
pio run
```

If PlatformIO is installed only inside the Windows user environment, the equivalent command is:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

## Upload

Connect the ESP32 board, then run:

```sh
pio run --target upload
```

If needed, set an upload port in `platformio.ini` or pass one through PlatformIO CLI options. Avoid committing machine-specific upload ports unless the project intentionally standardizes one.

## Dashboard Access Point

The firmware currently starts a local ESP32 access point for the dashboard. The default SSID/password are defined in `src/main.cpp` and passed into `WebDashboard::begin()`. This is acceptable for a private firmware repository when treated as a local device password, but it should be changed or moved to private configuration before any public release or shared deployment.

## Documentation

The current architecture notes live in `docs/`. Keep firmware documentation focused on software architecture, hardware pin mapping, build/upload workflow, tested baseline behavior, and future firmware plans. Do not add unpublished research manuscripts or unrelated academic draft material to this repository.
