# ESP32 Multimeter — Project Brief

This document is a complete handoff for implementing the ESP32 Multimeter project in
Claude Code. It covers hardware design, circuit topology, GPIO assignments, BOM, and
firmware requirements. Slot this into whatever project structure you already use for
your other ESP32 apps.

---

## Overview

A WiFi-served multimeter built on an ESP32 DevKit V1. No display — results are
served as a web page over the local network. Designed for beginners to identify
components in simple sensor circuits.

**Measures:**
- DC voltage: 0–33 V
- Resistance: auto-ranging, ~1 Ω – ~1 MΩ
- Continuity: audible/visual indication below 50 Ω
- Diode forward voltage (Vf): with 100 Ω reference
- Capacitance: ~1 nF – 1000 µF (RC timing method)

**Does NOT measure:** AC voltage, current, inductance, or anything above 33 V DC.

---

## Hardware

### Board
ESP32 DevKit V1 (38-pin, dual-row, ~$4 shipped). Any standard DevKit with the same
pinout works. The board straddles the centre gap of a standard 830-point breadboard,
columns 3–21.

### GPIO Pin Assignments

| GPIO | Direction | Function |
|------|-----------|----------|
| 34 | ADC input | Resistance / continuity / diode sense (with 10 kΩ protect) |
| 35 | ADC input | Voltage measurement |
| 32 | ADC input | Capacitance sense (watches for 63% threshold) |
| 25 | Digital output | Drive ref 100 Ω (auto-range low) |
| 26 | Digital output | Drive ref 1 kΩ (auto-range mid) |
| 27 | Digital output | Drive ref 10 kΩ (auto-range high) |
| 33 | Digital output | Capacitor charge trigger |

GPIO 34, 35, 32 are input-only pins (no internal pull-up/down) — ideal for ADC use.

---

## Circuit Design

### Section ①  Voltage Divider (0–33 V DC)

```
Probe A+ ──┬── 180 kΩ ──┬── GPIO35 (ADC)
           │             │
         (open)        20 kΩ ── GND
                         │
                      3.6 V Zener (cathode to node, anode to GND)
Probe B−  ─────────────── GND
```

- The 180 kΩ + 20 kΩ divider scales 33 V → 3.3 V at the ADC pin.
- The 3.6 V Zener clamps voltage spikes, protecting the ADC from overvoltage.
- **Voltage formula:** `V_in = ADC_reading × (3.3 / 4095) × ((180 + 20) / 20)`
  Simplifies to: `V_in = ADC_reading × 0.008058...`

### Section ②  Resistance / Continuity / Diode

```
GPIO25 ── 100 Ω ──┐
GPIO26 ──  1 kΩ ──┼── Probe A+
GPIO27 ── 10 kΩ ──┘      │
                        10 kΩ protect ── GPIO34 (ADC)
Probe B− ─────────────── GND
```

Only ONE of GPIO25/26/27 is driven HIGH at a time. The others are set to INPUT
(high-impedance) to avoid parallel current paths corrupting the measurement.

**How auto-ranging works:**

The unknown resistor (R_x) sits between Probe A+ and GND (via Probe B−). The active
reference resistor (R_ref) and R_x form a voltage divider. The ADC reads the voltage
at the junction.

`V_junction = 3.3 × R_x / (R_ref + R_x)`  
`R_x = R_ref × V_junction / (3.3 - V_junction)`

**Range selection logic (pick the ref that keeps ADC mid-scale):**

| Unknown R | Use ref | GPIO driven |
|-----------|---------|-------------|
| < ~500 Ω  | 100 Ω   | GPIO25      |
| 500 Ω – 5 kΩ | 1 kΩ | GPIO26     |
| > 5 kΩ    | 10 kΩ   | GPIO27      |

Auto-range algorithm: start with 10 kΩ ref. If ADC reads > 90% of full scale
(R_x >> R_ref), stay. If ADC reads < 10% (R_x << R_ref), switch down a range.
Take a second reading on the new range. Iterate once if needed.

**Continuity:** R_x < 50 Ω → trigger a short beep via a PWM tone on any spare GPIO,
or just display a prominent BEEP indicator in the web UI.

**Diode test:** Drive GPIO25 (100 Ω ref). Read ADC. Display Vf directly.
- Silicon diode: Vf ≈ 0.6–0.7 V
- Schottky: Vf ≈ 0.2–0.4 V
- LED: Vf ≈ 1.8–3.5 V (varies by colour)
- Open circuit / no diode: Vf ≈ 3.3 V (no drop across ref)
- Short / wrong orientation: Vf ≈ 0 V

The 10 kΩ protect resistor in series with GPIO34 limits current if the probe is
accidentally connected to a voltage source.

### Section ③  Capacitance (RC Timing)

```
GPIO33 ── 10 kΩ ── Probe A+
                      │
                    C_x (unknown)
                      │
Probe B− ─────────── GND
                      │
                   GPIO32 (ADC) ── same node as Probe A+
```

**How it works:**

1. Discharge phase: set GPIO33 LOW, GPIO32 INPUT. Wait ~200 ms for full discharge.
2. Charge phase: set GPIO33 HIGH. Start a microsecond timer.
3. Poll GPIO32 ADC continuously until reading ≥ 2115 (= 4095 × 0.632, the 63.2%
   RC time-constant threshold ≈ 2.073 V).
4. Stop timer. Record elapsed microseconds as `t`.
5. `C = t / R_charge` where R_charge = 10,000 Ω
6. Result in Farads → convert to display units (nF or µF).

**Range and accuracy notes:**
- Very small caps (< 10 nF): timing resolution of `micros()` limits accuracy. Add a
  note in the UI.
- Large caps (> 100 µF): charge time can exceed 1 second. Use a timeout of 3 s and
  report overflow.
- ESR (series resistance) in electrolytics causes slight underreading — acceptable
  for identification purposes.

---

## Bill of Materials

| Qty | Component | Value | Notes |
|-----|-----------|-------|-------|
| 1 | Resistor | 180 kΩ | Voltage divider top |
| 1 | Resistor | 20 kΩ | Voltage divider bottom |
| 4 | Resistor | 10 kΩ | 1× cap charge, 1× ADC protect, 2× spare |
| 1 | Resistor | 1 kΩ | R ref mid-range |
| 1 | Resistor | 100 Ω | R ref low-range / diode test |
| 1 | Zener diode | 3.6 V | Any 3.6 V Zener, e.g. BZX55C3V6 |
| 1 | Capacitor | 100 nF | Decoupling across 3.3V/GND on breadboard |
| 2 | Crocodile clip leads | — | Red (+) and black (−) probes |
| 1 | ESP32 DevKit V1 | — | 38-pin variant |

Total cost: ~£3–8 depending on source. All available on AliExpress or Amazon.

---

## Firmware Specification

### Stack

- **Framework:** Arduino (ESP32 Arduino Core, v2.x or v3.x)
- **Language:** C++
- **Web server:** `WebServer.h` (bundled with ESP32 Arduino Core)
- **WiFi:** `WiFi.h` in station mode (STA) — joins existing network

No external libraries required beyond what ships with the ESP32 Arduino Core.

### File Structure

Fit this into whatever structure your other ESP32 apps use. Suggested layout:

```
esp32-multimeter/
├── esp32-multimeter.ino   (or main.cpp if using PlatformIO)
├── config.h               (WiFi credentials, pin defs, tuning constants)
├── measure.h / measure.cpp (measurement functions)
├── webui.h / webui.cpp    (HTML generation, web server routes)
└── README.md
```

### config.h

```cpp
#pragma once

// WiFi
const char* WIFI_SSID     = "your_ssid";
const char* WIFI_PASSWORD = "your_password";

// ADC pins (input-only, no pull-up)
const int PIN_ADC_VOLTAGE = 35;
const int PIN_ADC_RDIODE  = 34;
const int PIN_ADC_CAP     = 32;

// Output pins — reference resistors
const int PIN_REF_100R  = 25;   // 100 Ω ref
const int PIN_REF_1K    = 26;   // 1 kΩ ref
const int PIN_REF_10K   = 27;   // 10 kΩ ref

// Output pin — capacitor charge
const int PIN_CAP_CHARGE = 33;

// Physical resistor values (measured if possible — improves accuracy)
const float R_REF_100  = 100.0;
const float R_REF_1K   = 1000.0;
const float R_REF_10K  = 10000.0;
const float R_CHARGE   = 10000.0;
const float R_PROTECT  = 10000.0;   // in series with ADC_RDIODE pin

// Voltage divider ratio: V_in = ADC_V × VDIV_SCALE
// = (3.3 / 4095) × ((180000 + 20000) / 20000)
const float VDIV_SCALE = (3.3f / 4095.0f) * (200000.0f / 20000.0f);

// ADC reference
const float ADC_VREF = 3.3f;
const int   ADC_MAX  = 4095;

// Continuity threshold
const float CONTINUITY_OHMS = 50.0f;

// Capacitance charge timeout
const unsigned long CAP_TIMEOUT_US = 3000000UL;  // 3 seconds

// ADC oversampling (average N readings)
const int ADC_SAMPLES = 64;
```

### Core Measurement Functions (measure.cpp)

```cpp
// Oversample ADC for noise reduction
float readADC(int pin) {
    long sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogRead(pin);
        delayMicroseconds(50);
    }
    return (float)sum / ADC_SAMPLES;
}

float adcToVolts(float raw) {
    return raw * (ADC_VREF / ADC_MAX);
}

// ── Voltage ──────────────────────────────────────────────────────────────────
float measureVoltage() {
    float raw = readADC(PIN_ADC_VOLTAGE);
    return raw * VDIV_SCALE;
}

// ── Resistance (single range) ────────────────────────────────────────────────
// Drives refPin HIGH, others INPUT, reads junction voltage, returns R_x in Ω
float measureResistanceWithRef(int refPin, float rRef) {
    // All ref pins to INPUT first
    pinMode(PIN_REF_100R, INPUT);
    pinMode(PIN_REF_1K,   INPUT);
    pinMode(PIN_REF_10K,  INPUT);

    pinMode(refPin, OUTPUT);
    digitalWrite(refPin, HIGH);
    delay(5);  // settle

    float raw    = readADC(PIN_ADC_RDIODE);
    float vJunc  = adcToVolts(raw);

    // Restore to INPUT
    pinMode(refPin, INPUT);

    if (vJunc >= ADC_VREF - 0.05f) return 1e9f;  // open circuit
    if (vJunc <= 0.05f)            return 0.0f;   // short

    return rRef * vJunc / (ADC_VREF - vJunc);
}

// Auto-ranging resistance
struct RResult { float ohms; String range; bool continuity; bool openCircuit; };

RResult measureResistance() {
    RResult r;
    r.continuity  = false;
    r.openCircuit = false;

    // Try 10 kΩ first
    float rx = measureResistanceWithRef(PIN_REF_10K, R_REF_10K);
    r.range = "10kΩ ref";

    if (rx < 500.0f) {
        // Switch to 100 Ω for better resolution
        rx = measureResistanceWithRef(PIN_REF_100R, R_REF_100);
        r.range = "100Ω ref";
    } else if (rx < 5000.0f) {
        rx = measureResistanceWithRef(PIN_REF_1K, R_REF_1K);
        r.range = "1kΩ ref";
    }

    // Correct for protect resistor in series with ADC pin
    // (only significant at low R values)
    // The protect R is between the measurement node and GPIO34.
    // It's in parallel with the path to ADC — minimal effect at high R,
    // but subtract at low R for accuracy:
    if (rx > 0 && rx < 1000.0f) {
        rx = (rx * R_PROTECT) / (R_PROTECT - rx);  // approx correction
    }

    r.ohms        = rx;
    r.continuity  = (rx < CONTINUITY_OHMS);
    r.openCircuit = (rx > 500000.0f);
    return r;
}

// ── Diode ────────────────────────────────────────────────────────────────────
struct DiodeResult { float vf; String type; };

DiodeResult measureDiode() {
    DiodeResult d;
    // Use 100 Ω ref for diode test
    float rx = measureResistanceWithRef(PIN_REF_100R, R_REF_100);
    // Vf is the voltage drop = 3.3 - V_junction
    float raw   = readADC(PIN_ADC_RDIODE);
    float vJunc = adcToVolts(raw);
    d.vf = ADC_VREF - vJunc;

    if      (d.vf > 3.1f)           d.type = "Open / not connected";
    else if (d.vf < 0.1f)           d.type = "Short / reversed";
    else if (d.vf < 0.4f)           d.type = "Schottky diode";
    else if (d.vf < 0.75f)          d.type = "Silicon diode";
    else if (d.vf < 1.2f)           d.type = "LED (infrared / red)";
    else if (d.vf < 2.2f)           d.type = "LED (yellow / green)";
    else                             d.type = "LED (blue / white / UV)";

    return d;
}

// ── Capacitance (RC timing) ───────────────────────────────────────────────────
struct CapResult { float farads; String display; bool timeout; bool tooSmall; };

CapResult measureCapacitance() {
    CapResult c;
    c.timeout  = false;
    c.tooSmall = false;

    // Discharge phase
    pinMode(PIN_CAP_CHARGE, OUTPUT);
    digitalWrite(PIN_CAP_CHARGE, LOW);
    delay(300);  // discharge time — increase for large caps if needed

    // Charge threshold: 63.2% of 3.3 V = 2.089 V → ADC = 2.089/3.3 * 4095 ≈ 2594
    const int THRESHOLD = (int)(0.632f * ADC_MAX);

    // Start charge
    unsigned long t0 = micros();
    digitalWrite(PIN_CAP_CHARGE, HIGH);

    unsigned long elapsed = 0;
    while (true) {
        int reading = analogRead(PIN_CAP_CHARGE);  // fast single read during timing
        elapsed = micros() - t0;
        if (reading >= THRESHOLD) break;
        if (elapsed >= CAP_TIMEOUT_US) { c.timeout = true; break; }
    }

    // Discharge again
    digitalWrite(PIN_CAP_CHARGE, LOW);
    delay(300);

    if (c.timeout) {
        c.display = "> 3 s timeout (cap too large or not connected)";
        c.farads  = 0;
        return c;
    }

    if (elapsed < 50) {
        c.tooSmall = true;
        c.display  = "< 5 nF (below reliable range)";
        c.farads   = 0;
        return c;
    }

    c.farads = (float)elapsed / 1e6f / R_CHARGE;  // C = t / R

    // Format display
    if (c.farads < 1e-6f)
        c.display = String(c.farads * 1e9f, 1) + " nF";
    else if (c.farads < 1e-3f)
        c.display = String(c.farads * 1e6f, 2) + " µF";
    else
        c.display = String(c.farads * 1e3f, 1) + " mF";

    return c;
}
```

### Web Server & HTML UI (webui.cpp)

The web server exposes two routes:

- `GET /` — returns the HTML dashboard (auto-refreshes every 2 seconds)
- `GET /data` — returns a JSON object with all current readings (for AJAX polling)

**`/data` JSON response format:**

```json
{
  "voltage":   12.34,
  "resistance": 4700.0,
  "continuity": false,
  "open":       false,
  "r_range":   "1kΩ ref",
  "diode_vf":   0.65,
  "diode_type": "Silicon diode",
  "cap_display": "470 nF",
  "cap_farads":  4.7e-7,
  "cap_timeout": false,
  "cap_small":   false,
  "mode":       "resistance"
}
```

**Mode selection:** Add a `?mode=voltage|resistance|diode|capacitance` query param
to `/data` so the UI can request individual modes without measuring everything every
poll (capacitance in particular is slow).

Or, if simplicity is preferred: always measure voltage and resistance on every poll,
and only measure capacitance when the user clicks a button that POSTs to `/measure/cap`.

**HTML UI requirements:**

- Clean, mobile-friendly single page (no external CDN dependencies — serve inline CSS)
- Show all readings in large text
- Highlight continuity in green if active
- Show diode type as a badge next to Vf
- Show capacitance with a "Measure" button (capacitance measurement takes time)
- Auto-refresh voltage and resistance every 2 s via `fetch('/data?mode=passive')`

### Main Sketch Structure (esp32-multimeter.ino)

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "measure.h"
#include "webui.h"

WebServer server(80);

void setup() {
    Serial.begin(115200);

    // All ref pins start as INPUT (high-Z)
    pinMode(PIN_REF_100R,  INPUT);
    pinMode(PIN_REF_1K,    INPUT);
    pinMode(PIN_REF_10K,   INPUT);
    pinMode(PIN_CAP_CHARGE, OUTPUT);
    digitalWrite(PIN_CAP_CHARGE, LOW);

    analogReadResolution(12);       // 12-bit ADC (0–4095)
    analogSetAttenuation(ADC_11db); // Full 0–3.3 V range

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
    Serial.println("\nIP: " + WiFi.localIP().toString());

    // Routes
    server.on("/",     HTTP_GET,  handleRoot);
    server.on("/data", HTTP_GET,  handleData);
    server.on("/measure/cap", HTTP_POST, handleCapMeasure);
    server.begin();
}

void loop() {
    server.handleClient();
}
```

---

## ADC Calibration Notes

The ESP32 ADC is non-linear, especially at the extremes (< 0.1 V and > 3.1 V). For
voltage measurement this matters most at the low end. Options:

1. **Ignore it** — good enough for component identification.
2. **Use `esp_adc_cal`** (IDF) or the Arduino `analogReadMilliVolts()` function
   (available in ESP32 Arduino Core v2+) which applies factory calibration
   automatically. Prefer `analogReadMilliVolts()` for best accuracy.
3. **Software correction table** — measure known voltages and fit a polynomial.

For resistance and capacitance, the non-linearity is less critical because the
formulas are ratiometric (the error partially cancels).

---

## Known Limitations

| Limitation | Detail |
|-----------|--------|
| No AC measurement | The ADC only reads DC. AC would require a precision rectifier circuit. |
| No current measurement | Would need a shunt resistor in series with the load and a differential amp. |
| Caps < 5 nF | `micros()` resolution limits timing accuracy. Results unreliable below ~10 nF. |
| Caps > 1000 µF | Charge time exceeds timeout. Increase `CAP_TIMEOUT_US` and discharge delay if needed. |
| ESR not measured | Electrolytics show slightly low capacitance due to ESR. Acceptable for identification. |
| ADC non-linearity | Worst at ADC extremes. Use `analogReadMilliVolts()` for best voltage accuracy. |
| Crosstalk | If ref resistor GPIOs are not INPUT when idle, leakage can affect adjacent ADC readings. Always set unused ref pins to INPUT. |
| Single-ended only | Measures voltage relative to GND. Cannot measure floating or differential signals. |

---

## Integration Notes (for existing ESP32 projects)

- The web server runs on port 80. If your other apps use port 80 or have their own
  `WebServer` instance, merge the routes into your existing server object.
- WiFi connection management: if you have a shared WiFi init routine, remove the
  WiFi setup from this sketch and call your shared version instead.
- mDNS: add `#include <ESPmDNS.h>` and `MDNS.begin("multimeter")` to access the
  device at `http://multimeter.local` instead of by IP.
- OTA: add `ArduinoOTA` in the usual way — no conflicts with this design.
- The measurement functions in `measure.cpp` are self-contained and stateless.
  They can be called from any task or loop without modification.

---

## Breadboard Layout Summary

```
Cols  1– 2  : Decoupling cap (100 nF across 3.3V / GND rails)
Cols  3–21  : ESP32 DevKit V1 (straddles centre gap)
Cols 23–36  : ① Voltage divider (top rows a–e) + ② R/diode circuit (bottom rows g–j)
Cols 38–44  : ③ Capacitance circuit (bottom rows g–j)

Top power rail  (R+) : 3.3V from ESP32 pin 3V3
Top ground rail (R−) : GND, runs full width
Bottom rails (S−/S+) : GND only (looped from top rail at col 46)

Long jumper wires (dashed in diagram):
  GPIO35 → col 13 row f, arcs to col 32 row c  (voltage ADC sense)
  GPIO34 → col 14 row f, arcs to col 37 row g  (R/diode ADC sense)
  GPIO32 → col 12 row f, arcs to col 44 row j  (cap ADC sense)
```

Refer to `esp32-multimeter-breadboard.png` for the visual layout.

---

*Generated from design session — circuit finalised, firmware spec ready to implement.*
