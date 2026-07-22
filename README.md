# PIKU V2.0.1 - Enhanced Inspection, Safety and Obstacle Recovery Platform

## Overview

PIKU V2.0.1 is the private ESP32 firmware repository for the enhanced PIKU 2.0 environmental inspection robot. It preserves the original V2.0 movement, dashboard, sensing, and display baseline while adding a hardware-tested safety and obstacle recovery layer for the upgraded prototype.

The firmware targets the ESP32 DOIT DevKit V1 with Arduino through PlatformIO. It is a research and prototype firmware release, not a certified industrial safety product.

## What Changed From V2.0

- Added IR close-range obstacle detection on GPIO39.
- Added flame detection on GPIO36.
- Added physical buzzer alarm output on GPIO33.
- Added `SafetyManager` for central hazard classification and movement blocking.
- Added `BuzzerManager` for non-blocking alarm/test buzzer patterns.
- Added MQ-2 warm-up state and filtered ADC handling while preserving existing threshold values.
- Added Near Obstacle Hazard handling for IR or valid front HC-SR04 distance at or below 7 cm.
- Added autonomous recovery for near-obstacle events.
- Added manual forward blocking while preserving backward, left, right, and stop escape commands.
- Added web dashboard alarm enable, mute, and test controls.
- Added TFT hazard and near-obstacle alert views.
- Improved ultrasonic invalid-read handling for safer navigation decisions.

## Key Features

- Manual and autonomous drive modes through a local ESP32 Wi-Fi dashboard.
- L298N four-pin motor direction control with pivot and spin turn modes.
- Servo-mounted HC-SR04 center, left, and right scanning.
- DHT22 temperature and humidity telemetry.
- MQ-2 gas monitoring on GPIO35 through a voltage divider.
- Critical stop behavior for flame, high-risk gas, and critical temperature.
- Close-range collision backup using IR obstacle sensing.
- Physical and browser-side alarm controls.
- ST7789 TFT telemetry and alert display.

## Hardware

- ESP32 DOIT DevKit V1.
- L298N motor driver and mobile robot chassis.
- HC-SR04 ultrasonic sensor on a servo scanner.
- DHT22 temperature and humidity sensor.
- MQ-2 gas sensor output connected to ESP32 ADC through a voltage divider.
- Flame sensor digital output.
- IR obstacle sensor digital output.
- GPIO33 buzzer.
- ST7789 TFT display.

## Final GPIO Pin Map

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
| MQ-2 analog output through divider | 35 |
| Buzzer output | 33 |
| Flame sensor input | 36 |
| IR obstacle sensor input | 39 |

GPIO34, GPIO35, GPIO36, and GPIO39 are ESP32 input-only pins. GPIO25 and GPIO14 are reserved for future L298N ENA/ENB PWM and must not be reused by added sensors.

## Environmental Safety System

The firmware separates critical safety stops from recoverable near-obstacle events.

Critical `SAFETY_STOP` is used only for:

- Flame detected.
- MQ-2 high-risk gas condition.
- Critical temperature.

When critical safety is active, movement is stopped, pending movement intent is cleared, and all movement except STOP is blocked. The hazard remains visible on the TFT and dashboard. Alarm mute affects sound only; it does not clear the hazard state or motor stop.

## Near Obstacle Hazard

Near Obstacle Hazard is active when either condition is true:

- IR obstacle detected.
- Valid front-facing HC-SR04 distance is less than or equal to 7 cm.

This is treated as a collision avoidance and recovery condition, not as a permanent critical safety stop.

## Autonomous Recovery

In AUTO mode, a near-obstacle event follows this recovery flow:

1. Stop immediately.
2. Publish warning/recovery telemetry.
3. Continue servicing sensors, dashboard, TFT, and buzzer.
4. Abort into critical stop if flame, high-risk gas, or critical temperature appears.
5. Reverse for the configured recovery interval.
6. Stop again.
7. Scan left and right using the servo-mounted HC-SR04.
8. Turn toward the clearer valid side.
9. Resume autonomous navigation after cooldown.

## Manual Safety Behavior

- STOP is always accepted.
- Critical hazards block all movement except STOP.
- Near-obstacle hazards block FORWARD only.
- BACKWARD, LEFT, and RIGHT remain available for escape when no critical hazard is active.

## Web Dashboard

The ESP32 hosts a local Wi-Fi dashboard with:

- AUTO and MANUAL mode controls.
- Pivot and spin turn selection.
- Manual movement controls.
- Distance, temperature, humidity, gas, flame, IR, safety, and navigation telemetry.
- Alarm reason and alarm sound state.
- Alarm enable, mute, and test beep controls.

The access point SSID and password are defined in `src/main.cpp`. Treat them as local device credentials and change or externalize them before broader distribution.

## TFT Alerts

The ST7789 display shows normal telemetry plus alert views for:

- Critical hazards.
- Near-obstacle warnings.
- Flame and IR states.
- Navigation/recovery states.
- Alarm reason and safety state.

## Alarm System

The alarm system separates hazard truth from sound output:

- `SafetyManager` determines safety state and alarm reason.
- `BuzzerManager` drives GPIO33 non-blocking buzzer patterns.
- Dashboard audio/vibration follows alarm enabled and mute state.
- Muting silences browser and physical buzzer output but does not clear safety state.
- Test beep requests a short physical buzzer test and browser-side test sound where supported.

## Sensor Scheduling

The firmware uses scheduled sensor reads and cached snapshots to keep the service loop responsive:

- DHT22 is read at a slower interval suitable for the sensor.
- MQ-2 ADC is filtered and includes warm-up status.
- HC-SR04 invalid reads are tracked and handled separately from valid distance values.
- IR and flame inputs are debounced before safety decisions.

## Software Architecture

Core modules:

- `main.cpp`: coordinator for setup, loop, safety enforcement, manual command handling, autonomous navigation, and recovery.
- `MotorController`: L298N direction control and motor state.
- `ServoScanner`: servo positioning and side scans.
- `SensorManager`: DHT22, MQ-2, HC-SR04, IR, and flame sensing.
- `SafetyManager`: critical, caution, near-obstacle, ultrasonic warning, alarm reason, and movement-blocking decisions.
- `BuzzerManager`: GPIO33 alarm output and test patterns.
- `DisplayManager`: TFT telemetry and alert screens.
- `DashboardPublisher`: shared telemetry publishing.
- `WebDashboard`: local web server, dashboard UI, JSON endpoint, drive/mode/alarm endpoints.

See `docs/SOFTWARE_ARCHITECTURE.md` for more detail.

## Project Structure

```text
.
|-- platformio.ini
|-- src/
|-- include/
|-- docs/
|-- lib/
|-- test/
|-- CHANGELOG.md
`-- README.md
```

Generated and local-only folders such as `.pio/`, `.vscode/`, `.codex/`, `.agents/`, and `backup/` are excluded from Git.

## Build and Upload

Build:

```sh
pio run -e esp32doit-devkit-v1
```

Windows PlatformIO Core path used on the maintainer machine:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run -e esp32doit-devkit-v1
```

Upload:

```sh
pio run -e esp32doit-devkit-v1 --target upload
```

Avoid committing machine-specific upload ports unless the project intentionally standardizes one.

## Hardware Validation

This repository contains the final V2.0.1 upgraded firmware as provided by the maintainer. Do not add test measurements, gas concentration claims, or safety certification claims unless they are backed by recorded validation data.

## Calibration Notes

- MQ-2 thresholds are raw ADC values after the GPIO35 voltage divider and still require final calibration.
- MQ-2 warm-up and filtering improve firmware behavior but do not replace sensor calibration.
- IR detection distance must be tuned physically using the sensor module potentiometer.
- HC-SR04 mounting and servo alignment affect recovery decisions.

## Limitations

- Not a certified industrial life-safety device.
- MQ-2 readings are not calibrated gas concentration measurements.
- Current circuit documentation still needs a V2.0.1 hardware update.
- Autonomous recovery remains a prototype behavior and should be supervised during testing.
- Motor drift and chassis-specific behavior may require mechanical or PWM tuning later.

## Future Work

- Calibrate MQ-2 thresholds with the installed voltage divider and target environment.
- Update V2.0.1 circuit diagram and public hardware documentation.
- Add optional ENA/ENB PWM using reserved GPIO25/GPIO14 after hardware validation.
- Add host-side tests for safety state transitions where practical.
- Prepare separate public V2.0.1 hardware documentation after updated diagrams/photos are ready.

## Research Context

PIKU V2.0.1 is an enhanced firmware milestone for an ESP32-based autonomous environmental inspection robot prototype. It is intended for education, research demonstration, and controlled prototype development.

## Safety Disclaimer

This firmware is experimental prototype software. It must not be used as a certified fire, gas, temperature, collision, or industrial safety system. Always test in controlled conditions and keep manual power cutoff available during robot operation.
