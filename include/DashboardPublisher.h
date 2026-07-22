#ifndef DASHBOARDPUBLISHER_H
#define DASHBOARDPUBLISHER_H

#include <Arduino.h>
#include "DisplayManager.h"
#include "SensorManager.h"
#include "WebDashboard.h"

class DashboardPublisher {
public:
  DashboardPublisher(WebDashboard& webDashboard, DisplayManager& displayManager, DashboardData& dashboardData);

  void syncState(const String& controlMode,
                 const String& turnMode,
                 const String& motorStateName,
                 const String& manualMotorDisplayState);

  void publishSensorReadings(const String& servoPos,
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
                             const String& navigationStatus);

private:
  WebDashboard& dashboard;
  DisplayManager& display;
  DashboardData& data;
  DashboardData lastPublishedData;
  bool hasPublishedData;
  unsigned long lastSerialLogMs;

  bool floatChanged(float current, float previous, float threshold) const;
  bool dashboardDataChanged(const DashboardData& current, const DashboardData& previous) const;
  void cachePublishedDashboardData();
  void publishIfChanged();
  void logSensorReadings(const String& servoPos, const SensorSnapshot& snapshot, const String& motorStateName);
};

#endif // DASHBOARDPUBLISHER_H
