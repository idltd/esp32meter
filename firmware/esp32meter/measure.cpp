#include <Arduino.h>
#include "config.h"
#include "measure.h"

// Oversample and return calibrated millivolts.
// analogReadMilliVolts() applies the ESP32's factory ADC calibration — more
// accurate than scaling raw counts, especially at the low end.
static float readmV(int pin) {
    long sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogReadMilliVolts(pin);
        delayMicroseconds(50);
    }
    return (float)sum / ADC_SAMPLES;
}

static void allRefsInput() {
    pinMode(PIN_REF_100R, INPUT);
    pinMode(PIN_REF_1K,   INPUT);
    pinMode(PIN_REF_10K,  INPUT);
}

// Drive refPin HIGH, measure voltage at the junction, return R_x in ohms.
// Circuit: 3.3V → R_ref → [junction = ProbeA+] → R_x → GND
// V_junction = 3.3 × R_x / (R_ref + R_x)  →  R_x = R_ref × V_j / (3.3 - V_j)
static float measureWithRef(int refPin, float rRef) {
    allRefsInput();
    pinMode(refPin, OUTPUT);
    digitalWrite(refPin, HIGH);
    delay(5);
    float vj = readmV(PIN_ADC_RDIODE);
    pinMode(refPin, INPUT);

    if (vj >= 3200.0f) return 1e9f;  // open circuit
    if (vj <=   50.0f) return 0.0f;  // short
    return rRef * vj / (ADC_VREF_MV - vj);
}

// ── Voltage ──────────────────────────────────────────────────────────────────

float measureVoltage() {
    return readmV(PIN_ADC_VOLTAGE) * VDIV_RATIO / 1000.0f;
}

// ── Resistance / continuity ───────────────────────────────────────────────────

RResult measureResistance() {
    RResult r = {};

    float rx = measureWithRef(PIN_REF_10K, R_REF_10K);
    r.range = "10k ref";

    if (rx < 500.0f) {
        rx = measureWithRef(PIN_REF_100R, R_REF_100);
        r.range = "100Ω ref";
    } else if (rx < 5000.0f) {
        rx = measureWithRef(PIN_REF_1K, R_REF_1K);
        r.range = "1k ref";
    }

    r.ohms       = rx;
    r.continuity = (rx < CONTINUITY_OHMS);
    r.openCircuit = (rx > 500000.0f);
    return r;
}

// ── Diode ────────────────────────────────────────────────────────────────────
// Probe orientation: ProbeA+ on anode, ProbeB- on cathode.
// V_junction = Vf (the diode forward voltage is the voltage at the junction).

DiodeResult measureDiode() {
    DiodeResult d = {};

    allRefsInput();
    pinMode(PIN_REF_100R, OUTPUT);
    digitalWrite(PIN_REF_100R, HIGH);
    delay(5);
    float vjMV = readmV(PIN_ADC_RDIODE);
    pinMode(PIN_REF_100R, INPUT);

    d.vf = vjMV / 1000.0f;

    if      (d.vf > 3.1f)   d.type = "Open or reversed";
    else if (d.vf < 0.1f)   d.type = "Short circuit";
    else if (d.vf < 0.40f)  d.type = "Schottky";
    else if (d.vf < 0.75f)  d.type = "Silicon diode";
    else if (d.vf < 1.50f)  d.type = "LED (IR / red)";
    else if (d.vf < 2.50f)  d.type = "LED (yellow / green)";
    else                     d.type = "LED (blue / white)";

    return d;
}

// ── Capacitance (RC timing) ───────────────────────────────────────────────────

CapResult measureCapacitance() {
    CapResult c = {};

    // Discharge
    pinMode(PIN_CAP_CHARGE, OUTPUT);
    digitalWrite(PIN_CAP_CHARGE, LOW);
    delay(300);

    // Charge: record time to reach 63.2% of Vcc (= one RC time constant)
    unsigned long t0 = micros();
    digitalWrite(PIN_CAP_CHARGE, HIGH);

    unsigned long elapsed = 0;
    while (true) {
        elapsed = micros() - t0;
        if (analogReadMilliVolts(PIN_ADC_CAP) >= 2089) break;  // 63.2% of 3300 mV
        if (elapsed >= CAP_TIMEOUT_US) { c.timeout = true; break; }
    }

    // Discharge after measurement
    digitalWrite(PIN_CAP_CHARGE, LOW);
    delay(300);

    if (c.timeout) return c;

    if (elapsed < 50) {
        c.tooSmall = true;
        return c;
    }

    c.farads = (float)elapsed / 1e6f / R_CHARGE;
    return c;
}

String formatCapacitance(const CapResult& c) {
    if (c.timeout)  return "> 3 s (cap too large or not connected)";
    if (c.tooSmall) return "< 5 nF (below reliable range)";
    if (c.farads == 0) return "—";

    if (c.farads < 1e-6f) return String(c.farads * 1e9f, 1) + " nF";
    if (c.farads < 1e-3f) return String(c.farads * 1e6f, 2) + " µF";
    return String(c.farads * 1e3f, 1) + " mF";
}
