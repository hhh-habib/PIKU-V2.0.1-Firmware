#ifndef DRIVECOMMANDHANDLER_H
#define DRIVECOMMANDHANDLER_H

#include <Arduino.h>
#include "MotorController.h"

struct DriveCommandResult {
  bool executed = false;
  String motorDisplayState = "";
  String navigationDecision = "";
};

class DriveCommandHandler {
public:
  explicit DriveCommandHandler(MotorController& motorController);

  DriveCommandResult handleCommand(const String& command, const String& currentManualState, const String& turnMode);

private:
  MotorController& motors;
};

#endif // DRIVECOMMANDHANDLER_H
