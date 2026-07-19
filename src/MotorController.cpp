#include "MotorController.h"

// Tested L298N input transition guard: coast before changing direction.
static constexpr unsigned long DIRECTION_SETTLE_MS = 5;
// Current tested forward startup baseline.
static constexpr unsigned long FORWARD_STARTUP_COMPENSATION_MS = 80;
// Reverse compensation previously made backward drift worse, so keep it disabled.
static constexpr unsigned long BACKWARD_STARTUP_COMPENSATION_MS = 0;
static constexpr bool MOTOR_DEBUG = false;

// FORWARD currently drifts RIGHT at startup, so the RIGHT side gets a head start.
// If FORWARD drift gets worse or flips direction, change this value.
const bool FORWARD_WEAKER_SIDE_IS_RIGHT = true;

// BACKWARD can have different transient behavior. Tune this separately after testing.
// If BACKWARD drift gets worse or flips direction, change this value.
const bool BACKWARD_WEAKER_SIDE_IS_RIGHT = true;

MotorController::MotorController(int rightIn1, int rightIn2, int leftIn1, int leftIn2)
  : _rightIn1(rightIn1),
    _rightIn2(rightIn2),
    _leftIn1(leftIn1),
    _leftIn2(leftIn2),
    currentState(MotorState::Stopped) {
}

void MotorController::setState(MotorState state) {
  currentState = state;
  logState(stateToString(state));
}

MotorState MotorController::getState() const {
  return currentState;
}

const char* MotorController::getStateName() const {
  return stateToString(currentState);
}

const char* MotorController::stateToString(MotorState state) {
  switch (state) {
    case MotorState::Forward:
      return "FORWARD";
    case MotorState::Backward:
      return "BACKWARD";
    case MotorState::LeftTurn:
    case MotorState::SpinLeft:
      return "LEFT";
    case MotorState::RightTurn:
    case MotorState::SpinRight:
      return "RIGHT";
    case MotorState::Stopped:
    default:
      return "STOP";
  }
}

void MotorController::logState(const char* stateName) const {
  if (MOTOR_DEBUG) {
    Serial.print("[MOTOR] ");
    Serial.println(stateName);
  }
}

void MotorController::coast() {
  digitalWrite(_rightIn1, LOW);
  digitalWrite(_rightIn2, LOW);
  digitalWrite(_leftIn1, LOW);
  digitalWrite(_leftIn2, LOW);
}

void MotorController::settleBeforeDirectionChange() {
  coast();
  delay(DIRECTION_SETTLE_MS);
}

void MotorController::energizeRightForward() {
  digitalWrite(_rightIn1, LOW);
  digitalWrite(_rightIn2, HIGH);
}

void MotorController::energizeLeftForward() {
  digitalWrite(_leftIn1, LOW);
  digitalWrite(_leftIn2, HIGH);
}

void MotorController::energizeRightBackward() {
  digitalWrite(_rightIn1, HIGH);
  digitalWrite(_rightIn2, LOW);
}

void MotorController::energizeLeftBackward() {
  digitalWrite(_leftIn1, HIGH);
  digitalWrite(_leftIn2, LOW);
}

void MotorController::applyForwardStartupCompensation() {
  if (FORWARD_STARTUP_COMPENSATION_MS == 0) {
    return;
  }

  if (FORWARD_WEAKER_SIDE_IS_RIGHT) {
    energizeRightForward();
  } else {
    energizeLeftForward();
  }
  delay(FORWARD_STARTUP_COMPENSATION_MS);
}

void MotorController::applyBackwardStartupCompensation() {
  if (BACKWARD_STARTUP_COMPENSATION_MS == 0) {
    return;
  }

  if (BACKWARD_WEAKER_SIDE_IS_RIGHT) {
    energizeRightBackward();
  } else {
    energizeLeftBackward();
  }
  delay(BACKWARD_STARTUP_COMPENSATION_MS);
}

void MotorController::stop() {
  coast();
  setState(MotorState::Stopped);
}

void MotorController::forward() {
  if (currentState == MotorState::Forward) {
    return;
  }

  settleBeforeDirectionChange();
  applyForwardStartupCompensation();
  energizeLeftForward();
  energizeRightForward();
  setState(MotorState::Forward);
}

void MotorController::backward() {
  if (currentState == MotorState::Backward) {
    return;
  }

  settleBeforeDirectionChange();
  applyBackwardStartupCompensation();
  energizeLeftBackward();
  energizeRightBackward();
  setState(MotorState::Backward);
}

void MotorController::leftTurn() {
  settleBeforeDirectionChange();
  digitalWrite(_rightIn1, LOW);
  digitalWrite(_rightIn2, HIGH);
  digitalWrite(_leftIn1, LOW);
  digitalWrite(_leftIn2, LOW);
  setState(MotorState::LeftTurn);
}

void MotorController::rightTurn() {
  settleBeforeDirectionChange();
  digitalWrite(_rightIn1, LOW);
  digitalWrite(_rightIn2, LOW);
  digitalWrite(_leftIn1, LOW);
  digitalWrite(_leftIn2, HIGH);
  setState(MotorState::RightTurn);
}

void MotorController::spinLeft() {
  settleBeforeDirectionChange();
  energizeLeftBackward();
  energizeRightForward();
  setState(MotorState::SpinLeft);
}

void MotorController::spinRight() {
  settleBeforeDirectionChange();
  energizeLeftForward();
  energizeRightBackward();
  setState(MotorState::SpinRight);
}
