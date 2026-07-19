#include "DisplayManager.h"
#include <SPI.h>
#include "PinConfig.h"
#include "RobotConfig.h"

namespace {
  const uint16_t COLOR_BG = ST77XX_BLACK;
  const uint16_t COLOR_HEADER = ST77XX_BLUE;
  const uint16_t COLOR_LABEL = ST77XX_CYAN;
  const uint16_t COLOR_VALUE = ST77XX_WHITE;
  const uint16_t COLOR_SAFE = ST77XX_GREEN;
  const uint16_t COLOR_CAUTION = ST77XX_YELLOW;
  const uint16_t COLOR_RISK = ST77XX_RED;
}

DisplayManager::DisplayManager()
  : tft(PinConfig::TFT_CS, PinConfig::TFT_DC, PinConfig::TFT_RST),
    tftFrameDrawn(false),
    lastTftUpdateMs(0),
    lastDisplayMode(""),
    lastDisplayDistance(""),
    lastDisplayTemp(""),
    lastDisplayHum(""),
    lastDisplayGas(""),
    lastDisplayScan(""),
    lastDisplayMove(""),
    lastFaceState(255) {
}

void DisplayManager::begin() {
  SPI.begin(PinConfig::TFT_SCK, PinConfig::TFT_MISO, PinConfig::TFT_MOSI, PinConfig::TFT_SS);
  tft.init(RobotConfig::TFT_WIDTH, RobotConfig::TFT_HEIGHT, SPI_MODE3);
  tft.setRotation(2);
  tft.fillScreen(COLOR_BG);

  tft.setTextColor(COLOR_SAFE);
  tft.setTextSize(2);
  tft.setCursor(45, 108);
  tft.print("PIKU V2 READY");
}

void DisplayManager::updateDashboard(const String& servoPos,
                                     float distance,
                                     float temp,
                                     float hum,
                                     int gasValue,
                                     const String& gasStatus,
                                     const String& controlMode,
                                     const String& navigationStatus) {
  unsigned long now = millis();
  if (tftFrameDrawn && now - lastTftUpdateMs < RobotConfig::TFT_UPDATE_INTERVAL_MS) {
    return;
  }

  if (!tftFrameDrawn) {
    drawDashboardFrame();
  }
  lastTftUpdateMs = now;

  bool obstacle = distance > 0 && distance < RobotConfig::OBSTACLE_CM;
  String distanceText = formatDistance(distance);
  String tempText = formatTemperature(temp);
  String humText = formatHumidity(hum);
  String moveText = shortenNavigationStatus(navigationStatus);
  uint8_t faceState = getFaceState(gasStatus, gasValue, obstacle);

  if (controlMode != lastDisplayMode) {
    drawValueField(86, 44, 132, 16, controlMode, COLOR_VALUE, 2);
    lastDisplayMode = controlMode;
  }
  if (distanceText != lastDisplayDistance) {
    drawValueField(86, 62, 132, 16, distanceText, obstacle ? COLOR_RISK : COLOR_VALUE, 2);
    lastDisplayDistance = distanceText;
  }
  if (tempText != lastDisplayTemp) {
    drawValueField(86, 80, 132, 16, tempText, COLOR_VALUE, 2);
    lastDisplayTemp = tempText;
  }
  if (humText != lastDisplayHum) {
    drawValueField(86, 98, 132, 16, humText, COLOR_VALUE, 2);
    lastDisplayHum = humText;
  }
  if (gasStatus != lastDisplayGas) {
    drawValueField(86, 116, 132, 16, gasStatus, gasStatusColor(gasStatus), 2);
    lastDisplayGas = gasStatus;
  }
  if (servoPos != lastDisplayScan) {
    drawValueField(76, 158, 48, 16, servoPos, COLOR_VALUE, 1);
    lastDisplayScan = servoPos;
  }
  if (moveText != lastDisplayMove) {
    drawValueField(184, 158, 42, 16, moveText, COLOR_VALUE, 1);
    lastDisplayMove = moveText;
  }
  if (faceState != lastFaceState) {
    drawRobotFace(faceState);
    lastFaceState = faceState;
  }
}

String DisplayManager::formatDistance(float distance) const {
  if (distance < 0) {
    return "-- cm";
  }
  return String(distance, 0) + " cm";
}

String DisplayManager::formatTemperature(float temp) const {
  if (isnan(temp)) {
    return "--.- C";
  }
  return String(temp, 1) + " C";
}

String DisplayManager::formatHumidity(float hum) const {
  if (isnan(hum)) {
    return "-- %";
  }
  return String(hum, 0) + " %";
}

uint16_t DisplayManager::gasStatusColor(const String& gasStatus) const {
  if (gasStatus == "HIGH RISK") {
    return COLOR_RISK;
  }
  if (gasStatus == "CAUTION") {
    return COLOR_CAUTION;
  }
  return COLOR_SAFE;
}

void DisplayManager::drawHeader() {
  tft.fillRect(0, 0, 240, 30, COLOR_HEADER);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_VALUE, COLOR_HEADER);
  tft.setCursor(74, 8);
  tft.print("PIKU V2");
}

void DisplayManager::drawStaticLabel(int16_t x, int16_t y, const char* label) {
  tft.setTextSize(2);
  tft.setTextColor(COLOR_LABEL, COLOR_BG);
  tft.setCursor(x, y);
  tft.print(label);
}

void DisplayManager::drawDashboardFrame() {
  tft.fillScreen(COLOR_BG);
  drawHeader();

  tft.drawRoundRect(6, 34, 228, 110, 5, COLOR_LABEL);
  drawStaticLabel(16, 44, "MODE");
  drawStaticLabel(16, 62, "DIST");
  drawStaticLabel(16, 80, "TEMP");
  drawStaticLabel(16, 98, "HUM");
  drawStaticLabel(16, 116, "GAS");

  tft.drawRoundRect(6, 148, 228, 34, 5, COLOR_CAUTION);
  drawStaticLabel(16, 158, "SCAN");
  drawStaticLabel(126, 158, "MOVE");

  tftFrameDrawn = true;
  lastDisplayMode = "";
  lastDisplayDistance = "";
  lastDisplayTemp = "";
  lastDisplayHum = "";
  lastDisplayGas = "";
  lastDisplayScan = "";
  lastDisplayMove = "";
  lastFaceState = 255;
}

void DisplayManager::drawValueField(int16_t x, int16_t y, int16_t w, int16_t h, const String& value, uint16_t color, uint8_t textSize) {
  tft.fillRect(x, y, w, h, COLOR_BG);
  tft.setTextSize(textSize);
  tft.setTextColor(color, COLOR_BG);
  tft.setCursor(x, y);
  tft.print(value);
}

String DisplayManager::shortenNavigationStatus(const String& navigationStatus) const {
  if (navigationStatus == "MANUAL_CONTROL") return "MANUAL";
  if (navigationStatus == "MANUAL_FORWARD") return "FWD";
  if (navigationStatus == "MANUAL_BACKWARD") return "BACK";
  if (navigationStatus == "MANUAL_LEFT") return "LEFT";
  if (navigationStatus == "MANUAL_RIGHT") return "RIGHT";
  if (navigationStatus == "MANUAL_STOP") return "STOP";
  if (navigationStatus == "MANUAL_OBSTACLE_STOP") return "OBS";
  if (navigationStatus == "FORWARD") return "FWD";
  if (navigationStatus == "BACKWARD") return "BACK";
  if (navigationStatus == "BACKWARD_RECOVERY") return "BACK";
  if (navigationStatus == "LEFT") return "LEFT";
  if (navigationStatus == "RIGHT") return "RIGHT";
  if (navigationStatus == "STOP") return "STOP";
  if (navigationStatus == "IDLE") return "IDLE";
  return navigationStatus.substring(0, 6);
}

uint8_t DisplayManager::getFaceState(const String& gasStatus, int gasValue, bool obstacle) const {
  if (gasStatus == "HIGH RISK" || gasValue >= RobotConfig::GAS_HIGH_RISK_THRESHOLD) {
    return 3;
  }
  if (gasStatus == "CAUTION" || gasValue >= RobotConfig::GAS_CAUTION_THRESHOLD) {
    return 2;
  }
  if (obstacle) {
    return 1;
  }
  return 0;
}

void DisplayManager::drawRobotFace(uint8_t faceState) {
  const int16_t cx = 120;
  const int16_t top = 188;
  bool highRiskGas = faceState == 3;
  bool cautionGas = faceState == 2;
  bool obstacle = faceState == 1;
  uint16_t faceColor = highRiskGas ? COLOR_RISK : (cautionGas ? COLOR_CAUTION : (obstacle ? COLOR_CAUTION : COLOR_LABEL));
  uint16_t mouthColor = highRiskGas ? COLOR_RISK : (cautionGas ? COLOR_CAUTION : (obstacle ? COLOR_VALUE : COLOR_SAFE));

  tft.fillRect(64, 184, 112, 56, COLOR_BG);

  tft.drawLine(cx, top - 10, cx, top - 2, faceColor);
  tft.fillCircle(cx, top - 13, 3, faceColor);

  tft.fillRoundRect(cx - 42, top, 84, 48, 8, faceColor);
  tft.fillRoundRect(cx - 37, top + 5, 74, 38, 6, COLOR_BG);
  tft.drawRoundRect(cx - 42, top, 84, 48, 8, COLOR_VALUE);

  tft.fillCircle(cx - 20, top + 19, 7, COLOR_VALUE);
  tft.fillCircle(cx + 20, top + 19, 7, COLOR_VALUE);
  tft.fillCircle(cx - 20, top + 19, 3, highRiskGas ? COLOR_RISK : COLOR_HEADER);
  tft.fillCircle(cx + 20, top + 19, 3, highRiskGas ? COLOR_RISK : COLOR_HEADER);

  if (highRiskGas) {
    tft.drawCircle(cx, top + 35, 6, mouthColor);
  } else if (cautionGas) {
    tft.drawLine(cx - 16, top + 37, cx - 6, top + 33, mouthColor);
    tft.drawLine(cx - 6, top + 33, cx + 6, top + 33, mouthColor);
    tft.drawLine(cx + 6, top + 33, cx + 16, top + 37, mouthColor);
  } else if (obstacle) {
    tft.drawLine(cx - 16, top + 36, cx + 16, top + 36, mouthColor);
  } else {
    tft.drawLine(cx - 17, top + 34, cx - 6, top + 39, mouthColor);
    tft.drawLine(cx - 6, top + 39, cx + 6, top + 39, mouthColor);
    tft.drawLine(cx + 6, top + 39, cx + 17, top + 34, mouthColor);
  }

  tft.fillRect(cx - 49, top + 17, 6, 16, faceColor);
  tft.fillRect(cx + 43, top + 17, 6, 16, faceColor);
}
