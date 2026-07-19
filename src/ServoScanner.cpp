#include "ServoScanner.h"
#include "RobotConfig.h"

ServoScanner::ServoScanner(int servoPin)
  : pin(servoPin), angle(RobotConfig::CENTER_SCAN), attached(false) {
}

void ServoScanner::begin() {
  ensureAttached();
}

void ServoScanner::detach() {
  if (attached) {
    servo.detach();
    attached = false;
  }
}

void ServoScanner::center() {
  writeAngle(RobotConfig::CENTER_SCAN);
}

void ServoScanner::lookLeft() {
  writeAngle(RobotConfig::LEFT_SCAN);
}

void ServoScanner::lookRight() {
  writeAngle(RobotConfig::RIGHT_SCAN);
}

void ServoScanner::writeAngle(int newAngle) {
  ensureAttached();
  servo.write(newAngle);
  angle = newAngle;
}

int ServoScanner::currentAngle() const {
  return angle;
}

bool ServoScanner::isAttached() const {
  return attached;
}

void ServoScanner::ensureAttached() {
  if (!attached) {
    servo.attach(pin);
    attached = true;
  }
}
