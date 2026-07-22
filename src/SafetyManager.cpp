#include "SafetyManager.h"
#include "RobotConfig.h"

SafetyManager::SafetyManager()
  : _gasCritical(false),
    _gasCaution(false),
    _flameCritical(false),
    _irObstacle(false),
    _tempCritical(false),
    _tempWarning(false),
    _ultrasonicCaution(false),
    _ultrasonicInvalid(true),
    _ultrasonicWarning(false),
    _nearObstacleHazard(false),
    _nearObstacleSource("NONE"),
    _lastCritical(false),
    _lastCaution(false),
    _lastNearObstacleHazard(false),
    _lastNearObstacleSource("NONE"),
    _lastReason("NONE"),
    _changed(true),
    _consecutiveInvalidDistance(0),
    _gasCriticalSince(0),
    _gasCriticalClearSince(0),
    _gasCautionSince(0),
    _gasCautionClearSince(0),
    _flameSince(0),
    _flameClearSince(0),
    _irSince(0),
    _irClearSince(0),
    _tempCriticalSince(0),
    _tempCriticalClearSince(0) {
}

void SafetyManager::begin() {
  _changed = true;
}

void SafetyManager::update(const SensorSnapshot& snapshot) {
  unsigned long now = millis();
  updateGas(snapshot, now);
  updateFlame(snapshot.flameDetected, now);
  updateIr(snapshot.irObstacleDetected, now);
  updateTemperature(snapshot.temperature, now);
  updateUltrasonic(snapshot);
  updateNearObstacle(snapshot);
  noteStateChange();
}

bool SafetyManager::isCritical() const {
  return _gasCritical || _flameCritical || _tempCritical;
}

bool SafetyManager::isCaution() const {
  return _gasCaution || _tempWarning || _ultrasonicWarning || _nearObstacleHazard;
}

bool SafetyManager::isIrObstacleActive() const {
  return _irObstacle;
}

bool SafetyManager::isNearObstacleHazardActive() const {
  return _nearObstacleHazard;
}

bool SafetyManager::isUltrasonicWarningActive() const {
  return _ultrasonicWarning;
}

bool SafetyManager::blocksMotion() const {
  return isCritical();
}

bool SafetyManager::blocksForwardMotion() const {
  return isCritical() || _nearObstacleHazard || _ultrasonicCaution || _ultrasonicWarning;
}

bool SafetyManager::stateChanged() {
  bool changed = _changed;
  _changed = false;
  return changed;
}

String SafetyManager::stateName() const {
  if (isCritical()) {
    return "CRITICAL";
  }
  if (isCaution()) {
    return "CAUTION";
  }
  return "SAFE";
}

String SafetyManager::alarmReason() const {
  if (_flameCritical) return "FLAME";
  if (_gasCritical) return "GAS_HIGH";
  if (_tempCritical) return "TEMP_HIGH";
  if (_nearObstacleHazard) return "NEAR_OBSTACLE";
  if (_irObstacle) return "IR_OBSTACLE";
  if (_ultrasonicWarning) return "ULTRASONIC_WARNING";
  if (_gasCaution) return "GAS_CAUTION";
  if (_tempWarning) return "TEMP_WARN";
  return "NONE";
}

String SafetyManager::nearObstacleSource() const {
  return _nearObstacleSource;
}

void SafetyManager::updateGas(const SensorSnapshot& snapshot, unsigned long now) {
  if (snapshot.mq2Warmup) {
    _gasCritical = false;
    _gasCaution = false;
    _gasCriticalSince = _gasCriticalClearSince = 0;
    _gasCautionSince = _gasCautionClearSince = 0;
    return;
  }

  if (!_gasCritical && snapshot.gasFiltered >= RobotConfig::GAS_HIGH_RISK_THRESHOLD) {
    if (_gasCriticalSince == 0) _gasCriticalSince = now;
    if (now - _gasCriticalSince >= RobotConfig::GAS_DEBOUNCE_MS) _gasCritical = true;
  } else if (_gasCritical && snapshot.gasFiltered <= RobotConfig::GAS_HIGH_RISK_CLEAR_THRESHOLD) {
    if (_gasCriticalClearSince == 0) _gasCriticalClearSince = now;
    if (now - _gasCriticalClearSince >= RobotConfig::GAS_CLEAR_DEBOUNCE_MS) _gasCritical = false;
  } else {
    _gasCriticalSince = 0;
    _gasCriticalClearSince = 0;
  }

  if (!_gasCaution && snapshot.gasFiltered >= RobotConfig::GAS_CAUTION_THRESHOLD) {
    if (_gasCautionSince == 0) _gasCautionSince = now;
    if (now - _gasCautionSince >= RobotConfig::GAS_DEBOUNCE_MS) _gasCaution = true;
  } else if (_gasCaution && snapshot.gasFiltered <= RobotConfig::GAS_CAUTION_CLEAR_THRESHOLD) {
    if (_gasCautionClearSince == 0) _gasCautionClearSince = now;
    if (now - _gasCautionClearSince >= RobotConfig::GAS_CLEAR_DEBOUNCE_MS) _gasCaution = false;
  } else {
    _gasCautionSince = 0;
    _gasCautionClearSince = 0;
  }
}

void SafetyManager::updateFlame(bool detected, unsigned long now) {
  if (detected) {
    _flameClearSince = 0;
    if (_flameSince == 0) _flameSince = now;
    if (now - _flameSince >= RobotConfig::FLAME_DEBOUNCE_MS) _flameCritical = true;
  } else {
    _flameSince = 0;
    if (_flameCritical) {
      if (_flameClearSince == 0) _flameClearSince = now;
      if (now - _flameClearSince >= RobotConfig::DIGITAL_CLEAR_DEBOUNCE_MS) _flameCritical = false;
    }
  }
}

void SafetyManager::updateIr(bool detected, unsigned long now) {
  if (detected) {
    _irClearSince = 0;
    if (_irSince == 0) _irSince = now;
    if (now - _irSince >= RobotConfig::IR_DEBOUNCE_MS) _irObstacle = true;
  } else {
    _irSince = 0;
    if (_irObstacle) {
      if (_irClearSince == 0) _irClearSince = now;
      if (now - _irClearSince >= RobotConfig::DIGITAL_CLEAR_DEBOUNCE_MS) _irObstacle = false;
    }
  }
}

void SafetyManager::updateTemperature(float temperature, unsigned long now) {
  if (isnan(temperature)) {
    _tempWarning = false;
    return;
  }

  _tempWarning = temperature >= RobotConfig::TEMP_WARNING_C;
  if (!_tempCritical && temperature >= RobotConfig::TEMP_CRITICAL_C) {
    if (_tempCriticalSince == 0) _tempCriticalSince = now;
    if (now - _tempCriticalSince >= RobotConfig::TEMP_DEBOUNCE_MS) _tempCritical = true;
  } else if (_tempCritical && temperature <= RobotConfig::TEMP_CRITICAL_CLEAR_C) {
    if (_tempCriticalClearSince == 0) _tempCriticalClearSince = now;
    if (now - _tempCriticalClearSince >= RobotConfig::TEMP_DEBOUNCE_MS) _tempCritical = false;
  } else {
    _tempCriticalSince = 0;
    _tempCriticalClearSince = 0;
  }
}

void SafetyManager::updateUltrasonic(const SensorSnapshot& snapshot) {
  _ultrasonicCaution = snapshot.distanceValid && snapshot.distanceCm <= RobotConfig::OBSTACLE_CM;
  _ultrasonicInvalid = !snapshot.distanceValid;
  if (snapshot.distanceValid) {
    _consecutiveInvalidDistance = 0;
    _ultrasonicWarning = false;
  } else {
    if (_consecutiveInvalidDistance < RobotConfig::ULTRASONIC_WARNING_INVALID_COUNT) {
      _consecutiveInvalidDistance++;
    }
    _ultrasonicWarning = _consecutiveInvalidDistance >= RobotConfig::ULTRASONIC_WARNING_INVALID_COUNT;
  }
}

void SafetyManager::updateNearObstacle(const SensorSnapshot& snapshot) {
  bool irNear = _irObstacle;
  bool ultrasonicNear = snapshot.distanceFacingFront && snapshot.distanceValid && snapshot.distanceCm <= RobotConfig::NEAR_OBSTACLE_CM;
  _nearObstacleHazard = irNear || ultrasonicNear;

  if (irNear && ultrasonicNear) {
    _nearObstacleSource = "BOTH";
  } else if (irNear) {
    _nearObstacleSource = "IR";
  } else if (ultrasonicNear) {
    _nearObstacleSource = "ULTRASONIC";
  } else {
    _nearObstacleSource = "NONE";
  }
}

void SafetyManager::noteStateChange() {
  bool critical = isCritical();
  bool caution = isCaution();
  String reason = alarmReason();
  if (critical != _lastCritical ||
      caution != _lastCaution ||
      _nearObstacleHazard != _lastNearObstacleHazard ||
      _nearObstacleSource != _lastNearObstacleSource ||
      reason != _lastReason) {
    _changed = true;
    _lastCritical = critical;
    _lastCaution = caution;
    _lastNearObstacleHazard = _nearObstacleHazard;
    _lastNearObstacleSource = _nearObstacleSource;
    _lastReason = reason;
  }
}
