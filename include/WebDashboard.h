#ifndef WEBDASHBOARD_H
#define WEBDASHBOARD_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

struct DashboardData {
  float temperature = 0.0f;
  float humidity = 0.0f;
  int gasValue = 0;
  int gasFilteredValue = 0;
  float frontDistance = 0.0f;
  bool distanceValid = false;
  bool mq2Warmup = true;
  bool flameDetected = false;
  bool irObstacleDetected = false;
  bool nearObstacleHazard = false;
  String nearObstacleSource = "NONE";
  bool ultrasonicWarning = false;
  String motorState = "STOP";
  String navigationDecision = "IDLE";
  String gasStatus = "SAFE";
  String safetyState = "SAFE";
  String alarmReason = "NONE";
  String alarmSoundState = "ENABLED";
  String buzzerState = "OFF";
  String controlMode = "AUTO";
  String turnMode = "PIVOT";
};

class WebDashboard {
public:
  WebDashboard();
  void begin(const char* ssid, const char* password);
  void loop();
  void setData(const DashboardData& data);
  void setNavigationDecision(const String& decision);
  void setControlMode(const String& mode);
  void setTurnMode(const String& mode);
  void setPendingCommand(const String& command);
  String getControlMode() const;
  String getTurnMode() const;
  String getPendingCommand() const;
  bool isAlarmEnabled() const;
  bool isAlarmMuted() const;
  bool consumeAlarmTestRequest();
  unsigned long modeCommandCounter() const;
  IPAddress localIP() const;

private:
  WebServer _server;
  DashboardData _data;
  bool _initialized;
  bool _alarmEnabled;
  bool _alarmMuted;
  bool _alarmTestRequested;
  unsigned long _modeCommandCounter;
  String _pendingCommand;

  void handleRoot();
  void handleData();
  void handleMode();
  void handleTurnMode();
  void handleCommand();
  void handleAlarmEnable();
  void handleAlarmMute();
  void handleAlarmTest();
  void handleModeAuto();
  void handleModeManual();
  void handleControlForward();
  void handleControlBackward();
  void handleControlLeft();
  void handleControlRight();
  void handleControlStop();
  void applyMode(const String& mode);
  void applyTurnMode(const String& mode);
  void applyCommand(const String& command);
  void sendOk();
  void sendJson();
};

#endif
