#ifndef BUZZERMANAGER_H
#define BUZZERMANAGER_H

#include <Arduino.h>

enum class BuzzerMode {
  Off,
  Caution,
  Hazard,
  Critical
};

class BuzzerManager {
public:
  explicit BuzzerManager(int pin);

  void begin();
  void setMode(BuzzerMode mode);
  void setEnabled(bool enabled);
  void setMuted(bool muted);
  void requestTest(unsigned long durationMs = 350);
  void update();
  const char* modeName() const;
  const char* soundStateName() const;
  bool isEnabled() const;
  bool isMuted() const;
  bool isTesting() const;

private:
  int _pin;
  BuzzerMode _mode;
  bool _enabled;
  bool _muted;
  bool _pinHigh;
  bool _toneActive;
  unsigned long _patternStartMs;
  unsigned long _lastToggleUs;
  unsigned long _testUntilMs;

  void setOutput(bool high);
  void updateTone(unsigned int frequencyHz);
  BuzzerMode effectiveMode(unsigned long nowMs) const;
  bool patternOnWindow(unsigned long nowMs) const;
};

#endif // BUZZERMANAGER_H
