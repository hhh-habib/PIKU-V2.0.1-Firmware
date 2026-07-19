#include "DriveCommandHandler.h"

DriveCommandHandler::DriveCommandHandler(MotorController& motorController)
  : motors(motorController) {
}

DriveCommandResult DriveCommandHandler::handleCommand(const String& command, const String& currentManualState, const String& turnMode) {
  DriveCommandResult result;

  if (command.length() == 0) {
    return result;
  }

  if (command != "STOP" && command == currentManualState) {
    return result;
  }

  if (command == "FORWARD") {
    motors.forward();
    result.motorDisplayState = "FORWARD";
    result.navigationDecision = "MANUAL_FORWARD";
  } else if (command == "BACKWARD") {
    motors.backward();
    result.motorDisplayState = "BACKWARD";
    result.navigationDecision = "MANUAL_BACKWARD";
  } else if (command == "LEFT") {
    if (turnMode == "SPIN") {
      motors.spinLeft();
    } else {
      motors.leftTurn();
    }
    result.motorDisplayState = "LEFT";
    result.navigationDecision = "MANUAL_LEFT";
  } else if (command == "RIGHT") {
    if (turnMode == "SPIN") {
      motors.spinRight();
    } else {
      motors.rightTurn();
    }
    result.motorDisplayState = "RIGHT";
    result.navigationDecision = "MANUAL_RIGHT";
  } else if (command == "STOP") {
    motors.stop();
    result.motorDisplayState = "STOP";
    result.navigationDecision = "MANUAL_STOP";
  } else {
    return result;
  }

  result.executed = true;
  return result;
}
