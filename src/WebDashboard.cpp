#include "WebDashboard.h"

WebDashboard::WebDashboard() : _server(80), _initialized(false) {}

void WebDashboard::begin(const char* ssid, const char* password) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(100);

  _server.on("/", [this]() { handleRoot(); });
  _server.on("/data", [this]() { handleData(); });
  _server.on("/mode", [this]() { handleMode(); });
  _server.on("/turn-mode", [this]() { handleTurnMode(); });
  _server.on("/cmd", [this]() { handleCommand(); });
  _server.on("/mode/auto", [this]() { handleModeAuto(); });
  _server.on("/mode/manual", [this]() { handleModeManual(); });
  _server.on("/control/forward", [this]() { handleControlForward(); });
  _server.on("/control/backward", [this]() { handleControlBackward(); });
  _server.on("/control/left", [this]() { handleControlLeft(); });
  _server.on("/control/right", [this]() { handleControlRight(); });
  _server.on("/control/stop", [this]() { handleControlStop(); });
  _server.begin();
  _initialized = true;

  Serial.println("[WebDashboard] Access Point started");
  Serial.print("[WebDashboard] SSID: ");
  Serial.println(ssid);
  Serial.print("[WebDashboard] IP: ");
  Serial.println(WiFi.softAPIP());
}

void WebDashboard::loop() {
  if (_initialized) {
    _server.handleClient();
  }
}

void WebDashboard::setData(const DashboardData& data) {
  _data = data;
}

void WebDashboard::setNavigationDecision(const String& decision) {
  _data.navigationDecision = decision;
}

void WebDashboard::setControlMode(const String& mode) {
  _data.controlMode = mode;
}

void WebDashboard::setTurnMode(const String& mode) {
  _data.turnMode = mode;
}

void WebDashboard::setPendingCommand(const String& command) {
  _pendingCommand = command;
}

String WebDashboard::getControlMode() const {
  return _data.controlMode;
}

String WebDashboard::getTurnMode() const {
  return _data.turnMode;
}

String WebDashboard::getPendingCommand() const {
  return _pendingCommand;
}

IPAddress WebDashboard::localIP() const {
  return WiFi.softAPIP();
}

void WebDashboard::handleRoot() {
  String html = R"html(
<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <title>PIKU V2 Dashboard</title>
    <style>
      * { box-sizing:border-box; }
      body {
        font-family: Arial, sans-serif;
        background:#111;
        color:#eee;
        margin:0;
        min-height:100%;
        overflow-y:auto;
        padding:14px 14px 96px;
      }
      h2 { margin:0 0 10px; font-size:22px; }
      .card {
        background:#1e1e1e;
        border:1px solid #333;
        border-radius:8px;
        padding:12px;
        margin:10px 0;
      }
      .row {
        display:flex;
        justify-content:space-between;
        gap:16px;
        padding:6px 0;
        border-bottom:1px solid #2a2a2a;
      }
      .row:last-child { border-bottom:0; }
      .label { color:#7dd3fc; }
      .value { text-align:right; font-weight:bold; }
      button {
        border:0;
        border-radius:8px;
        color:#fff;
        background:#334155;
        font-weight:bold;
        cursor:pointer;
      }
      .mode-bar {
        display:grid;
        grid-template-columns:1fr 1fr;
        gap:10px;
        margin-bottom:12px;
      }
      .mode-btn {
        min-height:52px;
        font-size:16px;
        background:#374151;
      }
      .mode-btn.active {
        background:#0f766e;
        box-shadow:0 0 0 2px #5eead4 inset;
      }
      .turn-bar {
        display:grid;
        grid-template-columns:1fr;
        gap:10px;
        margin-bottom:12px;
      }
      .turn-btn {
        min-height:46px;
        font-size:15px;
        background:#475569;
      }
      .turn-btn.spin {
        background:#7c3aed;
      }
      .alarm-bar {
        display:grid;
        grid-template-columns:1fr 1fr 1fr;
        gap:10px;
        margin-top:10px;
      }
      .alarm-btn { min-height:46px; font-size:14px; background:#475569; }
      .audio-status {
        margin-top:8px;
        color:#facc15;
        font-size:13px;
        font-weight:bold;
        text-align:center;
      }
      .alarm-indicator {
        margin-top:10px;
        padding:10px;
        border-radius:8px;
        text-align:center;
        font-weight:bold;
        background:#064e3b;
      }
      .alarm-indicator.caution { background:#854d0e; }
      .alarm-indicator.high { background:#991b1b; }
      .dpad {
        display:grid;
        grid-template-columns:1fr 1fr 1fr;
        grid-template-areas:
          ". forward ."
          "left stop right"
          ". backward .";
        gap:12px;
        max-width:420px;
        margin:0 auto;
        user-select:none;
        -webkit-user-select:none;
        -webkit-touch-callout:none;
      }
      .dpad button {
        touch-action:none;
        -ms-touch-action:none;
      }
      .dpad button {
        min-height:74px;
        font-size:16px;
        line-height:1.15;
        background:#2563eb;
      }
      .dpad button span {
        display:block;
        font-size:24px;
        margin-bottom:4px;
      }
      .dpad .stop {
        grid-area:stop;
        background:#dc2626;
        min-height:82px;
      }
      .forward { grid-area:forward; }
      .backward { grid-area:backward; }
      .left { grid-area:left; }
      .right { grid-area:right; }
      .dpad.auto button {
        opacity:.38;
        filter:grayscale(1);
      }
      .dpad.auto .stop {
        opacity:.65;
      }
      .refresh-btn {
        width:100%;
        min-height:44px;
        margin-top:6px;
      }
    </style>
  </head>
  <body>
    <h2>PIKU V2 Dashboard</h2>
    <div class="card">
      <div class="row"><span class="label">Temperature</span><span class="value" id="temp">--</span></div>
      <div class="row"><span class="label">Humidity</span><span class="value" id="hum">--</span></div>
      <div class="row"><span class="label">MQ2 Raw</span><span class="value" id="gas">--</span></div>
      <div class="row"><span class="label">Gas Status</span><span class="value" id="gasStatus">--</span></div>
      <div class="alarm-indicator" id="alarmIndicator">ALARM: SAFE</div>
      <div class="audio-status" id="audioStatus">AUDIO DISABLED</div>
      <div class="alarm-bar">
        <button class="alarm-btn" id="enableAlarmBtn" onclick="enableAlarm(event)">ENABLE ALARM</button>
        <button class="alarm-btn" id="testBeepBtn" onclick="testBeep(event)">TEST BEEP</button>
        <button class="alarm-btn" id="muteAlarmBtn" onclick="disableAlarm(event)">MUTE ALARM</button>
      </div>
      <div class="row"><span class="label">Front Distance</span><span class="value" id="dist">--</span></div>
      <div class="row"><span class="label">Motor State</span><span class="value" id="motor">--</span></div>
      <div class="row"><span class="label">Navigation</span><span class="value" id="nav">--</span></div>
      <div class="row"><span class="label">Mode</span><span class="value" id="mode">--</span></div>
      <div class="row"><span class="label">Turn Mode</span><span class="value" id="turnMode">--</span></div>
    </div>
    <div class="card">
      <div class="mode-bar">
        <button class="mode-btn" id="autoBtn" onclick="setMode('AUTO')">AUTO MODE</button>
        <button class="mode-btn" id="manualBtn" onclick="setMode('MANUAL')">MANUAL MODE</button>
      </div>
      <div class="turn-bar">
        <button class="turn-btn" id="turnModeBtn" onclick="toggleTurnMode()">Turn Mode: PIVOT</button>
      </div>
      <div class="dpad auto" id="dpad">
        <button class="forward hold" data-move="FORWARD"><span>^</span>FORWARD</button>
        <button class="left hold" data-move="LEFT"><span>&lt;</span>LEFT</button>
        <button class="stop" id="stopBtn"><span>■</span>STOP</button>
        <button class="right hold" data-move="RIGHT"><span>&gt;</span>RIGHT</button>
        <button class="backward hold" data-move="BACKWARD"><span>v</span>BACKWARD</button>
      </div>
    </div>
    <button class="refresh-btn" onclick="loadData()">Refresh</button>
    <script>
      let mode = 'AUTO';
      let turnMode = 'PIVOT';
      let audioCtx = null;
      let alarmOutput = null;
      let alarmEnabled = false;
      let alarmMuted = false;
      let highRiskTimer = null;
      let highRiskVibeTimer = null;
      let lastCautionBeep = 0;
      let activePress = false;
      let releaseStopTimer = null;
      let minStopAt = 0;

      function setMode(nextMode) {
        fetch('/mode?set=' + encodeURIComponent(nextMode)).then(() => loadData());
      }
      function setTurnMode(nextMode) {
        fetch('/turn-mode?set=' + encodeURIComponent(nextMode)).then(() => loadData());
      }
      function toggleTurnMode() {
        if (mode !== 'MANUAL') return;
        setTurnMode(turnMode === 'PIVOT' ? 'SPIN' : 'PIVOT');
      }
      function sendCmd(move) {
        return fetch('/cmd?move=' + encodeURIComponent(move)).then(() => loadData()).catch(() => {});
      }
      function startMove(move) {
        if (mode !== 'MANUAL') return;
        activePress = true;
        minStopAt = Date.now() + 140;
        if (releaseStopTimer) clearTimeout(releaseStopTimer);
        sendCmd(move);
      }
      function stopMove() {
        if (!activePress) return;
        activePress = false;
        const wait = Math.max(0, minStopAt - Date.now());
        if (releaseStopTimer) clearTimeout(releaseStopTimer);
        releaseStopTimer = setTimeout(() => sendCmd('STOP'), wait);
      }
      function explicitStop() {
        activePress = false;
        if (releaseStopTimer) clearTimeout(releaseStopTimer);
        sendCmd('STOP');
      }
      function updateModeButtons() {
        document.getElementById('autoBtn').classList.toggle('active', mode === 'AUTO');
        document.getElementById('manualBtn').classList.toggle('active', mode === 'MANUAL');
        document.getElementById('dpad').classList.toggle('auto', mode !== 'MANUAL');
      }
      function updateTurnModeButton() {
        const btn = document.getElementById('turnModeBtn');
        btn.textContent = 'Turn Mode: ' + turnMode;
        btn.classList.toggle('spin', turnMode === 'SPIN');
      }
      function ensureAudio() {
        if (!audioCtx) {
          audioCtx = new (window.AudioContext || window.webkitAudioContext)();
          alarmOutput = audioCtx.createGain();
          alarmOutput.gain.value = 1.0;
          alarmOutput.connect(audioCtx.destination);
        }
        if (audioCtx.state === 'suspended') {
          audioCtx.resume();
        }
      }
      function setAudioStatus(text) {
        document.getElementById('audioStatus').textContent = text;
      }
      function beep(freq, ms, delayMs = 0, level = 0.26) {
        if (!alarmEnabled || !audioCtx) return;
        ensureAudio();
        const osc = audioCtx.createOscillator();
        const gain = audioCtx.createGain();
        const startAt = audioCtx.currentTime + Math.max(0, delayMs) / 1000;
        const stopAt = startAt + Math.max(40, ms) / 1000;
        osc.frequency.value = freq;
        osc.type = 'sine';
        gain.gain.setValueAtTime(0.0001, startAt);
        gain.gain.exponentialRampToValueAtTime(level, startAt + 0.015);
        gain.gain.setValueAtTime(level, Math.max(startAt + 0.016, stopAt - 0.035));
        gain.gain.exponentialRampToValueAtTime(0.0001, stopAt);
        osc.connect(gain);
        gain.connect(alarmOutput);
        osc.start(startAt);
        osc.stop(stopAt + 0.01);
        osc.onended = () => {
          osc.disconnect();
          gain.disconnect();
        };
      }
      function urgentDoubleBeep() {
        beep(980, 150, 0, 0.30);
        beep(760, 170, 240, 0.30);
      }
      function vibrateHighRisk() {
        if (navigator.vibrate) navigator.vibrate([120, 80, 120]);
      }
      function stopAlarmTimers() {
        if (highRiskTimer) {
          clearInterval(highRiskTimer);
          highRiskTimer = null;
        }
        if (highRiskVibeTimer) {
          clearInterval(highRiskVibeTimer);
          highRiskVibeTimer = null;
        }
        if (navigator.vibrate) navigator.vibrate(0);
      }
      function enableAlarm(event) {
        if (event) event.preventDefault();
        ensureAudio();
        alarmEnabled = true;
        alarmMuted = false;
        if (alarmOutput && audioCtx) {
          alarmOutput.gain.cancelScheduledValues(audioCtx.currentTime);
          alarmOutput.gain.setValueAtTime(1.0, audioCtx.currentTime);
        }
        setAudioStatus('AUDIO ENABLED');
        beep(900, 250, 0, 0.30);
      }
      function testBeep(event) {
        if (event) event.preventDefault();
        if (!alarmEnabled || alarmMuted) return;
        ensureAudio();
        beep(900, 250, 0, 0.30);
      }
      function disableAlarm(event) {
        if (event) event.preventDefault();
        alarmEnabled = false;
        alarmMuted = true;
        stopAlarmTimers();
        if (alarmOutput && audioCtx) {
          alarmOutput.gain.cancelScheduledValues(audioCtx.currentTime);
          alarmOutput.gain.setValueAtTime(0.0, audioCtx.currentTime);
        }
        setAudioStatus('ALARM MUTED');
      }
      function updateAlarm(status) {
        const indicator = document.getElementById('alarmIndicator');
        indicator.textContent = 'ALARM: ' + status;
        indicator.className = 'alarm-indicator';
        if (status === 'CAUTION') indicator.classList.add('caution');
        if (status === 'HIGH RISK') indicator.classList.add('high');

        if (status === 'SAFE') {
          stopAlarmTimers();
          return;
        }
        if (!alarmEnabled || alarmMuted) return;
        ensureAudio();
        if (status === 'CAUTION') {
          stopAlarmTimers();
          if (Date.now() - lastCautionBeep > 5000) {
            lastCautionBeep = Date.now();
            beep(560, 160, 0, 0.25);
          }
        } else if (status === 'HIGH RISK' && !highRiskTimer) {
          urgentDoubleBeep();
          vibrateHighRisk();
          highRiskTimer = setInterval(() => {
            urgentDoubleBeep();
          }, 900);
          highRiskVibeTimer = setInterval(() => {
            vibrateHighRisk();
          }, 1200);
        }
      }
      function loadData() {
        fetch('/data').then(r => r.json()).then(d => {
          document.getElementById('temp').textContent = d.temperature + ' C';
          document.getElementById('hum').textContent = d.humidity + ' %';
          document.getElementById('gas').textContent = d.gasValue;
          document.getElementById('gasStatus').textContent = d.gasStatus;
          document.getElementById('dist').textContent = d.distance + ' cm';
          document.getElementById('motor').textContent = d.motorState;
          document.getElementById('nav').textContent = d.navigationDecision;
          document.getElementById('mode').textContent = d.mode;
          document.getElementById('turnMode').textContent = d.turnMode;
          mode = d.mode;
          turnMode = d.turnMode;
          updateModeButtons();
          updateTurnModeButton();
          updateAlarm(d.gasStatus);
        });
      }
      document.querySelectorAll('.hold').forEach(btn => {
        btn.addEventListener('pointerdown', e => {
          e.preventDefault();
          startMove(btn.dataset.move);
        });
        btn.addEventListener('pointerup', e => {
          e.preventDefault();
          stopMove();
        });
        btn.addEventListener('pointercancel', stopMove);
        btn.addEventListener('pointerleave', stopMove);
        btn.addEventListener('touchend', e => {
          e.preventDefault();
          stopMove();
        }, {passive:false});
      });
      document.getElementById('stopBtn').addEventListener('pointerdown', e => {
        e.preventDefault();
        explicitStop();
      });
      setInterval(loadData, 750);
      loadData();
    </script>
  </body>
</html>
)html";
  _server.send(200, "text/html", html);
}

void WebDashboard::handleData() {
  _server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  _server.sendHeader("Pragma", "no-cache");
  _server.sendHeader("Expires", "0");
  sendJson();
}

void WebDashboard::handleMode() {
  if (!_server.hasArg("set")) {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  String mode = _server.arg("set");
  mode.toUpperCase();
  if (mode != "AUTO" && mode != "MANUAL") {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  applyMode(mode);
  sendOk();
}

void WebDashboard::handleTurnMode() {
  if (!_server.hasArg("set")) {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  String mode = _server.arg("set");
  mode.toUpperCase();
  if (mode != "PIVOT" && mode != "SPIN") {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  applyTurnMode(mode);
  sendOk();
}

void WebDashboard::handleCommand() {
  if (!_server.hasArg("move")) {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  String command = _server.arg("move");
  command.toUpperCase();
  if (command != "FORWARD" && command != "BACKWARD" && command != "LEFT" &&
      command != "RIGHT" && command != "STOP") {
    _server.send(400, "text/plain", "ERR");
    return;
  }

  applyCommand(command);
  sendOk();
}

void WebDashboard::handleModeAuto() {
  applyMode("AUTO");
  sendOk();
}

void WebDashboard::handleModeManual() {
  applyMode("MANUAL");
  sendOk();
}

void WebDashboard::handleControlForward() {
  applyCommand("FORWARD");
  sendOk();
}

void WebDashboard::handleControlBackward() {
  applyCommand("BACKWARD");
  sendOk();
}

void WebDashboard::handleControlLeft() {
  applyCommand("LEFT");
  sendOk();
}

void WebDashboard::handleControlRight() {
  applyCommand("RIGHT");
  sendOk();
}

void WebDashboard::handleControlStop() {
  applyCommand("STOP");
  sendOk();
}

void WebDashboard::applyMode(const String& mode) {
  _data.controlMode = mode;
  _pendingCommand = "";
}

void WebDashboard::applyTurnMode(const String& mode) {
  _data.turnMode = mode;
}

void WebDashboard::applyCommand(const String& command) {
  if (_data.controlMode == "MANUAL") {
    _pendingCommand = command;
  }
}

void WebDashboard::sendOk() {
  _server.send(200, "text/plain", "OK");
}

void WebDashboard::sendJson() {
  String json = "{\"mode\":\"" + _data.controlMode + "\"" +
                ",\"turnMode\":\"" + _data.turnMode + "\"" +
                ",\"distance\":" + String(_data.frontDistance, 1) +
                ",\"temperature\":" + String(_data.temperature, 1) +
                ",\"humidity\":" + String(_data.humidity, 1) +
                ",\"gasValue\":" + String(_data.gasValue) +
                ",\"gasStatus\":\"" + _data.gasStatus + "\"" +
                ",\"motorState\":\"" + _data.motorState + "\"" +
                ",\"navigationDecision\":\"" + _data.navigationDecision + "\"}";
  _server.send(200, "application/json", json);
}
