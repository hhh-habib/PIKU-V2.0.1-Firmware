#include <Arduino.h>
#include "DisplayManager.h"
#include "DashboardPublisher.h"
#include "DriveCommandHandler.h"
#include "MotorController.h"
#include "PinConfig.h"
#include "RobotConfig.h"
#include "SensorManager.h"
#include "ServoScanner.h"
#include "WebDashboard.h"

MotorController motors(PinConfig::RIGHT_IN1, PinConfig::RIGHT_IN2, PinConfig::LEFT_IN1, PinConfig::LEFT_IN2);
DisplayManager display;
DriveCommandHandler driveCommands(motors);
SensorManager sensors;
ServoScanner servoScanner(PinConfig::SERVO_PIN);
WebDashboard dashboard;
DashboardData dashboardData;
DashboardPublisher dashboardPublisher(dashboard, display, dashboardData);
String lastControlMode = "MANUAL";
String manualMotorDisplayState = "";

unsigned long lastManualSensorMs = 0;
float lastManualFrontDistance = -1;

void stopMotors() {
  motors.stop();
}

void forward() {
  motors.forward();
}

void backward() {
  motors.backward();
}

void leftTurn() {
  motors.leftTurn();
}

void rightTurn() {
  motors.rightTurn();
}

void spinLeft() {
  motors.spinLeft();
}

void spinRight() {
  motors.spinRight();
}

void responsiveDelay(unsigned long durationMs) {
  unsigned long startMs = millis();
  while (millis() - startMs < durationMs) {
    dashboard.loop();
    delay(1);
  }
}

String getGasStatus(int gasValue) {
  if (gasValue >= RobotConfig::GAS_HIGH_RISK_THRESHOLD) {
    return "HIGH RISK";
  }
  if (gasValue >= RobotConfig::GAS_CAUTION_THRESHOLD) {
    return "CAUTION";
  }
  return "SAFE";
}

void holdManualServoForward() {
  servoScanner.center();
}

void releaseManualServo() {
  servoScanner.detach();
}

void syncDashboardState(const String& controlMode) {
  dashboardPublisher.syncState(controlMode, dashboard.getTurnMode(), motors.getStateName(), manualMotorDisplayState);
}

bool handleManualControl() {
  String command = dashboard.getPendingCommand();

  if (command.length() == 0) {
    return false;
  }

  String currentManualState = manualMotorDisplayState.length() > 0 ? manualMotorDisplayState : motors.getStateName();
  DriveCommandResult result = driveCommands.handleCommand(command, currentManualState, dashboard.getTurnMode());

  dashboard.setPendingCommand("");
  if (!result.executed) {
    return false;
  }

  manualMotorDisplayState = result.motorDisplayState;
  dashboardData.navigationDecision = result.navigationDecision;
  syncDashboardState("MANUAL");
  return true;
}

void publishSensorReadings(String servoPos, float distance, float temp, float hum, int gasValue) {
  String gasStatus = getGasStatus(gasValue);
  String controlMode = dashboard.getControlMode();
  String navigationStatus = dashboardData.navigationDecision.length() > 0 ? dashboardData.navigationDecision : motors.getStateName();
  dashboardPublisher.publishSensorReadings(servoPos,
                                           distance,
                                           temp,
                                           hum,
                                           gasValue,
                                           gasStatus,
                                           controlMode,
                                           dashboard.getTurnMode(),
                                           motors.getStateName(),
                                           manualMotorDisplayState,
                                           navigationStatus);
}

float readAndPublishSensors(String servoPos) {
  float distance = sensors.readDistanceCm();
  float temp = sensors.readTemperature();
  float hum = sensors.readHumidity();
  int gasValue = sensors.readGasRaw();

  publishSensorReadings(servoPos, distance, temp, hum, gasValue);

  return distance;
}

float scanAt(int angle, String name) {
  stopMotors();
  responsiveDelay(250);

  if (angle == RobotConfig::CENTER_SCAN) {
    servoScanner.center();
  } else if (angle == RobotConfig::LEFT_SCAN) {
    servoScanner.lookLeft();
  } else if (angle == RobotConfig::RIGHT_SCAN) {
    servoScanner.lookRight();
  } else {
    servoScanner.writeAngle(angle);
  }
  responsiveDelay(700);

  float distance = readAndPublishSensors(name);

  servoScanner.detach();
  responsiveDelay(250);

  return distance;
}

void updateManualMode() {
  holdManualServoForward();
  handleManualControl();

  unsigned long now = millis();
  if (lastManualSensorMs == 0 || now - lastManualSensorMs >= RobotConfig::MANUAL_SENSOR_INTERVAL_MS) {
    lastManualFrontDistance = readAndPublishSensors("FRONT");
    lastManualSensorMs = now;
  }

  if (motors.getState() == MotorState::Forward && lastManualFrontDistance > 0 && lastManualFrontDistance <= RobotConfig::OBSTACLE_CM) {
    stopMotors();
    manualMotorDisplayState = "STOP";
    dashboard.setPendingCommand("");
    dashboardData.navigationDecision = "MANUAL_OBSTACLE_STOP";
    syncDashboardState("MANUAL");
    Serial.println("Manual obstacle stop.");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PinConfig::RIGHT_IN1, OUTPUT);
  pinMode(PinConfig::RIGHT_IN2, OUTPUT);
  pinMode(PinConfig::LEFT_IN1, OUTPUT);
  pinMode(PinConfig::LEFT_IN2, OUTPUT);

  //pinMode(PinConfig::BUZZER_PIN, OUTPUT);

  sensors.begin();
  display.begin();

  stopMotors();
  manualMotorDisplayState = "STOP";
  dashboardData.controlMode = "MANUAL";
  dashboardData.turnMode = "PIVOT";
  dashboardData.motorState = "STOP";
  dashboardData.navigationDecision = "MANUAL_CONTROL";

  servoScanner.center();
  responsiveDelay(700);
  servoScanner.detach();

  dashboard.begin("PIKU_V2", "piku1234");
  dashboard.setControlMode("MANUAL");
  dashboard.setTurnMode("PIVOT");
  syncDashboardState("MANUAL");
  responsiveDelay(1000);
}

void loop() {
  dashboard.loop();

  String controlMode = dashboard.getControlMode();
  if (controlMode != lastControlMode) {
    stopMotors();
    lastControlMode = controlMode;
    if (controlMode == "MANUAL") {
      manualMotorDisplayState = "STOP";
      holdManualServoForward();
      dashboardData.navigationDecision = "MANUAL_CONTROL";
    } else {
      manualMotorDisplayState = "";
      releaseManualServo();
    }
    syncDashboardState(controlMode);
  }

  if (controlMode == "MANUAL") {
    updateManualMode();
    dashboard.loop();
    return;
  }

  float front = scanAt(RobotConfig::CENTER_SCAN, "FRONT");

  if (front == -1 || front > RobotConfig::OBSTACLE_CM) {
    Serial.println("Decision: FORWARD");
    dashboardData.navigationDecision = "FORWARD";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);

    forward();
    responsiveDelay(300);
    stopMotors();
    responsiveDelay(250);
  } 
  else {
    Serial.println("Obstacle detected. Moving backward.");
    dashboardData.navigationDecision = "BACKWARD_RECOVERY";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);

    backward();
    responsiveDelay(220);
    stopMotors();
    responsiveDelay(250);

    Serial.println("Scanning sides.");

    float left = scanAt(RobotConfig::LEFT_SCAN, "LEFT");
    float right = scanAt(RobotConfig::RIGHT_SCAN, "RIGHT");

    servoScanner.center();
    responsiveDelay(400);
    servoScanner.detach();

    if (left > right) {
      Serial.println("Decision: LEFT");
      dashboardData.navigationDecision = "LEFT";
      dashboard.setNavigationDecision(dashboardData.navigationDecision);
      leftTurn();
      responsiveDelay(170);
      stopMotors();
    } else {
      Serial.println("Decision: RIGHT");
      dashboardData.navigationDecision = "RIGHT";
      dashboard.setNavigationDecision(dashboardData.navigationDecision);
      rightTurn();
      responsiveDelay(170);
      stopMotors();
    }

    responsiveDelay(300);
  }
}
