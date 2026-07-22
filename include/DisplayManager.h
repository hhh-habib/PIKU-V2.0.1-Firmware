#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>

class DisplayManager {
public:
  DisplayManager();

  void begin();
  void updateDashboard(const String& servoPos,
                       float distance,
                       float temp,
                       float hum,
                       int gasValue,
                       int gasFilteredValue,
                       bool mq2Warmup,
                       bool flameDetected,
                       bool irObstacleDetected,
                       const String& gasStatus,
                       const String& safetyState,
                       const String& alarmReason,
                       bool nearObstacleHazard,
                       const String& nearObstacleSource,
                       const String& controlMode,
                       const String& navigationStatus);

private:
  Adafruit_ST7789 tft;
  bool tftFrameDrawn;
  unsigned long lastTftUpdateMs;
  String lastDisplayMode;
  String lastDisplayDistance;
  String lastDisplayTemp;
  String lastDisplayHum;
  String lastDisplayGas;
  String lastDisplaySafety;
  String lastDisplayHazard;
  String lastDisplayScan;
  String lastDisplayMove;
  uint8_t lastFaceState;

  String formatDistance(float distance) const;
  String formatTemperature(float temp) const;
  String formatHumidity(float hum) const;
  uint16_t gasStatusColor(const String& gasStatus) const;
  uint16_t safetyStatusColor(const String& safetyState) const;
  void drawHeader();
  void drawStaticLabel(int16_t x, int16_t y, const char* label);
  void drawDashboardFrame();
  void drawCriticalAlert(const String& alarmReason);
  void drawNearObstacleAlert(const String& nearObstacleSource);
  void drawValueField(int16_t x, int16_t y, int16_t w, int16_t h, const String& value, uint16_t color, uint8_t textSize);
  String shortenNavigationStatus(const String& navigationStatus) const;
  uint8_t getFaceState(const String& gasStatus, int gasValue, bool obstacle, bool critical) const;
  void drawRobotFace(uint8_t faceState);
};

#endif // DISPLAYMANAGER_H
