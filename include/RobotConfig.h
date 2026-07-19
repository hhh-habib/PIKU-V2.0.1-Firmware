#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include <DHT.h>

namespace RobotConfig {
  static constexpr int TFT_WIDTH = 240;
  static constexpr int TFT_HEIGHT = 240;

  static constexpr int DHT_TYPE = DHT22;

  static constexpr float OBSTACLE_CM = 25;
  static constexpr int GAS_THRESHOLD = 2000;
  static constexpr int GAS_CAUTION_THRESHOLD = 1200;
  static constexpr int GAS_HIGH_RISK_THRESHOLD = 1800;

  static constexpr int CENTER_SCAN = 90;
  static constexpr int LEFT_SCAN = 160;
  static constexpr int RIGHT_SCAN = 20;

  static constexpr unsigned long TFT_UPDATE_INTERVAL_MS = 500;
  static constexpr unsigned long MANUAL_SENSOR_INTERVAL_MS = 500;
  static constexpr unsigned long SERIAL_LOG_INTERVAL_MS = 1000;
}

#endif // ROBOTCONFIG_H
