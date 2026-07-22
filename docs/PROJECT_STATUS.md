# PIKU V2.0.1 Project Status

## Release Identity

- Current firmware release: `v2.0.1`
- Release name: `Enhanced Inspection, Safety and Obstacle Recovery Upgrade`
- Board target: `esp32doit-devkit-v1`
- Framework: Arduino through PlatformIO
- Repository: `PIKU-V2-Firmware`

PIKU V2.0.1 is a stable PIKU 2.0 firmware upgrade. It must not be described as PIKU 2.1.

## Implemented Features

- Manual and autonomous drive modes
- ESP32 access point web dashboard
- ST7789 TFT local dashboard and warning display
- L298N four-pin motor drive
- Pivot and spin turn modes
- Servo-mounted HC-SR04 center/left/right scanning
- DHT22 temperature and humidity telemetry
- MQ-2 gas telemetry with filtered ADC value and warm-up state
- Flame sensor safety input
- IR close-range obstacle backup input
- GPIO33 physical buzzer alarm output
- Dashboard alarm enable, mute, and test controls
- Critical safety stop for flame, high-risk gas, and critical temperature
- AUTO near-obstacle recovery for IR/near-obstacle events
- MANUAL forward blocking with escape movement allowed for near-obstacle events

## Current Pin Mapping

| Area | Signal | GPIO |
| --- | --- | ---: |
| TFT | CS | 5 |
| TFT | DC | 22 |
| TFT | RST | 4 |
| TFT | SCK | 18 |
| TFT | MOSI | 23 |
| TFT | MISO | -1 / unused |
| TFT | SS | -1 / unused |
| Motor | Right IN1 | 26 |
| Motor | Right IN2 | 27 |
| Motor | Left IN1 | 16 |
| Motor | Left IN2 | 17 |
| Reserved | Right ENA PWM | 25 |
| Reserved | Left ENB PWM | 14 |
| Servo | Signal | 13 |
| HC-SR04 | Trigger | 32 |
| HC-SR04 | Echo | 34 |
| DHT22 | Data | 21 |
| MQ-2 | Analog output through divider | 35 |
| Buzzer | Output | 33 |
| Flame sensor | Digital input | 36 |
| IR obstacle sensor | Digital input | 39 |

## Safety Status

Critical safety hazards:

- Flame detected
- MQ-2 high-risk gas
- Critical high temperature

Near-obstacle recovery conditions:

- IR close-range obstacle
- Valid front-facing ultrasonic distance at or below the near-obstacle threshold

IR is not a permanent critical hazard. It supplements HC-SR04 navigation as a short-range backup collision sensor.

## Alarm Status

- Firmware exposes alarm enabled, muted, and test states.
- Physical buzzer output uses GPIO33.
- Browser audio and vibration respect dashboard mute state.
- Muting does not clear safety state or hazard display.
- Test beep requests both physical buzzer test and browser audio test where the browser supports it.

## Build Status

Build verification for this release should be performed with:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" run
```

The build result for the final committed release is recorded in the maintainer handoff or release notes.

## Known Limitations

- Circuit diagram is not yet updated for the V2.0.1 sensor and buzzer wiring.
- MQ-2 high-risk threshold remains the existing firmware value and still needs calibration after voltage-divider validation.
- IR detection distance is hardware-potentiometer dependent and should be tuned on the installed module.
- Some autonomous recovery actions still use short motor/servo timing waits, with responsive service-loop calls around longer waits.
- Slight forward drift noted in the earlier stable baseline remains a motor/mechanical tuning item.

## Release Readiness Checklist

- Keep GPIO25 and GPIO14 reserved for future motor PWM.
- Do not change servo scan angles without hardware testing.
- Do not tune motor timings casually.
- Do not change MQ-2 thresholds until calibration is performed.
- Update the circuit diagram before a public hardware-facing documentation release.
- Update the research paper materials using the TODO in `docs/RESEARCH_PAPER_UPDATE_TODO.md`.
