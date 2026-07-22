# Changelog

## v2.0.1 - Enhanced Inspection, Safety and Obstacle Recovery Upgrade

### Added

- Flame sensor support on GPIO36 as a critical safety input.
- IR obstacle sensor support on GPIO39 as a close-range backup obstacle input.
- SafetyManager classification for critical hazards, caution states, near-obstacle hazards, forward-only blocking, and alarm reasons.
- BuzzerManager for GPIO33 physical buzzer output with non-blocking caution, hazard, critical, and test patterns.
- Dashboard alarm endpoints for enabling, muting, and testing alarm sound.
- Dashboard telemetry for safety state, alarm reason, alarm sound state, buzzer state, flame state, IR state, near-obstacle state, and ultrasonic warning state.
- AUTO near-obstacle recovery that stops, warns, reverses, scans left/right using the existing servo-mounted HC-SR04, turns toward the clearer path, and resumes autonomous movement.
- MANUAL forward blocking for near-obstacle hazards while preserving backward/left/right escape commands.

### Changed

- Critical safety stop is limited to flame, MQ-2 high-risk gas, and critical high temperature.
- IR obstacle detection no longer contributes to permanent critical safety stop.
- MQ-2 telemetry now documents that GPIO35 receives the divided analog voltage and still requires calibration.
- TFT warning views now distinguish critical hazards from near-obstacle recovery warnings.

### Preserved

- Existing motor direction pins and timing behavior.
- GPIO25 and GPIO14 reserved for future ENA/ENB PWM.
- Existing servo scan angles: center 90, left 160, right 20.
- HC-SR04 as the primary navigation and path-selection sensor.
- Existing MQ-2 threshold values pending calibration.

### Documentation Notes

- Circuit diagram update is still pending.
- Research paper update tasks are tracked separately in `docs/RESEARCH_PAPER_UPDATE_TODO.md`.
