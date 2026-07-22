#ifndef PINCONFIG_H
#define PINCONFIG_H

namespace PinConfig {
  // TFT
  static constexpr int TFT_CS = 5;
  static constexpr int TFT_DC = 22;
  static constexpr int TFT_RST = 4;
  static constexpr int TFT_SCK = 18;
  static constexpr int TFT_MISO = -1;
  static constexpr int TFT_MOSI = 23;
  static constexpr int TFT_SS = -1;

  // Motors: ENA/ENB jumper caps ON
  static constexpr int RIGHT_IN1 = 26;
  static constexpr int RIGHT_IN2 = 27;
  static constexpr int LEFT_IN1 = 16;
  static constexpr int LEFT_IN2 = 17;

  // Sensors
  static constexpr int SERVO_PIN = 13;
  static constexpr int TRIG_PIN = 32;
  static constexpr int ECHO_PIN = 34;
  static constexpr int DHT_PIN = 21;
  static constexpr int MQ2_PIN = 35;
  static constexpr int BUZZER_PIN = 33;
  static constexpr int FLAME_PIN = 36;
  static constexpr int IR_OBSTACLE_PIN = 39;

  // Reserved for future L298N ENA/ENB PWM. Do not use for sensors.
  static constexpr int RIGHT_ENABLE_PWM_RESERVED = 25;
  static constexpr int LEFT_ENABLE_PWM_RESERVED = 14;
}

#endif // PINCONFIG_H
