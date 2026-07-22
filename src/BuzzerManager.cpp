#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(int pin)
  : _pin(pin),
    _mode(BuzzerMode::Off),
    _enabled(true),
    _muted(false),
    _pinHigh(false),
    _toneActive(false),
    _patternStartMs(0),
    _lastToggleUs(0),
    _testUntilMs(0) {
}

void BuzzerManager::begin() {
  pinMode(_pin, OUTPUT);
  setOutput(false);
}

void BuzzerManager::setMode(BuzzerMode mode) {
  if (_mode == mode) {
    return;
  }

  _mode = mode;
  _patternStartMs = millis();
  _lastToggleUs = micros();
  _toneActive = false;
  setOutput(false);
}

void BuzzerManager::setEnabled(bool enabled) {
  if (_enabled == enabled) {
    return;
  }
  _enabled = enabled;
  if (_enabled) {
    _muted = false;
  }
  if (!_enabled) {
    _muted = true;
    _testUntilMs = 0;
    setOutput(false);
  }
}

void BuzzerManager::setMuted(bool muted) {
  if (_muted == muted) {
    return;
  }
  _muted = muted;
  if (_muted) {
    _testUntilMs = 0;
    setOutput(false);
  }
}

void BuzzerManager::requestTest(unsigned long durationMs) {
  _testUntilMs = millis() + durationMs;
  _lastToggleUs = micros();
  _toneActive = false;
  setOutput(false);
}

void BuzzerManager::update() {
  unsigned long nowMs = millis();
  BuzzerMode activeMode = effectiveMode(nowMs);
  if (activeMode == BuzzerMode::Off || !patternOnWindow(nowMs)) {
    if (_toneActive || _pinHigh) {
      _toneActive = false;
      setOutput(false);
    }
    return;
  }

  _toneActive = true;
  updateTone(activeMode == BuzzerMode::Critical ? 2200 : (activeMode == BuzzerMode::Hazard ? 2000 : 1600));
}

const char* BuzzerManager::modeName() const {
  switch (_mode) {
    case BuzzerMode::Critical:
      return "CRITICAL";
    case BuzzerMode::Hazard:
      return "HAZARD";
    case BuzzerMode::Caution:
      return "CAUTION";
    case BuzzerMode::Off:
    default:
      return "OFF";
  }
}

const char* BuzzerManager::soundStateName() const {
  if (isTesting()) {
    return "TEST";
  }
  if (_muted || !_enabled) {
    return "MUTED";
  }
  return "ENABLED";
}

bool BuzzerManager::isEnabled() const {
  return _enabled;
}

bool BuzzerManager::isMuted() const {
  return _muted || !_enabled;
}

bool BuzzerManager::isTesting() const {
  return _testUntilMs != 0 && millis() < _testUntilMs;
}

void BuzzerManager::setOutput(bool high) {
  _pinHigh = high;
  digitalWrite(_pin, high ? HIGH : LOW);
}

void BuzzerManager::updateTone(unsigned int frequencyHz) {
  unsigned long halfPeriodUs = 500000UL / frequencyHz;
  unsigned long nowUs = micros();
  if (nowUs - _lastToggleUs >= halfPeriodUs) {
    _lastToggleUs = nowUs;
    setOutput(!_pinHigh);
  }
}

BuzzerMode BuzzerManager::effectiveMode(unsigned long nowMs) const {
  if (_testUntilMs != 0 && nowMs < _testUntilMs) {
    return BuzzerMode::Caution;
  }
  if (!_enabled || _muted) {
    return BuzzerMode::Off;
  }
  return _mode;
}

bool BuzzerManager::patternOnWindow(unsigned long nowMs) const {
  if (isTesting()) {
    return true;
  }
  unsigned long phaseMs = nowMs - _patternStartMs;
  if (_mode == BuzzerMode::Critical) {
    return (phaseMs % 260UL) < 180UL;
  }
  if (_mode == BuzzerMode::Hazard) {
    return (phaseMs % 320UL) < 220UL;
  }
  if (_mode == BuzzerMode::Caution) {
    return (phaseMs % 1200UL) < 120UL;
  }
  return false;
}
