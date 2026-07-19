#include "DashboardPublisher.h"
#include "RobotConfig.h"

DashboardPublisher::DashboardPublisher(WebDashboard& webDashboard, DisplayManager& displayManager, DashboardData& dashboardData)
  : dashboard(webDashboard),
    display(displayManager),
    data(dashboardData),
    hasPublishedData(false),
    lastSerialLogMs(0) {
}

void DashboardPublisher::syncState(const String& controlMode,
                                   const String& turnMode,
                                   const String& motorStateName,
                                   const String& manualMotorDisplayState) {
  data.controlMode = controlMode;
  data.turnMode = turnMode;
  if (controlMode == "MANUAL" && manualMotorDisplayState.length() > 0) {
    data.motorState = manualMotorDisplayState;
  } else {
    data.motorState = motorStateName;
  }

  dashboard.setData(data);
  cachePublishedDashboardData();
}

void DashboardPublisher::publishSensorReadings(const String& servoPos,
                                               float distance,
                                               float temp,
                                               float hum,
                                               int gasValue,
                                               const String& gasStatus,
                                               const String& controlMode,
                                               const String& turnMode,
                                               const String& motorStateName,
                                               const String& manualMotorDisplayState,
                                               const String& navigationStatus) {
  display.updateDashboard(servoPos, distance, temp, hum, gasValue, gasStatus, controlMode, navigationStatus);

  //digitalWrite(PinConfig::BUZZER_PIN, LOW);

  data.temperature = temp;
  data.humidity = hum;
  data.gasValue = gasValue;
  data.gasStatus = gasStatus;
  data.frontDistance = distance;
  data.controlMode = controlMode;
  data.turnMode = turnMode;
  if (controlMode == "MANUAL" && manualMotorDisplayState.length() > 0) {
    data.motorState = manualMotorDisplayState;
  } else {
    data.motorState = motorStateName;
  }

  publishIfChanged();
  logSensorReadings(servoPos, distance, temp, hum, gasValue, motorStateName);
}

bool DashboardPublisher::floatChanged(float current, float previous, float threshold) const {
  if (isnan(current) && isnan(previous)) {
    return false;
  }
  if (isnan(current) || isnan(previous)) {
    return true;
  }
  return fabs(current - previous) >= threshold;
}

bool DashboardPublisher::dashboardDataChanged(const DashboardData& current, const DashboardData& previous) const {
  return floatChanged(current.temperature, previous.temperature, 0.1f) ||
         floatChanged(current.humidity, previous.humidity, 0.1f) ||
         floatChanged(current.frontDistance, previous.frontDistance, 0.5f) ||
         abs(current.gasValue - previous.gasValue) >= 5 ||
         current.gasStatus != previous.gasStatus ||
         current.motorState != previous.motorState ||
         current.navigationDecision != previous.navigationDecision ||
         current.controlMode != previous.controlMode ||
         current.turnMode != previous.turnMode;
}

void DashboardPublisher::cachePublishedDashboardData() {
  lastPublishedData = data;
  hasPublishedData = true;
}

void DashboardPublisher::publishIfChanged() {
  if (!hasPublishedData || dashboardDataChanged(data, lastPublishedData)) {
    dashboard.setData(data);
    cachePublishedDashboardData();
  }
}

void DashboardPublisher::logSensorReadings(const String& servoPos, float distance, float temp, float hum, int gasValue, const String& motorStateName) {
  unsigned long now = millis();
  if (lastSerialLogMs != 0 && now - lastSerialLogMs < RobotConfig::SERIAL_LOG_INTERVAL_MS) {
    return;
  }
  lastSerialLogMs = now;
  Serial.println("----------------------");
  Serial.print("Servo   : "); Serial.println(servoPos);
  Serial.print("Distance: "); Serial.println(distance);
  Serial.print("Temp    : "); Serial.println(temp);
  Serial.print("Humidity: "); Serial.println(hum);
  Serial.print("Gas Raw : "); Serial.println(gasValue);
  Serial.print("Motor   : "); Serial.println(motorStateName);
}
