#include "SensorManager.h"
#include "PinConfig.h"
#include "RobotConfig.h"

SensorManager::SensorManager()
  : dht(PinConfig::DHT_PIN, RobotConfig::DHT_TYPE) {
}

void SensorManager::begin() {
  pinMode(PinConfig::TRIG_PIN, OUTPUT);
  pinMode(PinConfig::ECHO_PIN, INPUT);
  dht.begin();
}

float SensorManager::readDistanceCm() {
  digitalWrite(PinConfig::TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(PinConfig::TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(PinConfig::TRIG_PIN, LOW);

  long duration = pulseIn(PinConfig::ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.0343 / 2.0;
}

float SensorManager::readTemperature() {
  return dht.readTemperature();
}

float SensorManager::readHumidity() {
  return dht.readHumidity();
}

int SensorManager::readGasRaw() {
  return analogRead(PinConfig::MQ2_PIN);
}
