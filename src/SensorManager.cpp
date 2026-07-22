#include "SensorManager.h"
#include "PinConfig.h"
#include "RobotConfig.h"

SensorManager::SensorManager()
  : dht(PinConfig::DHT_PIN, RobotConfig::DHT_TYPE),
    startupMs(0),
    lastFastSensorMs(0),
    lastUltrasonicMs(0),
    lastDhtMs(0),
    nextDistanceFacingFront(true),
    mq2SampleCount(0),
    mq2SampleIndex(0),
    mq2SampleSum(0) {
  for (int i = 0; i < RobotConfig::MQ2_FILTER_SAMPLES; i++) {
    mq2Samples[i] = 0;
  }
}

void SensorManager::begin() {
  pinMode(PinConfig::TRIG_PIN, OUTPUT);
  pinMode(PinConfig::ECHO_PIN, INPUT);
  pinMode(PinConfig::MQ2_PIN, INPUT);
  pinMode(PinConfig::IR_OBSTACLE_PIN, INPUT);
  pinMode(PinConfig::FLAME_PIN, INPUT);
  dht.begin();
  startupMs = millis();
  updateFastSensors(startupMs);
  updateUltrasonic(startupMs);
  updateDht(startupMs);
}

void SensorManager::update() {
  unsigned long now = millis();
  if (lastFastSensorMs == 0 || now - lastFastSensorMs >= RobotConfig::FAST_SENSOR_INTERVAL_MS) {
    updateFastSensors(now);
  }
  if (lastUltrasonicMs == 0 || now - lastUltrasonicMs >= RobotConfig::ULTRASONIC_INTERVAL_MS) {
    updateUltrasonic(now);
  }
  if (lastDhtMs == 0 || now - lastDhtMs >= RobotConfig::DHT_INTERVAL_MS) {
    updateDht(now);
  }
}

void SensorManager::setDistanceFacingFront(bool facingFront) {
  nextDistanceFacingFront = facingFront;
}

float SensorManager::forceDistanceSample() {
  updateUltrasonic(millis());
  return current.distanceCm;
}

const SensorSnapshot& SensorManager::snapshot() const {
  return current;
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
  return current.temperature;
}

float SensorManager::readHumidity() {
  return current.humidity;
}

int SensorManager::readGasRaw() {
  return current.gasRaw;
}

int SensorManager::readGasFiltered() const {
  return current.gasFiltered;
}

bool SensorManager::isMq2WarmingUp() const {
  return current.mq2Warmup;
}

void SensorManager::updateFastSensors(unsigned long now) {
  lastFastSensorMs = now;
  int raw = analogRead(PinConfig::MQ2_PIN);
  pushMq2Sample(raw);
  current.gasRaw = raw;
  current.gasFiltered = mq2SampleCount > 0 ? mq2SampleSum / mq2SampleCount : raw;
  current.mq2Warmup = now - startupMs < RobotConfig::MQ2_WARMUP_MS;
  current.irObstacleDetected = readActiveDigital(PinConfig::IR_OBSTACLE_PIN, RobotConfig::IR_ACTIVE_LEVEL);
  current.flameDetected = readActiveDigital(PinConfig::FLAME_PIN, RobotConfig::FLAME_ACTIVE_LEVEL);
  current.updatedMs = now;
}

void SensorManager::updateUltrasonic(unsigned long now) {
  lastUltrasonicMs = now;
  float distance = readDistanceCm();
  current.distanceCm = distance;
  current.distanceFacingFront = nextDistanceFacingFront;
  current.distanceValid = distance > 0.0f && distance <= RobotConfig::ULTRASONIC_MAX_CM;
  current.updatedMs = now;
}

void SensorManager::updateDht(unsigned long now) {
  lastDhtMs = now;
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (!isnan(temp)) {
    current.temperature = temp;
    current.temperatureValid = true;
  }
  if (!isnan(hum)) {
    current.humidity = hum;
    current.humidityValid = true;
  }
  current.updatedMs = now;
}

void SensorManager::pushMq2Sample(int raw) {
  if (mq2SampleCount < RobotConfig::MQ2_FILTER_SAMPLES) {
    mq2Samples[mq2SampleIndex] = raw;
    mq2SampleSum += raw;
    mq2SampleCount++;
  } else {
    mq2SampleSum -= mq2Samples[mq2SampleIndex];
    mq2Samples[mq2SampleIndex] = raw;
    mq2SampleSum += raw;
  }

  mq2SampleIndex = (mq2SampleIndex + 1) % RobotConfig::MQ2_FILTER_SAMPLES;
}

bool SensorManager::readActiveDigital(int pin, int activeLevel) const {
  return digitalRead(pin) == activeLevel;
}
