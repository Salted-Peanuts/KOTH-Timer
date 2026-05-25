// ---- IMPORTANT: define CaptureState BEFORE Arduino auto-prototypes ----
#include <stdint.h>
enum CaptureState : uint8_t { ST_READY, ST_IDLE, ST_A, ST_B, ST_CONTESTED, ST_DONE };
// ----------------------------------------------------------------------

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Preferences.h>
#include <TM1637Display.h>

/*
  Arduino Nano ESP32 (NORA-W106) - King of the Hill Timer
  FULL CODE (current spec)

  Game rules:
  - Only counts down while EXACTLY ONE team button is held
  - Both held => contested (no score)
  - None held => no score
  - Match ends when either team timer reaches 0

  UI requirements:
  - Team names derived from chosen team colours (no A/B)
  - No stop button
  - Reset button DISABLED unless game is paused OR match is finished
  - When match is over, show which team won
  - "Identify teams" button (only works when stopped): flashes displays + button LEDs

  Hardware:
  - 4x TM1637 displays (2 per team)
  - 2x arcade buttons + their LEDs (GPIO driven)
  - Battery ADC divider + bargraph indicator with separate "show battery" button
  - Bargraph wiring: common GND, each segment -> resistor -> GPIO (GPIO HIGH = ON)

  Battery smoothing:
  - Uses oversampling + EMA low-pass filter for stable voltage readout.
*/

//// ===== Wi-Fi AP =====
static const char* AP_SSID = "KOTH-Timer";
static const char* AP_PASS = ""; // open AP

//// ===== Pins (Arduino Nano ESP32 labels) =====
// Buttons (to GND, INPUT_PULLUP)
static const int PIN_BTN_A   = D2;
static const int PIN_BTN_B   = D3;
static const int PIN_BTN_BAT = D4;

// Arcade button LEDs (GPIO -> resistor -> LED -> GND). HIGH = ON
static const int PIN_LED_A   = D5;
static const int PIN_LED_B   = D7;

// TM1637 (VCC -> VIN, GND -> GND)
static const int PIN_TM_CLK  = D8;
static const int PIN_TM_A1   = D9;
static const int PIN_TM_A2   = D10;
static const int PIN_TM_B1   = D11;
static const int PIN_TM_B2   = D6;   // moved off D12 (unreliable)

// Battery ADC divider midpoint
static const int PIN_BATT_ADC = A0;

// 5-bar battery indicator (common GND; GPIO HIGH = ON)
static const int PIN_BAR_RED    = A1;
static const int PIN_BAR_YELLOW = A2;
static const int PIN_BAR_G1     = A3;
static const int PIN_BAR_G2     = A4;
static const int PIN_BAR_G3     = A5;

//// ===== Battery divider (set to your actual divider values) =====
static const float R1 = 100000.0f;
static const float R2 = 100000.0f;

//// ===== Battery calibration knobs =====
// Tune with multimeter.
// Example: multimeter 4.12V, UI shows 3.95V => ADC_GAIN = 4.12/3.95 = 1.043
static const float ADC_GAIN   = 0.920f;
static const float ADC_OFFSET = 0.000f; // volts

//// ===== Battery smoothing knobs =====
// EMA_ALPHA: 0.05 = very smooth/slow, 0.20 = faster response
static const float EMA_ALPHA = 0.12f;
// Update period for the EMA input sample
static const uint32_t BATT_UPDATE_MS = 250;
// Oversampling per raw read
static const int BATT_SAMPLES = 24;

//// ===== Debounce =====
struct DebouncedButton {
  int pin;
  bool stable;      // HIGH not pressed, LOW pressed
  bool lastReading;
  uint32_t lastChangeMs;
};
static const uint32_t DEBOUNCE_MS = 25;

static DebouncedButton btnA  {PIN_BTN_A,   true, true, 0};
static DebouncedButton btnB  {PIN_BTN_B,   true, true, 0};
static DebouncedButton btnBt {PIN_BTN_BAT, true, true, 0};

static inline void readDebounced(DebouncedButton &b) {
  bool r = digitalRead(b.pin);
  if (r != b.lastReading) {
    b.lastReading = r;
    b.lastChangeMs = millis();
  }
  if (millis() - b.lastChangeMs > DEBOUNCE_MS) {
    b.stable = r;
  }
}
static inline bool isPressed(const DebouncedButton &b) { return b.stable == LOW; }

//// ===== State / Config =====
static Preferences prefs;

static uint32_t durationSec = 5 * 60;
static int32_t remainingA = 5 * 60;
static int32_t remainingB = 5 * 60;

static bool started = false;
static bool paused  = false;

static String colorA = "#ff3333";
static String colorB = "#3333ff";

static CaptureState captureState = ST_READY;

static uint32_t lastTickMs = 0;
static uint32_t accumMs = 0;

//// ===== Identify mode =====
static bool identifyMode = false;
static uint32_t identifyUntilMs = 0;

//// ===== Battery / bargraph =====
static float battVolts = 0.0f;     // SMOOTHED
static float battVoltsRaw = 0.0f;  // instantaneous (for debugging if you want)
static int battPct = 0;

static bool barActive = false;
static uint32_t barOffAtMs = 0;
static const uint32_t BAR_SHOW_MS = 5000;

//// ===== Displays =====
static TM1637Display dispA1(PIN_TM_CLK, PIN_TM_A1);
static TM1637Display dispA2(PIN_TM_CLK, PIN_TM_A2);
static TM1637Display dispB1(PIN_TM_CLK, PIN_TM_B1);
static TM1637Display dispB2(PIN_TM_CLK, PIN_TM_B2);

//// ===== Web =====
static WebServer server(80);
static WebSocketsServer ws(81);
static uint32_t lastBroadcastMs = 0;

//// ===== Helpers =====
static String formatMMSS(int32_t s) {
  if (s < 0) s = 0;
  int32_t m = s / 60;
  int32_t r = s % 60;
  char buf[8];
  snprintf(buf, sizeof(buf), "%02ld:%02ld", (long)m, (long)r);
  return String(buf);
}
static uint16_t encodeMMSS(int32_t s) {
  if (s < 0) s = 0;
  int mm = (int)(s / 60);
  int ss = (int)(s % 60);
  return (uint16_t)(mm * 100 + ss);
}
static void resetTimers() {
  remainingA = (int32_t)durationSec;
  remainingB = (int32_t)durationSec;
}

static void loadConfig() {
  prefs.begin("koth", true);
  durationSec = prefs.getUInt("dur", 5 * 60);
  colorA = prefs.getString("colA", "#ff3333");
  colorB = prefs.getString("colB", "#3333ff");
  prefs.end();

  if (durationSec < 60) durationSec = 60;
  if (durationSec > 60 * 60) durationSec = 60 * 60;
}
static void saveConfig() {
  prefs.begin("koth", false);
  prefs.putUInt("dur", durationSec);
  prefs.putString("colA", colorA);
  prefs.putString("colB", colorB);
  prefs.end();
}

// Leading zero suppression on TM1637 when < 10 minutes (600 seconds)
static void showTimeOn(TM1637Display &d, int32_t seconds, uint8_t colonMask) {
  uint16_t v = encodeMMSS(seconds);
  bool leadingZero = (seconds >= 600); // show leading 0 only for 10:00 and above
  d.showNumberDecEx(v, colonMask, leadingZero);
}
static void updateDisplays() {
  const uint8_t colonMask = 0b01000000;
  showTimeOn(dispA1, remainingA, colonMask);
  showTimeOn(dispA2, remainingA, colonMask);
  showTimeOn(dispB1, remainingB, colonMask);
  showTimeOn(dispB2, remainingB, colonMask);
}

// Winner string used by UI
static String winnerString() {
  // Match ends when either team reaches 0
  if (remainingA <= 0 && remainingB <= 0) return "DRAW";
  if (remainingA <= 0) return "TEAM_A";
  if (remainingB <= 0) return "TEAM_B";
  return "";
}

// Battery conversion
static float readBattVoltsInstant() {
  uint32_t sum = 0;
  for (int i = 0; i < BATT_SAMPLES; i++) {
    sum += analogRead(PIN_BATT_ADC);
    delay(2);
  }
  float raw = (float)sum / (float)BATT_SAMPLES;

  float v_adc  = (raw / 4095.0f) * 3.3f;
  float v_batt = v_adc * ((R1 + R2) / R2);

  // Calibration knobs
  v_batt = (v_batt * ADC_GAIN) + ADC_OFFSET;

  return v_batt;
}

static int voltsToPct(float v) {
  if (v >= 4.20f) return 100;
  if (v <= 3.30f) return 0;
  if (v >= 4.00f) return (int)(80 + (v - 4.00f) * (20.0f / 0.20f));
  if (v >= 3.85f) return (int)(55 + (v - 3.85f) * (25.0f / 0.15f));
  if (v >= 3.70f) return (int)(30 + (v - 3.70f) * (25.0f / 0.15f));
  if (v >= 3.50f) return (int)(10 + (v - 3.50f) * (20.0f / 0.20f));
  return (int)((v - 3.30f) * (10.0f / 0.20f));
}
static int voltsToBars(float v) {
  if (v >= 4.10f) return 5;
  if (v >= 3.95f) return 4;
  if (v >= 3.80f) return 3;
  if (v >= 3.65f) return 2;
  if (v >= 3.45f) return 1;
  return 0;
}

// Bargraph outputs: HIGH=ON, LOW=OFF (your wiring)
static void barAllOff() {
  digitalWrite(PIN_BAR_RED, LOW);
  digitalWrite(PIN_BAR_YELLOW, LOW);
  digitalWrite(PIN_BAR_G1, LOW);
  digitalWrite(PIN_BAR_G2, LOW);
  digitalWrite(PIN_BAR_G3, LOW);
}
static void barShowBars(int bars) {
  digitalWrite(PIN_BAR_RED,    (bars >= 1) ? HIGH : LOW);
  digitalWrite(PIN_BAR_YELLOW, (bars >= 2) ? HIGH : LOW);
  digitalWrite(PIN_BAR_G1,     (bars >= 3) ? HIGH : LOW);
  digitalWrite(PIN_BAR_G2,     (bars >= 4) ? HIGH : LOW);
  digitalWrite(PIN_BAR_G3,     (bars >= 5) ? HIGH : LOW);
}
static void triggerBarShow() {
  barActive = true;
  barOffAtMs = millis() + BAR_SHOW_MS;
}

static void evaluateCaptureState(bool aHeld, bool bHeld) {
  if (remainingA <= 0 || remainingB <= 0) { captureState = ST_DONE; return; }
  if (!started) { captureState = ST_READY; return; }
  if (paused) { captureState = ST_IDLE; return; }

  if (aHeld && !bHeld) captureState = ST_A;
  else if (bHeld && !aHeld) captureState = ST_B;
  else if (aHeld && bHeld) captureState = ST_CONTESTED;
  else captureState = ST_IDLE;
}

static const char* stateToStr(CaptureState s) {
  switch (s) {
    case ST_READY:     return "READY";
    case ST_IDLE:      return "IDLE";
    case ST_A:         return "TEAM_A";
    case ST_B:         return "TEAM_B";
    case ST_CONTESTED: return "CONTESTED";
    case ST_DONE:      return "DONE";
    default:           return "IDLE";
  }
}

static String makeStateJson() {
  String win = winnerString();

  String json = "{";
  json += "\"a\":\"" + formatMMSS(remainingA) + "\",";
  json += "\"b\":\"" + formatMMSS(remainingB) + "\",";
  json += "\"status\":\"" + String(stateToStr(captureState)) + "\",";
  json += "\"started\":" + String(started ? "true" : "false") + ",";
  json += "\"paused\":" + String(paused ? "true" : "false") + ",";
  json += "\"dur\":" + String(durationSec) + ",";
  json += "\"colA\":\"" + colorA + "\",";
  json += "\"colB\":\"" + colorB + "\",";
  json += "\"bv\":" + String(battVolts, 2) + ",";
  json += "\"bp\":" + String(battPct) + ",";
  json += "\"identify\":" + String(identifyMode ? "true" : "false") + ",";
  json += "\"winner\":\"" + win + "\"";
  json += "}";
  return json;
}

static void broadcastState() {
  String payload = makeStateJson(); // lvalue needed by WebSockets 2.7.2
  ws.broadcastTXT(payload);
}

//// ===== UI (no Wi-Fi strength; reset lock; winner shown) =====
static const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<title>KOTH Timer</title>
<style>
:root{--a:#ff3333;--b:#3333ff}
body{margin:0;padding:16px;font-family:system-ui;background:#0f0f10;color:#eee}
.wrap{max-width:960px;margin:0 auto}
h2{margin:0 0 12px 0}
.row{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.card{border-radius:16px;padding:16px;background:#17171a;border:1px solid #2a2a2f}
.team{font-size:16px;opacity:.85;margin-bottom:8px;display:flex;align-items:center;gap:10px}
.dot{width:12px;height:12px;border-radius:50%}
.time{font-size:60px;font-weight:900}
.status{margin:12px 0;padding:14px;border-radius:14px;background:#141416;border:1px solid #2a2a2f;font-size:18px}
.controls{display:flex;flex-wrap:wrap;gap:10px}
button{border:0;border-radius:12px;padding:12px 16px;font-size:16px;font-weight:800;background:#2a2a2f;color:#eee;cursor:pointer}
button.primary{background:#3a3a44}
button:disabled{opacity:.35;cursor:not-allowed}
input[type=number]{width:120px;border-radius:10px;border:1px solid #333;padding:8px;font-size:16px;background:#0f0f10;color:#eee}
input[type=color]{width:50px;height:40px;border:0;background:transparent}
.small{font-size:12px;opacity:.6;margin-top:10px}
.mono{font-variant-numeric:tabular-nums}
.pill{display:inline-block;padding:8px 10px;border-radius:999px;background:#101014;border:1px solid #2a2a2f;font-size:13px;opacity:.85}
.pills{display:flex;flex-wrap:wrap;gap:10px;align-items:center;margin-top:10px}
</style>
</head>
<body>
<div class="wrap">

<h2>King of the Hill Timer</h2>

<div class="row">
  <div class="card">
    <div class="team">
      <span class="dot" id="dotA" style="background:var(--a)"></span>
      <span id="labelA">Team</span>
    </div>
    <div class="time mono" id="timeA">00:00</div>
  </div>

  <div class="card">
    <div class="team">
      <span class="dot" id="dotB" style="background:var(--b)"></span>
      <span id="labelB">Team</span>
    </div>
    <div class="time mono" id="timeB">00:00</div>
  </div>
</div>

<div class="status" id="status">Connecting...</div>

<div class="card">
  <div class="controls">
    <button class="primary" onclick="cmd('start')">Start</button>
    <button onclick="cmd('pause')">Pause</button>
    <button onclick="cmd('resume')">Resume</button>
    <button id="resetBtn" onclick="cmd('reset')">Reset</button>
    <button onclick="cmd('identify')">Identify teams</button>
  </div>

  <div class="pills">
    <span class="pill mono" id="batt">Battery: --</span>
    <span class="pill" id="state">State: --</span>
  </div>

  <div style="height:12px"></div>

  <div class="controls">
    <label>Duration
      <input id="mins" type="number" min="1" max="60" value="5"/>
    </label>
    <button class="primary" onclick="applyDuration()">Apply</button>
  </div>

  <div style="height:12px"></div>

  <div class="controls">
    <label>A <input id="colA" type="color" value="#ff3333"/></label>
    <label>B <input id="colB" type="color" value="#3333ff"/></label>
    <button class="primary" onclick="applyColors()">Apply colours</button>
  </div>

  <div class="small">Reset only works when paused or after match ends.</div>
</div>

</div>

<script>
let ws;

function hexToRgb(hex){
  if(!hex) return null;
  const h = (""+hex).replace("#","").trim();
  if(h.length !== 6) return null;
  const n = parseInt(h, 16);
  if(Number.isNaN(n)) return null;
  return {r:(n>>16)&255, g:(n>>8)&255, b:n&255};
}
function rgbToHsl(r,g,b){
  r/=255; g/=255; b/=255;
  const max=Math.max(r,g,b), min=Math.min(r,g,b);
  let h=0, s=0, l=(max+min)/2;
  const d=max-min;
  if(d){
    s=d/(1-Math.abs(2*l-1));
    switch(max){
      case r: h=((g-b)/d)%6; break;
      case g: h=(b-r)/d+2; break;
      case b: h=(r-g)/d+4; break;
    }
    h*=60; if(h<0) h+=360;
  }
  return {h,s,l};
}
// Robust: derive a name from hue bucket
function teamName(hex){
  const rgb = hexToRgb(hex);
  if(!rgb) return "Team";
  const hsl = rgbToHsl(rgb.r, rgb.g, rgb.b);
  if(hsl.s < 0.18 || hsl.l < 0.12 || hsl.l > 0.92) return "Neutral Team";
  const h = hsl.h;
  if(h < 15 || h >= 345) return "Red Team";
  if(h < 45) return "Orange Team";
  if(h < 70) return "Yellow Team";
  if(h < 160) return "Green Team";
  if(h < 200) return "Cyan Team";
  if(h < 255) return "Blue Team";
  if(h < 300) return "Purple Team";
  return "Pink Team";
}

function setCSS(a,b){
  document.documentElement.style.setProperty('--a',a);
  document.documentElement.style.setProperty('--b',b);
  document.getElementById('dotA').style.background = a;
  document.getElementById('dotB').style.background = b;
}

function connect(){
  ws = new WebSocket(`ws://${location.hostname}:81/`);

  ws.onopen = () => {
    document.getElementById('status').textContent = "Connected";
  };

  ws.onclose = () => {
    document.getElementById('status').textContent = "Disconnected (retrying)";
    setTimeout(connect, 800);
  };

  ws.onerror = () => { try{ ws.close(); }catch(e){} };

  ws.onmessage = (ev) => {
    let d=null;
    try{ d = JSON.parse(ev.data); }catch(e){ return; }
    if(!d) return;

    // Times
    if(typeof d.a === "string") document.getElementById('timeA').textContent = d.a;
    if(typeof d.b === "string") document.getElementById('timeB').textContent = d.b;

    // Colours + team labels
    if(typeof d.colA === "string" && typeof d.colB === "string"){
      setCSS(d.colA, d.colB);
      document.getElementById('labelA').textContent = teamName(d.colA);
      document.getElementById('labelB').textContent = teamName(d.colB);
    }

    // Battery
    if(d.bp !== undefined && d.bv !== undefined){
      const bp = Number(d.bp);
      const bv = Number(d.bv);
      if(Number.isFinite(bp) && Number.isFinite(bv)){
        document.getElementById('batt').textContent = `Battery: ${Math.round(bp)}% (${bv.toFixed(2)}V)`;
      }
    }

    // Reset lock
    const resetBtn = document.getElementById('resetBtn');
    const paused = !!d.paused;
    const finished = (d.winner === "TEAM_A" || d.winner === "TEAM_B" || d.winner === "DRAW");
    resetBtn.disabled = !(paused || finished);

    // State pill
    document.getElementById('state').textContent =
      `State: ${d.started ? "started" : "stopped"}${paused ? ", paused" : ""}`;

    // Status line (winner + capture)
    const nameA = document.getElementById('labelA').textContent || "Team A";
    const nameB = document.getElementById('labelB').textContent || "Team B";

    let s = "Ready";

    if(d.identify){
      s = "Identifying teams";
    }
    else if(d.winner === "TEAM_A"){
      s = `${nameA} wins!`;
    }
    else if(d.winner === "TEAM_B"){
      s = `${nameB} wins!`;
    }
    else if(d.winner === "DRAW"){
      s = "Draw!";
    }
    else if(d.status === "TEAM_A"){
      s = `${nameA} capturing`;
    }
    else if(d.status === "TEAM_B"){
      s = `${nameB} capturing`;
    }
    else if(d.status === "CONTESTED"){
      s = "Contested (no score)";
    }
    else if(d.status === "IDLE"){
      s = d.started ? (paused ? "Paused" : "No capture") : "Stopped";
    }
    else if(d.status === "READY"){
      s = "Ready";
    }
    else if(d.status === "DONE"){
      s = "Match complete";
    }

    document.getElementById('status').textContent = s;
  };
}

function cmd(c){
  fetch(`/cmd?c=${encodeURIComponent(c)}`).catch(()=>{});
}
function applyDuration(){
  const m = parseInt(document.getElementById('mins').value || "5", 10);
  fetch(`/set?dur=${encodeURIComponent(m)}`).catch(()=>{});
}
function applyColors(){
  const a = document.getElementById('colA').value;
  const b = document.getElementById('colB').value;
  fetch(`/set?colA=${encodeURIComponent(a)}&colB=${encodeURIComponent(b)}`).catch(()=>{});
}

connect();
</script>
</body>
</html>
)HTML";

static void handleRoot(){ server.send_P(200, "text/html", INDEX_HTML); }

static void handleConfig(){
  String json="{";
  json += "\"dur\":" + String(durationSec) + ",";
  json += "\"colA\":\"" + colorA + "\",";
  json += "\"colB\":\"" + colorB + "\"}";
  server.send(200, "application/json", json);
}

static void handleSet(){
  if(server.hasArg("dur")){
    int mins = server.arg("dur").toInt();
    if(mins < 1) mins = 1;
    if(mins > 60) mins = 60;
    durationSec = (uint32_t)mins * 60;
  }
  if(server.hasArg("colA")) colorA = server.arg("colA");
  if(server.hasArg("colB")) colorB = server.arg("colB");

  saveConfig();
  if(!started) resetTimers();
  server.send(200, "text/plain", "OK");
}

static void handleCmd(){
  String c = server.arg("c");

  if(c=="start"){
    started = true;
    paused  = false;
  }
  else if(c=="pause"){
    paused = true;
  }
  else if(c=="resume"){
    paused = false;
  }
  else if(c=="reset"){
    // Only allow reset if paused OR match finished
    if(paused || remainingA <= 0 || remainingB <= 0){
      resetTimers();
      started = false;
      paused  = false;
    }
  }
  else if(c=="identify" && !started){
    identifyMode = true;
    identifyUntilMs = millis() + 5000;
  }

  server.send(200, "text/plain", "OK");
}

static void onWsEvent(uint8_t num, WStype_t type, uint8_t*, size_t){
  if(type == WStype_CONNECTED){
    String p = makeStateJson();
    ws.sendTXT(num, p);
  }
}

//// ===== Setup/Loop =====
void setup(){
  pinMode(PIN_BTN_A, INPUT_PULLUP);
  pinMode(PIN_BTN_B, INPUT_PULLUP);
  pinMode(PIN_BTN_BAT, INPUT_PULLUP);

  pinMode(PIN_LED_A, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  digitalWrite(PIN_LED_A, LOW);
  digitalWrite(PIN_LED_B, LOW);

  pinMode(PIN_BAR_RED, OUTPUT);
  pinMode(PIN_BAR_YELLOW, OUTPUT);
  pinMode(PIN_BAR_G1, OUTPUT);
  pinMode(PIN_BAR_G2, OUTPUT);
  pinMode(PIN_BAR_G3, OUTPUT);
  barAllOff();

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_BATT_ADC, ADC_11db);

  loadConfig();
  resetTimers();

  dispA1.setBrightness(7, true);
  dispA2.setBrightness(7, true);
  dispB1.setBrightness(7, true);
  dispB2.setBrightness(7, true);
  updateDisplays();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, (strlen(AP_PASS)>0 ? AP_PASS : NULL));

  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/set", handleSet);
  server.on("/cmd", handleCmd);
  server.begin();

  ws.begin();
  ws.onEvent(onWsEvent);

  // Initialise battery filter with a first reading so it doesn't ramp from 0
  battVoltsRaw = readBattVoltsInstant();
  battVolts = battVoltsRaw;
  battPct = voltsToPct(battVolts);

  lastTickMs = millis();
  lastBroadcastMs = 0;
}

void loop(){
  server.handleClient();
  ws.loop();

  const uint32_t now = millis();

  // ---- Smooth battery update ----
  static uint32_t lastBattMs = 0;
  if(now - lastBattMs >= BATT_UPDATE_MS){
    battVoltsRaw = readBattVoltsInstant();
    battVolts = battVolts + EMA_ALPHA * (battVoltsRaw - battVolts); // EMA smoothing
    battPct = voltsToPct(battVolts);
    lastBattMs = now;
  }

  // ---- Battery bar button ----
  readDebounced(btnBt);
  static bool lastBat = false;
  bool batNow = isPressed(btnBt);
  if(batNow && !lastBat) triggerBarShow();
  lastBat = batNow;

  if(barActive){
    barShowBars(voltsToBars(battVolts));
    if((int32_t)(now - barOffAtMs) >= 0){
      barActive = false;
      barAllOff();
    }
  } else {
    barAllOff();
  }

  // ---- Identify mode (stopped only): flash displays + LEDs ----
  if(identifyMode){
    if((int32_t)(now - identifyUntilMs) >= 0){
      identifyMode = false;
      dispA1.clear(); dispA2.clear(); dispB1.clear(); dispB2.clear();
      digitalWrite(PIN_LED_A, LOW);
      digitalWrite(PIN_LED_B, LOW);
    } else {
      bool blink = (now / 400) % 2;

      if(blink){
        dispA1.showNumberDec(1111, false);
        dispA2.showNumberDec(1111, false);
        dispB1.showNumberDec(2222, false);
        dispB2.showNumberDec(2222, false);
        digitalWrite(PIN_LED_A, HIGH);
        digitalWrite(PIN_LED_B, HIGH);
      } else {
        dispA1.clear(); dispA2.clear(); dispB1.clear(); dispB2.clear();
        digitalWrite(PIN_LED_A, LOW);
        digitalWrite(PIN_LED_B, LOW);
      }

      // keep UI live
      if(now - lastBroadcastMs >= 150){
        broadcastState();
        lastBroadcastMs = now;
      }
      return; // skip normal game logic during identify
    }
  }

  // ---- Read team buttons ----
  readDebounced(btnA);
  readDebounced(btnB);

  bool aHeld = isPressed(btnA);
  bool bHeld = isPressed(btnB);

  // ---- Evaluate capture state ----
  evaluateCaptureState(aHeld, bHeld);

  // ---- Arcade button LED logic ----
  if(!started){
    digitalWrite(PIN_LED_A, LOW);
    digitalWrite(PIN_LED_B, LOW);
  } else if(captureState == ST_A){
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, LOW);
  } else if(captureState == ST_B){
    digitalWrite(PIN_LED_A, LOW);
    digitalWrite(PIN_LED_B, HIGH);
  } else if(captureState == ST_CONTESTED){
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
  } else {
    digitalWrite(PIN_LED_A, LOW);
    digitalWrite(PIN_LED_B, LOW);
  }

  // ---- Countdown tick ----
  uint32_t elapsedMs = now - lastTickMs;
  lastTickMs = now;

  if(started && !paused && (captureState == ST_A || captureState == ST_B)){
    accumMs += elapsedMs;
    while(accumMs >= 1000){
      accumMs -= 1000;
      if(captureState == ST_A){
        if(remainingA > 0) remainingA--;
      } else {
        if(remainingB > 0) remainingB--;
      }
    }
  } else {
    accumMs = 0;
  }

  // ---- Update display ----
  static uint32_t lastDispMs = 0;
  if(now - lastDispMs >= 100){
    updateDisplays();
    lastDispMs = now;
  }

  // ---- Broadcast to UI ----
  if(now - lastBroadcastMs >= 150){
    broadcastState();
    lastBroadcastMs = now;
  }
}
