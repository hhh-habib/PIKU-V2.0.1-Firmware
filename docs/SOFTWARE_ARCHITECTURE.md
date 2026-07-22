# PIKU V2.0.1 Software Architecture

PIKU V2.0.1 keeps the original stable PIKU 2.0 object-oriented firmware layout and extends it with safety supervision, alarm control, and close-range obstacle recovery. The architecture intentionally avoids a broad refactor so the tested motor, servo, sensor, TFT, and dashboard behavior remains familiar.

## Layers

### Configuration

- `PinConfig` owns the complete GPIO map.
- `RobotConfig` owns tuning constants for display size, sensor thresholds, debounce timing, servo scan angles, navigation timings, alarm durations, and sampling intervals.

### Hardware and Device Managers

- `MotorController` owns the L298N input-pin drive behavior, motor states, pivot turns, spin turns, coasting transitions, and stable startup timing. PWM is not implemented in this release.
- `ServoScanner` owns center, left, right, and arbitrary servo positioning.
- `SensorManager` owns DHT22, MQ-2 ADC filtering, HC-SR04 distance sampling, IR obstacle input, flame input, sensor intervals, and cached sensor snapshots.
- `BuzzerManager` owns GPIO33 alarm output, non-blocking software patterns, alarm enable/mute handling, and short test-beep behavior.
- `DisplayManager` owns the ST7789 TFT telemetry view plus full-screen critical and near-obstacle warnings.

### Safety and Application Services

- `SafetyManager` classifies critical hazards, caution states, near-obstacle hazards, ultrasonic warnings, motion blocking, forward-only blocking, and alarm reasons.
- `DriveCommandHandler` routes manual drive commands to `MotorController`.
- `DashboardPublisher` publishes the current sensor, motor, navigation, safety, alarm, and buzzer state to the TFT, web dashboard model, and serial telemetry.
- `WebDashboard` owns the ESP32 web server, dashboard HTML/JavaScript, movement endpoints, mode endpoints, `/data` JSON endpoint, and alarm endpoints.

### Coordinator

- `main.cpp` wires the managers together, runs the service loop, handles mode changes, enforces critical safety stops, manages manual command acceptance, executes autonomous navigation, and runs IR/near-obstacle recovery.

## Safety Model

Critical hazards are permanent movement blockers while active:

- Flame detected
- MQ-2 high-risk gas condition
- Critical high temperature

`SafetyManager::blocksMotion()` returns true only for these critical hazards. When active, the coordinator stops motors, clears pending command state, blocks all movement except STOP, displays the hazard, and activates alarms unless muted.

Near-obstacle hazards are recovery conditions, not critical safety stop conditions:

- IR close-range obstacle
- Valid front-facing HC-SR04 distance at or below `NEAR_OBSTACLE_CM`

`SafetyManager::blocksForwardMotion()` returns true for critical hazards, near-obstacle hazards, and unsafe/invalid ultrasonic warning conditions. This prevents unsafe forward motion while still allowing manual escape commands when no critical hazard is active.

## AUTO Near-Obstacle Recovery

The current autonomous recovery path is coordinated in `main.cpp`:

1. Stop motors immediately.
2. Publish an IR/near-obstacle recovery navigation state.
3. Sound a temporary caution or hazard alarm if sound is enabled.
4. Wait for the configured settle interval while servicing sensors, safety, dashboard, TFT, and buzzer updates.
5. Abort if a critical flame, gas, or temperature hazard appears.
6. Reverse using the existing tested reverse timing.
7. Stop.
8. Run the existing servo-mounted HC-SR04 left/right scan.
9. Turn toward the clearer valid side using existing turn behavior.
10. Resume AUTO navigation after the cooldown/latched recovery guard allows it.

The HC-SR04 remains the primary navigation sensor. IR is a close-range backup sensor whose physical detection distance should be tuned on the sensor module.

## Manual Mode Rules

- STOP is always accepted.
- Critical hazards block all movement except STOP.
- Near-obstacle hazards block FORWARD.
- BACKWARD, LEFT, and RIGHT remain available for escape when no critical hazard is active.

## Alarm Model

Alarm truth and alarm sound control are separate.

- Safety state and alarm reason come from sensor and safety logic.
- Sound output is controlled by alarm enabled/muted/test state.
- Muting silences both browser audio/vibration and physical GPIO33 buzzer output.
- Muting does not clear `safetyState`, `alarmReason`, TFT warnings, dashboard warnings, or critical motor stop.
- Test beep requests a short browser-side test sound and a short physical buzzer test.

## Protected Values

### Motor and PWM

- `DIRECTION_SETTLE_MS = 5`
- `FORWARD_STARTUP_COMPENSATION_MS = 80`
- `BACKWARD_STARTUP_COMPENSATION_MS = 0`
- GPIO25 and GPIO14 remain reserved for future L298N ENA/ENB PWM.

### Servo Scan

- `CENTER_SCAN = 90`
- `LEFT_SCAN = 160`
- `RIGHT_SCAN = 20`

### Sensor and Alarm Pins

- `MQ2_PIN = GPIO35`
- `BUZZER_PIN = GPIO33`
- `FLAME_PIN = GPIO36`
- `IR_OBSTACLE_PIN = GPIO39`

### MQ-2

- MQ-2 ADC input is on GPIO35 after a voltage divider.
- Current thresholds remain unchanged in V2.0.1.
- Thresholds require later calibration against the actual voltage-divider hardware and target gas response.

## Intentional Non-Changes

- No PWM motor control has been added yet.
- Servo scan angles are unchanged.
- Existing motor timing is preserved.
- The HC-SR04 remains the primary obstacle/navigation sensor.
- The circuit diagram has not been updated in this release.
- No research paper manuscript has been edited in this repository update.

## Future Work

- Calibrate MQ-2 caution/high-risk thresholds after voltage-divider validation.
- Update the circuit diagram for GPIO33 buzzer, GPIO36 flame sensor, GPIO39 IR sensor, and GPIO35 divider details.
- Consider extracting autonomous navigation into a dedicated class only after the current behavior is hardware-stable.
- Add firmware tests or host-side logic tests for safety state transitions where practical.
