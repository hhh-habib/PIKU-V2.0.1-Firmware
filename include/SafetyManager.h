#ifndef SAFETYMANAGER_H
#define SAFETYMANAGER_H

#include <Arduino.h>
#include "SensorManager.h"

class SafetyManager {
public:
  SafetyManager();

  void begin();
  void update(const SensorSnapshot& snapshot);
  bool isCritical() const;
  bool isCaution() const;
  bool isIrObstacleActive() const;
  bool isNearObstacleHazardActive() const;
  bool isUltrasonicWarningActive() const;
  bool blocksMotion() const;
  bool blocksForwardMotion() const;
  bool stateChanged();
  String stateName() const;
  String alarmReason() const;
  String nearObstacleSource() const;

private:
  bool _gasCritical;
  bool _gasCaution;
  bool _flameCritical;
  bool _irObstacle;
  bool _tempCritical;
  bool _tempWarning;
  bool _ultrasonicCaution;
  bool _ultrasonicInvalid;
  bool _ultrasonicWarning;
  bool _nearObstacleHazard;
  String _nearObstacleSource;
  bool _lastCritical;
  bool _lastCaution;
  bool _lastNearObstacleHazard;
  String _lastNearObstacleSource;
  String _lastReason;
  bool _changed;
  int _consecutiveInvalidDistance;

  unsigned long _gasCriticalSince;
  unsigned long _gasCriticalClearSince;
  unsigned long _gasCautionSince;
  unsigned long _gasCautionClearSince;
  unsigned long _flameSince;
  unsigned long _flameClearSince;
  unsigned long _irSince;
  unsigned long _irClearSince;
  unsigned long _tempCriticalSince;
  unsigned long _tempCriticalClearSince;

  void updateGas(const SensorSnapshot& snapshot, unsigned long now);
  void updateFlame(bool detected, unsigned long now);
  void updateIr(bool detected, unsigned long now);
  void updateTemperature(float temperature, unsigned long now);
  void updateUltrasonic(const SensorSnapshot& snapshot);
  void updateNearObstacle(const SensorSnapshot& snapshot);
  void noteStateChange();
};

#endif // SAFETYMANAGER_H
