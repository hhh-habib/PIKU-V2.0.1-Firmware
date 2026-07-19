#ifndef SERVOSCANNER_H
#define SERVOSCANNER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoScanner {
public:
  explicit ServoScanner(int servoPin);

  void begin();
  void detach();
  void center();
  void lookLeft();
  void lookRight();
  void writeAngle(int angle);
  int currentAngle() const;
  bool isAttached() const;

private:
  Servo servo;
  int pin;
  int angle;
  bool attached;

  void ensureAttached();
};

#endif // SERVOSCANNER_H
