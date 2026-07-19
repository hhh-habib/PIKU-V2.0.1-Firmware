#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <DHT.h>

class SensorManager {
public:
  SensorManager();

  void begin();
  float readDistanceCm();
  float readTemperature();
  float readHumidity();
  int readGasRaw();

private:
  DHT dht;
};

#endif // SENSORMANAGER_H
