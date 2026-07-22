#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <DHT.h>

namespace RobotConfig {
  static constexpr int TFT_WIDTH = 240;
  static constexpr int TFT_HEIGHT = 240;

  static constexpr int DHT_TYPE = DHT22;

  static constexpr float OBSTACLE_CM = 25;
  static constexpr float NEAR_OBSTACLE_CM = 7.0f;
  static constexpr float ULTRASONIC_MAX_CM = 350;
  static constexpr int GAS_THRESHOLD = 2000;
  // MQ-2 thresholds are raw ADC values after the GPIO35 voltage divider and require calibration.
  static constexpr int GAS_CAUTION_THRESHOLD = 1200;
  static constexpr int GAS_HIGH_RISK_THRESHOLD = 1800;
  static constexpr int GAS_CAUTION_CLEAR_THRESHOLD = 1050;
  static constexpr int GAS_HIGH_RISK_CLEAR_THRESHOLD = 1600;

  static constexpr float TEMP_WARNING_C = 45.0f;
  static constexpr float TEMP_CRITICAL_C = 55.0f;
  static constexpr float TEMP_CRITICAL_CLEAR_C = 50.0f;

  static constexpr int IR_ACTIVE_LEVEL = LOW;
  static constexpr int FLAME_ACTIVE_LEVEL = LOW;

  static constexpr int CENTER_SCAN = 90;
  static constexpr int LEFT_SCAN = 160;
  static constexpr int RIGHT_SCAN = 20;

  static constexpr unsigned long TFT_UPDATE_INTERVAL_MS = 500;
  static constexpr unsigned long MANUAL_SENSOR_INTERVAL_MS = 500;
  static constexpr unsigned long SERIAL_LOG_INTERVAL_MS = 1000;
  static constexpr unsigned long FAST_SENSOR_INTERVAL_MS = 50;
  static constexpr unsigned long ULTRASONIC_INTERVAL_MS = 150;
  static constexpr unsigned long DHT_INTERVAL_MS = 2200;
  static constexpr unsigned long DASHBOARD_PUBLISH_INTERVAL_MS = 250;
  static constexpr unsigned long MQ2_WARMUP_MS = 30000;
  static constexpr unsigned long IR_RECOVERY_SETTLE_MS = 1000;
  static constexpr unsigned long IR_RECOVERY_REVERSE_MS = 220;
  static constexpr unsigned long IR_RECOVERY_COOLDOWN_MS = 1500;
  static constexpr unsigned long OBSTACLE_WARNING_MS = 1800;
  static constexpr int ULTRASONIC_WARNING_INVALID_COUNT = 3;
  static constexpr unsigned long GAS_DEBOUNCE_MS = 600;
  static constexpr unsigned long GAS_CLEAR_DEBOUNCE_MS = 2000;
  static constexpr unsigned long FLAME_DEBOUNCE_MS = 80;
  static constexpr unsigned long IR_DEBOUNCE_MS = 80;
  static constexpr unsigned long TEMP_DEBOUNCE_MS = 1000;
  static constexpr unsigned long DIGITAL_CLEAR_DEBOUNCE_MS = 200;
  static constexpr int MQ2_FILTER_SAMPLES = 8;
}

#endif // ROBOTCONFIG_H
