# Esp piku Project Status

## Current file structure
- `.gitignore`
- `.pio/`
- `.vscode/`
- `backup/`
- `docs/`
  - `PROJECT_STATUS.md`
  - `SOFTWARE_ARCHITECTURE.md`
- `include/`
  - `DashboardPublisher.h`
  - `DisplayManager.h`
  - `DriveCommandHandler.h`
  - `MotorController.h`
  - `PinConfig.h`
  - `README`
  - `RobotConfig.h`
  - `SensorManager.h`
  - `ServoScanner.h`
  - `WebDashboard.h`
- `lib/`
- `platformio.ini`
- `src/`
  - `DashboardPublisher.cpp`
  - `DisplayManager.cpp`
  - `DriveCommandHandler.cpp`
  - `main.cpp`
  - `MotorController.cpp`
  - `SensorManager.cpp`
  - `ServoScanner.cpp`
  - `WebDashboard.cpp`
- `test/`

## Verified hardware features
- ESP32-based `esp32doit-devkit-v1`
- TFT display using `Adafruit_ST7789`
- Dual motor H-bridge control via four GPIO pins
- Ultrasonic distance sensing with HC-SR04-style trigger/echo pins
- DHT22 temperature and humidity sensor
- MQ-2 gas sensor input
- Servo scanner on a dedicated control pin
- Buzzer output pin available

## Stable OOP architecture
- `PinConfig` centralizes GPIO mapping
- `RobotConfig` centralizes shared tuning constants
- `MotorController` owns motor drive behavior, `MotorState`, pivot turns, spin turns, and stable motor timing
- `ServoScanner` owns servo positioning
- `SensorManager` owns raw DHT, MQ2, and ultrasonic reads
- `DisplayManager` owns TFT rendering
- `DriveCommandHandler` routes manual/dashboard drive commands
- `DashboardPublisher` publishes TFT, web, and serial state
- `WebDashboard` owns the local browser dashboard
- `main.cpp` remains the coordinator for setup, loop, mode switching, manual obstacle safety, sensor read/publish flow, scan sequencing, and auto navigation
- See `docs/SOFTWARE_ARCHITECTURE.md` for the baseline freeze

## Pin mapping
- TFT
  - `TFT_CS` = 5
  - `TFT_DC` = 22
  - `TFT_RST` = 4
  - `TFT_SCK` = 18
  - `TFT_MISO` = -1
  - `TFT_MOSI` = 23
  - `TFT_SS` = -1
- Motors
  - `RIGHT_IN1` = 26
  - `RIGHT_IN2` = 27
  - `LEFT_IN1` = 16
  - `LEFT_IN2` = 17
- Sensors
  - `SERVO_PIN` = 13
  - `TRIG_PIN` = 32
  - `ECHO_PIN` = 34
  - `DHT_PIN` = 21
  - `DHT_TYPE` = `DHT22`
  - `MQ2_PIN` = 35
  - `BUZZER_PIN` = 33

## Build status
- Build succeeded for `Esp piku`
- Verified with PlatformIO build using PlatformIO Core
- Firmware image generated successfully

## Upload status
- Upload task last executed successfully in the VS Code PlatformIO terminal for `Esp piku`

## Known limitation
- Slight left drift during forward motion

## Servo scan range
- Center scan position: `90`
- Left scan position: `160`
- Right scan position: `20`

## Buzzer status
- Buzzer pin exists as `BUZZER_PIN = 33`
- Buzzer restoration is a future candidate and is not part of the current stable baseline
