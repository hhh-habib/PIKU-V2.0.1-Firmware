#include <Arduino.h>
#include "BuzzerManager.h"
#include "DisplayManager.h"
#include "DashboardPublisher.h"
#include "DriveCommandHandler.h"
#include "MotorController.h"
#include "PinConfig.h"
#include "RobotConfig.h"
#include "SafetyManager.h"
#include "SensorManager.h"
#include "ServoScanner.h"
#include "WebDashboard.h"

MotorController motors(PinConfig::RIGHT_IN1, PinConfig::RIGHT_IN2, PinConfig::LEFT_IN1, PinConfig::LEFT_IN2);
DisplayManager display;
DriveCommandHandler driveCommands(motors);
SensorManager sensors;
ServoScanner servoScanner(PinConfig::SERVO_PIN);
BuzzerManager buzzer(PinConfig::BUZZER_PIN);
SafetyManager safety;
WebDashboard dashboard;
DashboardData dashboardData;
DashboardPublisher dashboardPublisher(dashboard, display, dashboardData);
String lastControlMode = "MANUAL";
String manualMotorDisplayState = "";
String currentServoPosition = "FRONT";

unsigned long lastDashboardPublishMs = 0;
unsigned long obstacleWarningUntilMs = 0;
unsigned long lastIrRecoveryMs = 0;
unsigned long criticalStopModeCommandCounter = 0;
float lastManualFrontDistance = -1;
bool criticalStopLatched = false;
bool irRecoveryLatched = false;

bool serviceLoop();
void publishCurrentTelemetry(bool force = false);
void triggerObstacleWarning();
bool runAutoNearObstacleRecovery();
float scanAt(int angle, String name);

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
    sensors.setDistanceFacingFront(currentServoPosition == "FRONT");
    serviceLoop();
    delay(1);
  }
}

String getGasStatus(const SensorSnapshot& snapshot) {
  if (snapshot.mq2Warmup) {
    return "WARMUP";
  }
  if (snapshot.gasFiltered >= RobotConfig::GAS_HIGH_RISK_THRESHOLD) {
    return "HIGH RISK";
  }
  if (snapshot.gasFiltered >= RobotConfig::GAS_CAUTION_THRESHOLD) {
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

void publishCurrentTelemetry(bool force) {
  unsigned long now = millis();
  if (!force && lastDashboardPublishMs != 0 &&
      now - lastDashboardPublishMs < RobotConfig::DASHBOARD_PUBLISH_INTERVAL_MS) {
    return;
  }

  const SensorSnapshot& snapshot = sensors.snapshot();
  String controlMode = dashboard.getControlMode();
  String navigationStatus = dashboardData.navigationDecision.length() > 0 ? dashboardData.navigationDecision : motors.getStateName();
  dashboardPublisher.publishSensorReadings(currentServoPosition,
                                           snapshot,
                                           getGasStatus(snapshot),
                                           safety.stateName(),
                                           safety.alarmReason(),
                                           safety.isNearObstacleHazardActive(),
                                           safety.nearObstacleSource(),
                                           safety.isUltrasonicWarningActive(),
                                           buzzer.soundStateName(),
                                           buzzer.modeName(),
                                           controlMode,
                                           dashboard.getTurnMode(),
                                           motors.getStateName(),
                                           manualMotorDisplayState,
                                           navigationStatus);
  lastDashboardPublishMs = now;
}

bool serviceLoop() {
  dashboard.loop();
  sensors.update();
  safety.update(sensors.snapshot());
  bool safetyChanged = safety.stateChanged();

  buzzer.setEnabled(dashboard.isAlarmEnabled());
  buzzer.setMuted(dashboard.isAlarmMuted());
  if (dashboard.consumeAlarmTestRequest()) {
    buzzer.requestTest();
  }

  if (safety.isCritical()) {
    if (!criticalStopLatched) {
      criticalStopLatched = true;
      criticalStopModeCommandCounter = dashboard.modeCommandCounter();
    }
    if (motors.getState() != MotorState::Stopped) {
      stopMotors();
    }
    manualMotorDisplayState = "STOP";
    dashboard.setPendingCommand("");
    dashboardData.navigationDecision = "SAFETY_STOP";
  }

  if (safety.isCritical()) {
    buzzer.setMode(BuzzerMode::Critical);
  } else if (safety.isNearObstacleHazardActive() || millis() < obstacleWarningUntilMs) {
    buzzer.setMode(BuzzerMode::Hazard);
  } else if (safety.isCaution()) {
    buzzer.setMode(BuzzerMode::Caution);
  } else {
    buzzer.setMode(BuzzerMode::Off);
  }
  buzzer.update();
  publishCurrentTelemetry(safetyChanged);
  return safetyChanged;
}

bool handleManualControl() {
  String command = dashboard.getPendingCommand();

  if (command.length() == 0) {
    return false;
  }

  if (safety.blocksMotion() && command != "STOP") {
    stopMotors();
    manualMotorDisplayState = "STOP";
    dashboard.setPendingCommand("");
    dashboardData.navigationDecision = "SAFETY_STOP";
    syncDashboardState("MANUAL");
    return false;
  }

  if (command == "FORWARD" && safety.blocksForwardMotion()) {
    stopMotors();
    manualMotorDisplayState = "STOP";
    dashboard.setPendingCommand("");
    dashboardData.navigationDecision = safety.isNearObstacleHazardActive() ? "FORWARD_BLOCKED_NEAR" : "FORWARD_BLOCKED";
    syncDashboardState("MANUAL");
    return false;
  }

  String currentManualState = manualMotorDisplayState.length() > 0 ? manualMotorDisplayState : motors.getStateName();
  DriveCommandResult result = driveCommands.handleCommand(command, currentManualState, dashboard.getTurnMode());

  dashboard.setPendingCommand("");
  if (!result.executed) {
    return false;
  }

  if (criticalStopLatched && !safety.isCritical()) {
    criticalStopLatched = false;
  }
  manualMotorDisplayState = result.motorDisplayState;
  dashboardData.navigationDecision = result.navigationDecision;
  syncDashboardState("MANUAL");
  return true;
}

float readAndPublishSensors(String servoPos) {
  currentServoPosition = servoPos;
  sensors.setDistanceFacingFront(servoPos == "FRONT");
  float distance = sensors.forceDistanceSample();
  safety.update(sensors.snapshot());
  publishCurrentTelemetry(true);
  return distance;
}

void triggerObstacleWarning() {
  obstacleWarningUntilMs = millis() + RobotConfig::OBSTACLE_WARNING_MS;
}

bool runAutoNearObstacleRecovery() {
  unsigned long now = millis();
  if (irRecoveryLatched && safety.isNearObstacleHazardActive()) {
    stopMotors();
    dashboardData.navigationDecision = "NEAR_OBSTACLE_HOLD";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    triggerObstacleWarning();
    responsiveDelay(100);
    return true;
  }

  if (lastIrRecoveryMs != 0 && now - lastIrRecoveryMs < RobotConfig::IR_RECOVERY_COOLDOWN_MS) {
    stopMotors();
    dashboardData.navigationDecision = "NEAR_RECOVERY_COOLDOWN";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    responsiveDelay(100);
    return true;
  }

  irRecoveryLatched = true;
  lastIrRecoveryMs = now;
  stopMotors();
  triggerObstacleWarning();
  dashboardData.navigationDecision = "NEAR_OBSTACLE_RECOVERY";
  dashboard.setNavigationDecision(dashboardData.navigationDecision);
  publishCurrentTelemetry(true);
  responsiveDelay(RobotConfig::IR_RECOVERY_SETTLE_MS);
  if (safety.isCritical()) {
    return true;
  }

  dashboardData.navigationDecision = "NEAR_REVERSE";
  dashboard.setNavigationDecision(dashboardData.navigationDecision);
  backward();
  responsiveDelay(RobotConfig::IR_RECOVERY_REVERSE_MS);
  stopMotors();
  responsiveDelay(250);
  if (safety.isCritical()) {
    return true;
  }

  float left = scanAt(RobotConfig::LEFT_SCAN, "LEFT");
  float right = scanAt(RobotConfig::RIGHT_SCAN, "RIGHT");
  if (safety.isCritical()) {
    return true;
  }

  servoScanner.center();
  responsiveDelay(400);
  servoScanner.detach();

  bool leftValid = left > 0;
  bool rightValid = right > 0;
  if (!leftValid && !rightValid) {
    dashboardData.navigationDecision = "NEAR_SCAN_INVALID_STOP";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    stopMotors();
  } else if (leftValid && (!rightValid || left > right)) {
    dashboardData.navigationDecision = "NEAR_TURN_LEFT";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    leftTurn();
    responsiveDelay(170);
    stopMotors();
  } else {
    dashboardData.navigationDecision = "NEAR_TURN_RIGHT";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    rightTurn();
    responsiveDelay(170);
    stopMotors();
  }

  responsiveDelay(300);
  safety.update(sensors.snapshot());
  if (!safety.isNearObstacleHazardActive()) {
    irRecoveryLatched = false;
  }
  return true;
}

float scanAt(int angle, String name) {
  stopMotors();
  currentServoPosition = name;
  sensors.setDistanceFacingFront(name == "FRONT");
  responsiveDelay(250);
  if (safety.isCritical()) {
    return -1;
  }

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
  if (safety.isCritical()) {
    servoScanner.detach();
    return -1;
  }

  float distance = readAndPublishSensors(name);

  servoScanner.detach();
  responsiveDelay(250);

  return distance;
}

void updateManualMode() {
  holdManualServoForward();
  sensors.setDistanceFacingFront(true);
  handleManualControl();

  currentServoPosition = "FRONT";
  const SensorSnapshot& snapshot = sensors.snapshot();
  lastManualFrontDistance = snapshot.distanceCm;

  if (motors.getState() == MotorState::Forward &&
      safety.isNearObstacleHazardActive()) {
    stopMotors();
    manualMotorDisplayState = "STOP";
    dashboard.setPendingCommand("");
    dashboardData.navigationDecision = "FORWARD_BLOCKED_NEAR";
    syncDashboardState("MANUAL");
    Serial.println("Manual near obstacle stop.");
  } else if (motors.getState() == MotorState::Forward &&
             snapshot.distanceValid &&
             lastManualFrontDistance <= RobotConfig::OBSTACLE_CM) {
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

  buzzer.begin();
  sensors.begin();
  safety.begin();
  display.begin();

  stopMotors();
  manualMotorDisplayState = "STOP";
  dashboardData.controlMode = "MANUAL";
  dashboardData.turnMode = "PIVOT";
  dashboardData.motorState = "STOP";
  dashboardData.navigationDecision = "MANUAL_CONTROL";
  dashboardData.safetyState = "SAFE";
  dashboardData.alarmReason = "NONE";
  dashboardData.alarmSoundState = "ENABLED";
  dashboardData.buzzerState = "OFF";

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
  serviceLoop();

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

  if (safety.isCritical()) {
    publishCurrentTelemetry(true);
    delay(1);
    return;
  }

  if (criticalStopLatched) {
    if (dashboard.modeCommandCounter() != criticalStopModeCommandCounter) {
      criticalStopLatched = false;
      stopMotors();
      dashboardData.navigationDecision = controlMode == "AUTO" ? "AUTO_REARMED" : "MANUAL_CONTROL";
      dashboard.setNavigationDecision(dashboardData.navigationDecision);
    } else if (controlMode == "AUTO") {
      stopMotors();
      dashboardData.navigationDecision = "SAFETY_CLEARED_STOP";
      dashboard.setNavigationDecision(dashboardData.navigationDecision);
      publishCurrentTelemetry(true);
      delay(1);
      return;
    }
  }

  if (controlMode == "MANUAL") {
    updateManualMode();
    serviceLoop();
    return;
  }

  if (!safety.isNearObstacleHazardActive()) {
    irRecoveryLatched = false;
  }

  if (safety.isNearObstacleHazardActive()) {
    if (runAutoNearObstacleRecovery()) {
      return;
    }
  }

  float front = scanAt(RobotConfig::CENTER_SCAN, "FRONT");
  const SensorSnapshot& frontSnapshot = sensors.snapshot();

  if (safety.isCritical()) {
    publishCurrentTelemetry(true);
    return;
  }

  if (safety.isNearObstacleHazardActive()) {
    if (runAutoNearObstacleRecovery()) {
      return;
    }
  }

  if (front < 0 || !frontSnapshot.distanceValid) {
    Serial.println("Ultrasonic invalid. Holding position.");
    dashboardData.navigationDecision = "ULTRASONIC_INVALID_STOP";
    dashboard.setNavigationDecision(dashboardData.navigationDecision);
    stopMotors();
    responsiveDelay(250);
  } else if (front > RobotConfig::OBSTACLE_CM && !safety.blocksForwardMotion()) {
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

    if (safety.isCritical()) {
      publishCurrentTelemetry(true);
      return;
    }

    servoScanner.center();
    responsiveDelay(400);
    servoScanner.detach();

    bool leftValid = left > 0;
    bool rightValid = right > 0;

    if (!leftValid && !rightValid) {
      Serial.println("Side scans invalid. Holding position.");
      dashboardData.navigationDecision = "SIDE_SCAN_INVALID_STOP";
      dashboard.setNavigationDecision(dashboardData.navigationDecision);
      stopMotors();
    } else if (leftValid && (!rightValid || left > right)) {
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
