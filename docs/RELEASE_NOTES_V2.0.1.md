# PIKU V2.0.1 Release Notes

## Enhanced Inspection, Safety and Obstacle Recovery Upgrade

PIKU V2.0.1 is a firmware upgrade for the existing PIKU 2.0 ESP32 inspection robot. It strengthens the robot's environmental inspection and safety behavior while preserving the stable OOP firmware structure and tested movement/navigation baseline.

## Highlights

- Added flame sensor monitoring on GPIO36.
- Added IR close-range obstacle backup monitoring on GPIO39.
- Added GPIO33 physical buzzer alarm support.
- Added dashboard alarm enable, mute, and test controls.
- Added firmware separation between safety state and alarm sound state.
- Added AUTO near-obstacle recovery behavior for IR/near-obstacle detections.
- Added MANUAL forward blocking while still allowing escape movement.
- Preserved GPIO25 and GPIO14 for future L298N ENA/ENB PWM.

## Safety Behavior

Permanent critical safety stop now comes only from:

- Flame detected
- MQ-2 high-risk gas
- Critical high temperature

IR obstacle detection is handled as a near-obstacle recovery event, not a permanent safety stop. The robot stops, warns, reverses, scans with the servo-mounted HC-SR04, turns toward the clearer side, and resumes autonomous navigation when safe to do so.

## Alarm Behavior

Dashboard mute silences both the browser alarm behavior and physical GPIO33 buzzer output. Muting does not clear the safety state, alarm reason, TFT warning, dashboard warning, or critical motor stop.

Dashboard test beep requests both a browser-side test sound and a short physical buzzer test.

## Calibration and Hardware Notes

- MQ-2 thresholds are unchanged in this release.
- MQ-2 calibration is still required after validating the GPIO35 voltage-divider behavior.
- The IR sensor detection distance should be tuned physically using the sensor potentiometer.
- The circuit diagram has not yet been updated for this release.

## Build Target

- PlatformIO environment: `esp32doit-devkit-v1`
- Framework: Arduino
- Main config: `platformio.ini`
