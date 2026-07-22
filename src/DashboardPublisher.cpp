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
                                               const SensorSnapshot& snapshot,
                                               const String& gasStatus,
                                               const String& safetyState,
                                               const String& alarmReason,
                                               bool nearObstacleHazard,
                                               const String& nearObstacleSource,
                                               bool ultrasonicWarning,
                                               const String& alarmSoundState,
                                               const String& buzzerState,
                                               const String& controlMode,
                                               const String& turnMode,
                                               const String& motorStateName,
                                               const String& manualMotorDisplayState,
                                               const String& navigationStatus) {
  display.updateDashboard(servoPos,
                          snapshot.distanceCm,
                          snapshot.temperature,
                          snapshot.humidity,
                          snapshot.gasRaw,
                          snapshot.gasFiltered,
                          snapshot.mq2Warmup,
                          snapshot.flameDetected,
                          snapshot.irObstacleDetected,
                          gasStatus,
                          safetyState,
                          alarmReason,
                          nearObstacleHazard,
                          nearObstacleSource,
                          controlMode,
                          navigationStatus);

  data.temperature = snapshot.temperature;
  data.humidity = snapshot.humidity;
  data.gasValue = snapshot.gasRaw;
  data.gasFilteredValue = snapshot.gasFiltered;
  data.mq2Warmup = snapshot.mq2Warmup;
  data.gasStatus = gasStatus;
  data.frontDistance = snapshot.distanceCm;
  data.distanceValid = snapshot.distanceValid;
  data.flameDetected = snapshot.flameDetected;
  data.irObstacleDetected = snapshot.irObstacleDetected;
  data.nearObstacleHazard = nearObstacleHazard;
  data.nearObstacleSource = nearObstacleSource;
  data.ultrasonicWarning = ultrasonicWarning;
  data.safetyState = safetyState;
  data.alarmReason = alarmReason;
  data.alarmSoundState = alarmSoundState;
  data.buzzerState = buzzerState;
  data.controlMode = controlMode;
  data.turnMode = turnMode;
  if (controlMode == "MANUAL" && manualMotorDisplayState.length() > 0) {
    data.motorState = manualMotorDisplayState;
  } else {
    data.motorState = motorStateName;
  }

  publishIfChanged();
  logSensorReadings(servoPos, snapshot, motorStateName);
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
         abs(current.gasFilteredValue - previous.gasFilteredValue) >= 5 ||
         current.distanceValid != previous.distanceValid ||
         current.mq2Warmup != previous.mq2Warmup ||
         current.flameDetected != previous.flameDetected ||
         current.irObstacleDetected != previous.irObstacleDetected ||
         current.nearObstacleHazard != previous.nearObstacleHazard ||
         current.nearObstacleSource != previous.nearObstacleSource ||
         current.ultrasonicWarning != previous.ultrasonicWarning ||
         current.gasStatus != previous.gasStatus ||
         current.safetyState != previous.safetyState ||
         current.alarmReason != previous.alarmReason ||
         current.alarmSoundState != previous.alarmSoundState ||
         current.buzzerState != previous.buzzerState ||
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

void DashboardPublisher::logSensorReadings(const String& servoPos, const SensorSnapshot& snapshot, const String& motorStateName) {
  unsigned long now = millis();
  if (lastSerialLogMs != 0 && now - lastSerialLogMs < RobotConfig::SERIAL_LOG_INTERVAL_MS) {
    return;
  }
  lastSerialLogMs = now;
  Serial.println("----------------------");
  Serial.print("Servo   : "); Serial.println(servoPos);
  Serial.print("Distance: "); Serial.println(snapshot.distanceCm);
  Serial.print("Temp    : "); Serial.println(snapshot.temperature);
  Serial.print("Humidity: "); Serial.println(snapshot.humidity);
  Serial.print("Gas Raw : "); Serial.println(snapshot.gasRaw);
  Serial.print("Gas Avg : "); Serial.println(snapshot.gasFiltered);
  Serial.print("MQ2 Warm: "); Serial.println(snapshot.mq2Warmup ? "YES" : "NO");
  Serial.print("Flame   : "); Serial.println(snapshot.flameDetected ? "DETECTED" : "CLEAR");
  Serial.print("IR Obs  : "); Serial.println(snapshot.irObstacleDetected ? "DETECTED" : "CLEAR");
  Serial.print("Motor   : "); Serial.println(motorStateName);
}
