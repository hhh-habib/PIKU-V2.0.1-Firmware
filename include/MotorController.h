#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include <Arduino.h>

enum class MotorState {
  Stopped,
  Forward,
  Backward,
  LeftTurn,
  RightTurn,
  SpinLeft,
  SpinRight
};

class MotorController {
public:
  MotorController(int rightIn1, int rightIn2, int leftIn1, int leftIn2);

  void stop();
  void forward();
  void backward();
  void leftTurn();
  void rightTurn();
  void spinLeft();
  void spinRight();

  MotorState getState() const;
  const char* getStateName() const;
  static const char* stateToString(MotorState state);

private:
  int _rightIn1;
  int _rightIn2;
  int _leftIn1;
  int _leftIn2;
  MotorState currentState;

  void coast();
  void settleBeforeDirectionChange();
  void energizeRightForward();
  void energizeLeftForward();
  void energizeRightBackward();
  void energizeLeftBackward();
  void applyForwardStartupCompensation();
  void applyBackwardStartupCompensation();
  void setState(MotorState state);
  void logState(const char* stateName) const;
};

#endif // MOTORCONTROLLER_H
