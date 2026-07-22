# PIKU V2 Firmware

PIKU V2 is a private ESP32 DOIT DevKit V1 firmware repository for the PIKU 2.0 autonomous environmental inspection robot. The current documented release is **PIKU V2.0.1 - Enhanced Inspection, Safety and Obstacle Recovery Upgrade**.

This firmware preserves the stable PIKU 2.0 behavior while adding a stronger inspection and safety layer: flame detection, close-range IR obstacle backup, physical buzzer control, dashboard alarm controls, and safer autonomous recovery behavior.

## Current Capabilities

- Manual and autonomous control through the ESP32 Wi-Fi access point dashboard
- L298N motor control using four direction GPIO pins
- GPIO25 and GPIO14 reserved for future L298N ENA/ENB PWM
- HC-SR04 ultrasonic navigation with servo-mounted center/left/right scanning
- IR close-range backup obstacle detection on GPIO39
- Flame detection on GPIO36
- MQ-2 analog gas monitoring on GPIO35 through a voltage divider
- DHT22 temperature and humidity monitoring
- ST7789 TFT local status display
- GPIO33 physical buzzer with non-blocking alarm patterns
- Dashboard alarm controls: enable, mute, and test beep

## Safety Model

Permanent critical safety stop is limited to:

- Flame detected
- MQ-2 high-risk gas condition
- Critical high temperature

When a critical hazard is active, the firmware stops the motors, clears pending movement intent, blocks all movement except STOP, and keeps the hazard visible on the TFT and dashboard. If the critical hazard clears after the configured debounce/hysteresis rules, the robot remains stopped. The user must reselect AUTO mode or issue a new valid manual command before movement resumes.

IR obstacle detection is not a permanent critical hazard. It is treated as a close-range collision backup:

- In AUTO mode, a debounced near obstacle stops the robot, triggers a temporary warning, reverses, performs the existing servo plus HC-SR04 left/right scan, turns toward the clearer side, and resumes autonomous navigation.
- In MANUAL mode, forward movement is blocked while a near obstacle is active, but backward, left, right, and stop commands remain available unless a critical safety hazard is active.

## Pin Map

| Function | GPIO |
| --- | ---: |
| TFT CS | 5 |
| TFT DC | 22 |
| TFT RST | 4 |
| TFT SCK | 18 |
| TFT MOSI | 23 |
| TFT MISO | -1 / unused |
| TFT SS | -1 / unused |
| Right motor IN1 | 26 |
| Right motor IN2 | 27 |
| Left motor IN1 | 16 |
| Left motor IN2 | 17 |
| Right ENA PWM reserved | 25 |
| Left ENB PWM reserved | 14 |
| Servo signal | 13 |
| HC-SR04 trigger | 32 |
| HC-SR04 echo | 34 |
| DHT22 data | 21 |
| MQ-2 analog output | 35 |
| Buzzer | 33 |
| Flame sensor | 36 |
| IR obstacle sensor | 39 |

GPIO34, GPIO35, GPIO36, and GPIO39 are ESP32 input-only pins. GPIO35 uses the MQ-2 voltage divider input. GPIO25 and GPIO14 must remain unused by sensors so they are available for future motor PWM.

## Firmware Structure

```text
.
|-- platformio.ini
|-- src/
|   |-- main.cpp
|   |-- MotorController.cpp
|   |-- ServoScanner.cpp
|   |-- SensorManager.cpp
|   |-- SafetyManager.cpp
|   |-- BuzzerManager.cpp
|   |-- DisplayManager.cpp
|   |-- DashboardPublisher.cpp
|   `-- WebDashboard.cpp
|-- include/
|   |-- PinConfig.h
|   |-- RobotConfig.h
|   |-- MotorController.h
|   |-- ServoScanner.h
|   |-- SensorManager.h
|   |-- SafetyManager.h
|   |-- BuzzerManager.h
|   |-- DisplayManager.h
|   |-- DashboardPublisher.h
|   |-- DriveCommandHandler.h
|   `-- WebDashboard.h
|-- docs/
|-- lib/
|-- test/
`-- README.md
```

Local generated or private folders such as `.pio/`, `.vscode/`, `backup/`, `.codex/`, and `.agents/` are excluded from Git.

## Build

The project is built with PlatformIO:

```sh
pio run
```

On the current Windows development machine, PlatformIO Core can also be invoked with:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

Target environment:

- PlatformIO board: `esp32doit-devkit-v1`
- Framework: Arduino
- Monitor speed: `115200`
- Upload speed: `115200`

## Upload

Connect the ESP32 board and run:

```sh
pio run --target upload
```

Avoid committing machine-specific upload ports unless the project intentionally standardizes one.

## Dashboard

The firmware starts a local ESP32 access point and serves the robot dashboard from the ESP32. Dashboard controls include:

- AUTO / MANUAL mode selection
- Pivot / spin turn mode selection
- Manual movement buttons
- Sensor telemetry and safety state
- Alarm sound state and buzzer state
- Enable alarm, mute alarm, and test beep controls

The AP SSID and password are defined in `src/main.cpp`. Treat them as local device credentials and change or externalize them before any broader distribution.

## Calibration Notes

- MQ-2 thresholds are raw ADC values after the GPIO35 voltage divider and require later calibration.
- Current gas thresholds are intentionally preserved in V2.0.1.
- IR detection distance should be adjusted physically using the IR module potentiometer.
- The current circuit diagram has not been updated for the V2.0.1 sensor and buzzer upgrade.

## Documentation

- Architecture: `docs/SOFTWARE_ARCHITECTURE.md`
- Project status: `docs/PROJECT_STATUS.md`
- Release notes: `docs/RELEASE_NOTES_V2.0.1.md`
- Research paper update checklist: `docs/RESEARCH_PAPER_UPDATE_TODO.md`
- Changelog: `CHANGELOG.md`
