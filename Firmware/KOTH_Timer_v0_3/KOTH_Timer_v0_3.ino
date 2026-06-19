// ---- IMPORTANT: define CaptureState BEFORE Arduino auto-prototypes ----
#include <stdint.h>
enum CaptureState : uint8_t { ST_READY, ST_IDLE, ST_A, ST_B, ST_CONTESTED, ST_DONE };
// ----------------------------------------------------------------------

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Preferences.h>
#include <TM1637Display.h>
#include <DNSServer.h>

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

// ===== Wi-Fi AP =====
static const char* AP_SSID = "KOTH-Timer";
static const char* AP_PASS = ""; 
// Leave blank for open Wi-Fi.
// Optional: use "kothtimer" if you want a password later.
// Password must be at least 8 characters if used.

static const uint8_t AP_CHANNEL = 1;
static const uint8_t AP_MAX_CLIENTS = 3;
static const bool AP_HIDDEN = false;

static IPAddress AP_IP(10, 10, 10, 1);
static IPAddress AP_GATEWAY(10, 10, 10, 1);
static IPAddress AP_SUBNET(255, 255, 255, 0);

static const byte DNS_PORT = 53;
static DNSServer dnsServer;

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
static const uint32_t BATT_UPDATE_MS = 1000;
// Oversampling per raw read
static const int BATT_SAMPLES = 8;

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

static const int32_t MAX_REMAINING_SEC = (99 * 60) + 59; // TM1637 MM:SS practical max

static int32_t clampRemainingTime(int32_t seconds) {
  if (seconds < 0) return 0;
  if (seconds > MAX_REMAINING_SEC) return MAX_REMAINING_SEC;
  return seconds;
}

static void adjustTeamTime(const String &team, int32_t deltaSec) {
  if (team == "A") {
    remainingA = clampRemainingTime(remainingA + deltaSec);
  } else if (team == "B") {
    remainingB = clampRemainingTime(remainingB + deltaSec);
  }

  // Prevent an immediate extra second being removed straight after adjustment.
  accumMs = 0;
  lastTickMs = millis();

  updateDisplays();
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
    delay(1);
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
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#101216">
<title>KOTH Timer</title>
<style>
:root{
  --bg:#101216;
  --panel:#1a1f28;
  --panel2:#232a36;
  --line:#343c4b;
  --text:#f5f7fb;
  --muted:#aab3c2;
  --a:#ff3333;
  --b:#3333ff;
  --aText:#fff;
  --bText:#fff;
  --blue:#4aa3ff;
  --green:#35d07f;
  --yellow:#f3c84b;
  --red:#ff6464;
}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html,body{margin:0;min-height:100%;background:var(--bg);color:var(--text);font-family:system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif}
body{padding:env(safe-area-inset-top) 0 env(safe-area-inset-bottom)}
.app{width:100%;max-width:520px;margin:0 auto;padding:12px 12px 24px}
.top{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:10px}
h1{font-size:1.15rem;margin:0;font-weight:900;letter-spacing:.02em}
.pill{display:inline-flex;align-items:center;justify-content:center;gap:6px;border:1px solid var(--line);background:var(--panel);color:var(--muted);border-radius:999px;padding:7px 10px;font-size:.78rem;white-space:nowrap}
.pill.good{color:#b8ffd5;border-color:#23653f}
.pill.bad{color:#ffd0d0;border-color:#713434}
.statusBox{border:1px solid var(--line);background:linear-gradient(180deg,var(--panel2),var(--panel));border-radius:18px;padding:14px;margin-bottom:10px;text-align:center}
.statusMain{font-size:1.35rem;font-weight:950;line-height:1.1}
.statusSub{margin-top:5px;color:var(--muted);font-size:.9rem}
.teams{display:grid;gap:10px}
.teamCard{position:relative;overflow:hidden;border:1px solid var(--line);background:var(--panel);border-radius:20px;padding:12px 12px 14px}
.teamCard::before{content:"";position:absolute;left:0;top:0;bottom:0;width:8px;background:var(--team)}
.teamHead{display:flex;align-items:center;justify-content:space-between;margin-left:8px;gap:10px}
.teamName{font-size:1rem;font-weight:900}
.teamHint{font-size:.78rem;color:var(--muted)}
.time{font-variant-numeric:tabular-nums;text-align:center;font-size:4.15rem;line-height:1;font-weight:950;letter-spacing:-.055em;padding:4px 0 0}
.teamCard.active{box-shadow:0 0 0 2px var(--team) inset}
.teamCard.done{opacity:.72}
.grid2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.grid3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px}
.section{border:1px solid var(--line);background:var(--panel);border-radius:20px;padding:12px;margin-top:10px}
.sectionTitle{font-size:1rem;font-weight:900;margin:0 0 10px}
button{width:100%;min-height:54px;border:0;border-radius:15px;background:#303848;color:var(--text);font-size:1rem;font-weight:900;padding:12px 10px;cursor:pointer}
button:active{transform:scale(.98);filter:brightness(1.13)}
button.primary{background:var(--blue);color:#061321}
button.good{background:var(--green);color:#04170c}
button.warn{background:var(--yellow);color:#1f1700}
button.bad{background:var(--red);color:#250000}
button.teamAdjustA{background:var(--a);color:var(--aText)}
button.teamAdjustB{background:var(--b);color:var(--bText)}
button.minus{filter:saturate(.75) brightness(.82)}
button.minus:active{filter:saturate(.75) brightness(1)}
button:disabled{opacity:.35;cursor:not-allowed;transform:none;filter:none}
label{display:block;color:var(--muted);font-size:.82rem;font-weight:750}
input[type=number]{width:100%;min-height:48px;margin-top:6px;border-radius:13px;border:1px solid var(--line);background:#10141b;color:var(--text);font-size:1.1rem;font-weight:850;padding:8px 10px}
input[type=color]{width:100%;height:48px;margin-top:6px;border:1px solid var(--line);border-radius:13px;background:#10141b;padding:4px}
.help{color:var(--muted);font-size:.82rem;line-height:1.35;margin-top:8px}
details{border:1px solid var(--line);background:var(--panel);border-radius:20px;padding:0;margin-top:10px;overflow:hidden}
summary{list-style:none;cursor:pointer;padding:14px 12px;font-size:1rem;font-weight:900}
summary::-webkit-details-marker{display:none}
.detailsInner{padding:0 12px 12px}
.sep{height:10px}
.miniStats{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:10px}
.stat{border:1px solid var(--line);background:#121720;border-radius:14px;padding:9px;text-align:center}
.statLabel{color:var(--muted);font-size:.72rem}
.statValue{font-size:.95rem;font-weight:900;margin-top:2px}
@media (max-width:380px){
  .app{padding-left:9px;padding-right:9px}
  .time{font-size:3.45rem}
  button{font-size:.92rem;min-height:50px}
  .statusMain{font-size:1.15rem}
}
</style>
</head>
<body>
<div class="app">
  <div class="top">
    <h1>KOTH Timer</h1>
    <span class="pill" id="connPill">Connecting</span>
  </div>

  <div class="statusBox">
    <div class="statusMain" id="statusMain">Loading...</div>
    <div class="statusSub" id="statusSub">Connect to KOTH-Timer and open 10.10.10.1</div>
  </div>

  <div class="teams">
    <div class="teamCard" id="cardA" style="--team:var(--a)">
      <div class="teamHead">
        <div>
          <div class="teamName" id="labelA">Team A</div>
          <div class="teamHint">Button A</div>
        </div>
        <span class="pill" id="stateA">Ready</span>
      </div>
      <div class="time" id="timeA">00:00</div>
    </div>

    <div class="teamCard" id="cardB" style="--team:var(--b)">
      <div class="teamHead">
        <div>
          <div class="teamName" id="labelB">Team B</div>
          <div class="teamHint">Button B</div>
        </div>
        <span class="pill" id="stateB">Ready</span>
      </div>
      <div class="time" id="timeB">00:00</div>
    </div>
  </div>

  <div class="section">
    <div class="sectionTitle">Game Control</div>
    <div class="grid2">
      <button class="good" onclick="cmd('start')">Start</button>
      <button class="warn" onclick="cmd('pause')">Pause</button>
      <button class="primary" onclick="cmd('resume')">Resume</button>
      <button class="bad" id="resetBtn" onclick="cmd('reset')">Reset</button>
    </div>
    <div class="miniStats">
      <div class="stat">
        <div class="statLabel">Battery</div>
        <div class="statValue" id="batt">--</div>
      </div>
      <div class="stat">
        <div class="statLabel">Game State</div>
        <div class="statValue" id="stateText">--</div>
      </div>
    </div>
    <div class="help">Reset is only enabled when paused or after the match has finished.</div>
  </div>

  <div class="section">
    <div class="sectionTitle">Ref Time Fix</div>
    <label>Adjustment amount, seconds
      <input id="adjSecs" type="number" min="1" max="600" value="30" inputmode="numeric">
    </label>
    <div class="sep"></div>
    <div class="grid2">
      <button id="fixAPlus" class="teamAdjustA" onclick="adjustTime('A', getAdjustSeconds())">Red Team +</button>
      <button id="fixAMinus" class="teamAdjustA minus" onclick="adjustTime('A', -getAdjustSeconds())">Red Team −</button>
      <button id="fixBPlus" class="teamAdjustB" onclick="adjustTime('B', getAdjustSeconds())">Blue Team +</button>
      <button id="fixBMinus" class="teamAdjustB minus" onclick="adjustTime('B', -getAdjustSeconds())">Blue Team −</button>
    </div>
    <div class="sep"></div>
    <div class="grid3">
      <button onclick="quickSet(10)">10s</button>
      <button onclick="quickSet(30)">30s</button>
      <button onclick="quickSet(60)">60s</button>
    </div>
    <div class="help">Left buttons add time. Right buttons subtract time. Use this during gameplay if a ref needs to correct a mistake without resetting the match.</div>
  </div>

  <details>
    <summary>Match Setup</summary>
    <div class="detailsInner">
      <label>Duration, minutes
        <input id="mins" type="number" min="1" max="60" value="5" inputmode="numeric">
      </label>
      <div class="sep"></div>
      <button class="primary" onclick="applyDuration()">Apply Duration</button>

      <div class="sep"></div>
      <div class="grid2">
        <label>Team A colour
          <input id="colA" type="color" value="#ff3333">
        </label>
        <label>Team B colour
          <input id="colB" type="color" value="#3333ff">
        </label>
      </div>
      <div class="sep"></div>
      <button class="primary" onclick="applyColors()">Apply Colours</button>
      <div class="sep"></div>
      <button onclick="cmd('identify')">Identify Teams</button>
      <div class="help">Identify teams only works while the game is stopped.</div>
    </div>
  </details>

  <details>
    <summary>Connection Help</summary>
    <div class="detailsInner">
      <div class="help">
        Wi-Fi: <b>KOTH-Timer</b><br>
        Page: <b>http://10.10.10.1</b><br>
        Test: <b>http://10.10.10.1/ping</b><br><br>
        If your phone says no internet, choose stay connected or use without internet.
      </div>
    </div>
  </details>
</div>

<script>
let ws = null;
let lastStateAt = 0;
let wsWanted = true;

const el = (id) => document.getElementById(id);

function setConn(text, good){
  const p = el("connPill");
  p.textContent = text;
  p.className = "pill " + (good ? "good" : "bad");
}

function hexToRgb(hex){
  const h = (hex || "").replace("#","").trim();
  if(h.length !== 6) return null;
  const n = parseInt(h, 16);
  if(Number.isNaN(n)) return null;
  return {r:(n>>16)&255,g:(n>>8)&255,b:n&255};
}

function rgbToHsl(r,g,b){
  r/=255; g/=255; b/=255;
  const max=Math.max(r,g,b), min=Math.min(r,g,b);
  let h=0, s=0, l=(max+min)/2;
  const d=max-min;
  if(d){
    s=d/(1-Math.abs(2*l-1));
    if(max===r) h=((g-b)/d)%6;
    else if(max===g) h=(b-r)/d+2;
    else h=(r-g)/d+4;
    h*=60;
    if(h<0) h+=360;
  }
  return {h,s,l};
}

function teamName(hex){
  const rgb = hexToRgb(hex);
  if(!rgb) return "Team";
  const hsl = rgbToHsl(rgb.r,rgb.g,rgb.b);
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

function contrastText(hex){
  const rgb = hexToRgb(hex);
  if(!rgb) return "#ffffff";
  // Perceived brightness. Black text is easier on bright yellow/green/orange.
  const brightness = (rgb.r * 299 + rgb.g * 587 + rgb.b * 114) / 1000;
  return brightness > 155 ? "#061018" : "#ffffff";
}

function setColours(a,b){
  document.documentElement.style.setProperty("--a", a);
  document.documentElement.style.setProperty("--b", b);
  document.documentElement.style.setProperty("--aText", contrastText(a));
  document.documentElement.style.setProperty("--bText", contrastText(b));
  el("colA").value = a;
  el("colB").value = b;

  const nameA = teamName(a);
  const nameB = teamName(b);

  el("labelA").textContent = nameA;
  el("labelB").textContent = nameB;

  // Ref time-fix buttons use the selected colour names for fast event use.
  el("fixAPlus").textContent = nameA + " +";
  el("fixAMinus").textContent = nameA + " −";
  el("fixBPlus").textContent = nameB + " +";
  el("fixBMinus").textContent = nameB + " −";
}

function updateCards(d){
  const a = el("cardA");
  const b = el("cardB");
  a.classList.remove("active","done");
  b.classList.remove("active","done");
  el("stateA").textContent = "Ready";
  el("stateB").textContent = "Ready";

  if(d.status === "TEAM_A"){
    a.classList.add("active");
    el("stateA").textContent = "Capturing";
    el("stateB").textContent = "Defending";
  } else if(d.status === "TEAM_B"){
    b.classList.add("active");
    el("stateA").textContent = "Defending";
    el("stateB").textContent = "Capturing";
  } else if(d.status === "CONTESTED"){
    a.classList.add("active");
    b.classList.add("active");
    el("stateA").textContent = "Contested";
    el("stateB").textContent = "Contested";
  } else if(d.status === "DONE"){
    a.classList.add("done");
    b.classList.add("done");
    el("stateA").textContent = "Done";
    el("stateB").textContent = "Done";
  }
}

function updateStatus(d){
  const nameA = el("labelA").textContent || "Team A";
  const nameB = el("labelB").textContent || "Team B";
  let main = "Ready";
  let sub = "Waiting for game start";

  if(d.identify){
    main = "Identifying teams";
    sub = "Displays and LEDs are flashing";
  } else if(d.winner === "TEAM_A"){
    main = nameA + " wins!";
    sub = "Match complete";
  } else if(d.winner === "TEAM_B"){
    main = nameB + " wins!";
    sub = "Match complete";
  } else if(d.winner === "DRAW"){
    main = "Draw!";
    sub = "Match complete";
  } else if(d.paused){
    main = "Paused";
    sub = "Ref controls are available";
  } else if(!d.started){
    main = "Ready";
    sub = "Press Start to begin";
  } else if(d.status === "TEAM_A"){
    main = nameA + " capturing";
    sub = "Only Button A is held";
  } else if(d.status === "TEAM_B"){
    main = nameB + " capturing";
    sub = "Only Button B is held";
  } else if(d.status === "CONTESTED"){
    main = "Contested";
    sub = "Both buttons are held — no score";
  } else {
    main = "No capture";
    sub = "Hold one team button to score";
  }

  el("statusMain").textContent = main;
  el("statusSub").textContent = sub;
}

function updateUI(d){
  lastStateAt = Date.now();

  if(typeof d.a === "string") el("timeA").textContent = d.a;
  if(typeof d.b === "string") el("timeB").textContent = d.b;

  if(typeof d.colA === "string" && typeof d.colB === "string"){
    setColours(d.colA, d.colB);
  }

  if(d.dur !== undefined){
    const mins = Math.round(Number(d.dur) / 60);
    if(Number.isFinite(mins)) el("mins").value = mins;
  }

  if(d.bp !== undefined && d.bv !== undefined){
    const bp = Number(d.bp);
    const bv = Number(d.bv);
    if(Number.isFinite(bp) && Number.isFinite(bv)){
      el("batt").textContent = Math.round(bp) + "% / " + bv.toFixed(2) + "V";
    }
  }

  const finished = d.winner === "TEAM_A" || d.winner === "TEAM_B" || d.winner === "DRAW";
  el("resetBtn").disabled = !(!!d.paused || finished);

  el("stateText").textContent = (d.started ? "Started" : "Stopped") + (d.paused ? " / Paused" : "");
  updateCards(d);
  updateStatus(d);
}

function connectWs(){
  if(!wsWanted) return;
  try{
    ws = new WebSocket("ws://" + location.hostname + ":81/");
  }catch(e){
    setTimeout(connectWs, 1500);
    return;
  }

  ws.onopen = function(){ setConn("Live", true); };
  ws.onmessage = function(ev){
    try{ updateUI(JSON.parse(ev.data)); }catch(e){}
  };
  ws.onerror = function(){ try{ ws.close(); }catch(e){} };
  ws.onclose = function(){
    setConn("Retrying", false);
    setTimeout(connectWs, 1500);
  };
}

function pollState(){
  fetch("/state", {cache:"no-store"})
    .then(r => r.json())
    .then(d => {
      updateUI(d);
      if(!ws || ws.readyState !== 1) setConn("Polling", true);
    })
    .catch(() => {
      if(Date.now() - lastStateAt > 3000) setConn("Offline", false);
    });
}

function cmd(c){
  fetch("/cmd?c=" + encodeURIComponent(c), {cache:"no-store"})
    .then(() => setTimeout(pollState, 120))
    .catch(() => {});
}

function applyDuration(){
  let m = parseInt(el("mins").value || "5", 10);
  if(!Number.isFinite(m)) m = 5;
  if(m < 1) m = 1;
  if(m > 60) m = 60;
  el("mins").value = m;
  fetch("/set?dur=" + encodeURIComponent(m), {cache:"no-store"})
    .then(() => setTimeout(pollState, 120))
    .catch(() => {});
}

function applyColors(){
  const a = el("colA").value;
  const b = el("colB").value;
  fetch("/set?colA=" + encodeURIComponent(a) + "&colB=" + encodeURIComponent(b), {cache:"no-store"})
    .then(() => setTimeout(pollState, 120))
    .catch(() => {});
}

function quickSet(s){
  el("adjSecs").value = s;
}

function getAdjustSeconds(){
  let s = parseInt(el("adjSecs").value || "30", 10);
  if(!Number.isFinite(s)) s = 30;
  if(s < 1) s = 1;
  if(s > 600) s = 600;
  el("adjSecs").value = s;
  return s;
}

function adjustTime(team, delta){
  fetch("/adjust?team=" + encodeURIComponent(team) + "&delta=" + encodeURIComponent(delta), {cache:"no-store"})
    .then(() => setTimeout(pollState, 120))
    .catch(() => {});
}

connectWs();
pollState();
setInterval(pollState, 1000);
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

static void handleCaptivePortal(){
  server.sendHeader("Cache-Control", "no-store");
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handlePing(){
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", "KOTH Timer OK");
}

static void handleState(){
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", makeStateJson());
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

static void handleAdjust(){
  if(!server.hasArg("team") || !server.hasArg("delta")){
    server.send(400, "text/plain", "Missing team or delta");
    return;
  }

  String team = server.arg("team");
  int32_t delta = server.arg("delta").toInt();

  // Safety clamp: one click/request can only adjust by up to 10 minutes.
  if(delta > 600) delta = 600;
  if(delta < -600) delta = -600;

  if(team != "A" && team != "B"){
    server.send(400, "text/plain", "Invalid team");
    return;
  }

  adjustTeamTime(team, delta);

  // If time was added back after a match finished, normal loop logic will
  // leave DONE state on the next capture-state evaluation.
  broadcastState();

  server.send(200, "text/plain", "OK");
}

static void onWsEvent(uint8_t num, WStype_t type, uint8_t*, size_t){
  if(type == WStype_CONNECTED){
    String p = makeStateJson();
    ws.sendTXT(num, p);
  }
}

static void startAdminWiFi(){
  // Clean Wi-Fi reset before starting AP.
  WiFi.persistent(false);
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);
  delay(300);

  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  // Try max Wi-Fi power for better phone reliability.
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  // Force known AP address.
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);

  const char* pass = nullptr;
  if(strlen(AP_PASS) >= 8){
    pass = AP_PASS;
  }

  bool apOK = WiFi.softAP(
    AP_SSID,
    pass,
    AP_CHANNEL,
    AP_HIDDEN,
    AP_MAX_CLIENTS
  );

  delay(300);

  dnsServer.start(DNS_PORT, "*", AP_IP);

  Serial.println();
  Serial.println("===== KOTH Timer WiFi =====");
  Serial.print("AP started: ");
  Serial.println(apOK ? "YES" : "NO");
  Serial.print("SSID: ");
  Serial.println(AP_SSID);
  if(strlen(AP_PASS) >= 8){
    Serial.print("Password: ");
    Serial.println(AP_PASS);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Clients: ");
  Serial.println(WiFi.softAPgetStationNum());
  Serial.println("===========================");
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

 startAdminWiFi();

server.on("/", handleRoot);
server.on("/config", handleConfig);
server.on("/set", handleSet);
server.on("/cmd", handleCmd);
server.on("/adjust", handleAdjust);
server.on("/ping", handlePing);
server.on("/state", handleState);

// Common phone/laptop captive portal check URLs.
// These help phones pop up the timer page automatically.
server.on("/generate_204", handleCaptivePortal);        // Android
server.on("/gen_204", handleCaptivePortal);             // Android/Chrome
server.on("/hotspot-detect.html", handleCaptivePortal); // iPhone/iPad/macOS
server.on("/ncsi.txt", handleCaptivePortal);            // Windows

server.onNotFound(handleCaptivePortal);

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
  dnsServer.processNextRequest();
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
      if(now - lastBroadcastMs >= 500){
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
  if(now - lastBroadcastMs >= 500){
    broadcastState();
    lastBroadcastMs = now;
  }

  delay(1);
}
