#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "RobotConfig.h"

struct SensorSnapshot {
  float distanceCm = -1.0f;
  bool distanceValid = false;
  bool distanceFacingFront = true;
  float temperature = NAN;
  bool temperatureValid = false;
  float humidity = NAN;
  bool humidityValid = false;
  int gasRaw = 0;
  int gasFiltered = 0;
  bool mq2Warmup = true;
  bool irObstacleDetected = false;
  bool flameDetected = false;
  unsigned long updatedMs = 0;
};

class SensorManager {
public:
  SensorManager();

  void begin();
  void update();
  void setDistanceFacingFront(bool facingFront);
  float forceDistanceSample();
  const SensorSnapshot& snapshot() const;
  float readDistanceCm();
  float readTemperature();
  float readHumidity();
  int readGasRaw();
  int readGasFiltered() const;
  bool isMq2WarmingUp() const;

private:
  DHT dht;
  SensorSnapshot current;
  unsigned long startupMs;
  unsigned long lastFastSensorMs;
  unsigned long lastUltrasonicMs;
  unsigned long lastDhtMs;
  bool nextDistanceFacingFront;
  int mq2Samples[RobotConfig::MQ2_FILTER_SAMPLES];
  int mq2SampleCount;
  int mq2SampleIndex;
  long mq2SampleSum;

  void updateFastSensors(unsigned long now);
  void updateUltrasonic(unsigned long now);
  void updateDht(unsigned long now);
  void pushMq2Sample(int raw);
  bool readActiveDigital(int pin, int activeLevel) const;
};

#endif // SENSORMANAGER_H
