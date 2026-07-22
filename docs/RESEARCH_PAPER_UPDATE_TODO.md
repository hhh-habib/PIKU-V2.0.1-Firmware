# Research Paper Update TODO

The research paper and academic manuscript materials should be updated after the V2.0.1 firmware behavior is finalized and the circuit diagram is revised. Do not publish manuscript claims that are not supported by firmware behavior, circuit documentation, or hardware validation.

## Required Updates

- Rename the firmware/software version in the paper to PIKU V2.0.1 where this upgrade is discussed.
- Describe V2.0.1 as an enhanced PIKU 2.0 firmware release, not as PIKU 2.1.
- Add the flame sensor as a critical safety input.
- Add the IR sensor as a close-range backup obstacle sensor, not a permanent critical hazard.
- Explain the AUTO near-obstacle recovery sequence: stop, warn, reverse, servo/HC-SR04 scan, choose clearer side, turn, continue.
- Explain MANUAL forward blocking with backward/left/right escape commands.
- Add GPIO33 physical buzzer alarm support.
- Explain dashboard alarm enable, mute, and test controls.
- Clarify that muting sound does not clear safety state or hazard display.
- Update MQ-2 discussion to mention the GPIO35 voltage divider and the need for threshold calibration.
- Preserve the role of HC-SR04 as the primary navigation sensor.
- Note that GPIO25 and GPIO14 are reserved for future motor PWM.

## Figures and Tables To Update

- Circuit diagram after the hardware drawing is revised.
- GPIO/pin map table.
- Software architecture block diagram if it currently omits SafetyManager or BuzzerManager.
- Safety state table.
- Autonomous navigation flowchart.
- Web dashboard screenshot or description.

## Validation Notes To Avoid Overclaiming

- Do not claim new hardware validation unless the robot has actually been tested with the final V2.0.1 firmware build.
- Separate firmware build verification from physical robot testing.
- Keep MQ-2 threshold claims tentative until calibration is completed.
