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
                       const String& gasStatus,
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
  String lastDisplayScan;
  String lastDisplayMove;
  uint8_t lastFaceState;

  String formatDistance(float distance) const;
  String formatTemperature(float temp) const;
  String formatHumidity(float hum) const;
  uint16_t gasStatusColor(const String& gasStatus) const;
  void drawHeader();
  void drawStaticLabel(int16_t x, int16_t y, const char* label);
  void drawDashboardFrame();
  void drawValueField(int16_t x, int16_t y, int16_t w, int16_t h, const String& value, uint16_t color, uint8_t textSize);
  String shortenNavigationStatus(const String& navigationStatus) const;
  uint8_t getFaceState(const String& gasStatus, int gasValue, bool obstacle) const;
  void drawRobotFace(uint8_t faceState);
};

#endif // DISPLAYMANAGER_H
