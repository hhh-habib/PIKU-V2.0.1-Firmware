# PIKU V2 Software Architecture

Stable OOP Baseline v1 documents the current hardware-tested structure of the PIKU V2 firmware. This file is a baseline freeze: it describes the current architecture and protected values, and it should be updated only when a planned firmware change is tested.

## Architecture Layers

### Config

- `PinConfig`: central GPIO mapping for TFT, motors, servo, ultrasonic sensor, DHT sensor, MQ2 sensor, and buzzer.
- `RobotConfig`: shared tuning constants for TFT size, DHT type, obstacle threshold, gas thresholds, servo scan angles, and publish/update intervals.

### Hardware Driver Layer

- `MotorController`: motor hardware driver for the L298N input pins. It owns `MotorState`, digital HIGH/LOW drive behavior, pivot turns, spin turns, transition coasting, and stable startup timing. No PWM is currently used.
- `ServoScanner`: servo positioning wrapper for center, left, right, arbitrary angle writes, attach, and detach behavior.
- `SensorManager`: raw sensor access for ultrasonic distance, DHT temperature/humidity, and MQ2 analog readings.
- `DisplayManager`: ST7789 TFT rendering for the robot dashboard and face/status display.

### Application / Service Layer

- `DriveCommandHandler`: manual/dashboard command routing for `FORWARD`, `BACKWARD`, `LEFT`, `RIGHT`, and `STOP`, including pivot/spin turn selection.
- `DashboardPublisher`: publishes the current robot state to the TFT display, web dashboard data model, and serial log.
- `WebDashboard`: local browser dashboard server, mode/turn-mode endpoints, command endpoints, and `/data` JSON output.

### Coordinator

- `main.cpp`: coordinates object wiring, setup, loop execution, mode switching, manual obstacle safety, sensor read/publish flow, scan sequencing, and auto navigation.

## Protected Stable Baseline Values

### Motor Timing

- `DIRECTION_SETTLE_MS = 5`
- `FORWARD_STARTUP_COMPENSATION_MS = 80`
- `BACKWARD_STARTUP_COMPENSATION_MS = 0`
- No PWM is currently used.

### Servo Scan

- `CENTER_SCAN = 90`
- `LEFT_SCAN = 160`
- `RIGHT_SCAN = 20`

### Pins

- `MQ2_PIN = GPIO35`
- `BUZZER_PIN = GPIO33`

### Robot Behavior

- Manual mode is default.
- Pivot and spin turns both exist.
- Auto navigation is intentionally still in `main.cpp`.
- MQ2 investigation/calibration is paused.

## Do Not Break

- Do not tune motors without hardware testing.
- Do not add PWM without an explicit decision.
- Do not change servo scan angles casually.
- Do not extract `AutoNavigator` unless planned and tested.
- Do not modify MQ2 thresholds while MQ2 investigation is paused.
- Do not change manual mode default.

## Remaining main.cpp Responsibilities

- Object wiring.
- Setup/loop coordination.
- Manual obstacle safety.
- Sensor read/publish coordination.
- `scanAt` sequence.
- Auto navigation logic.
- Gas status classification.

## Future Refactor Candidates

- `AutoNavigator` extraction later.
- Gas/safety status manager later.
- Optional wrapper cleanup later.
- Buzzer restoration later.
- PIKU V2.1 sensors later.
