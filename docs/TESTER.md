# ESP32 Meter — Kit Manual

---

## Overview

ESP32 Meter is a WiFi-served multimeter built on an ESP32-C3. You build the measurement circuit on a breadboard, flash the firmware, and read results in any browser — no display, no dedicated software.

**Measures:**
- DC voltage: 0–33 V
- Resistance: ~1 Ω – 1 MΩ (auto-ranging)
- Continuity: below 50 Ω
- Diode forward voltage (Vf): identifies silicon, Schottky, and LED types
- Capacitance: ~5 nF – 1000 µF (RC timing method)

Unlike the Lab kit, you build the measurement circuit yourself. The resistor divider, reference network, and ADC protection components are all external — understanding their purpose is part of the exercise.

---

## Bill of Materials

| Qty | Component | Value | Function |
|-----|-----------|-------|----------|
| 1 | Resistor | 180 kΩ | Voltage divider top arm |
| 1 | Resistor | 20 kΩ | Voltage divider bottom arm |
| 2 | Resistor | 10 kΩ | ADC input protection + capacitor charge |
| 1 | Resistor | 1 kΩ | Mid-range resistance reference |
| 1 | Resistor | 100 Ω | Low-range resistance reference / diode test |
| 1 | Zener diode | 3.6 V | Overvoltage clamp on voltage input |
| 1 | Capacitor | 100 nF | Power supply decoupling |
| 2 | Crocodile clip leads | — | Red (+) and black (−) test probes |

Total BOM cost: approximately £3–8 depending on source. All parts available from AliExpress, Amazon, or any electronics supplier.

---

## GPIO Pin Assignments (ESP32-C3)

| GPIO | Direction | Function |
|------|-----------|----------|
| 0 | ADC input | Voltage divider sense |
| 1 | ADC input | Resistance / continuity / diode sense (via 10 kΩ protect) |
| 2 | ADC input | Capacitance sense (direct, no protect) |
| 3 | Digital output | Drive 100 Ω reference |
| 4 | Digital output | Drive 1 kΩ reference |
| 5 | Digital output | Drive 10 kΩ reference |
| 6 | Digital output | Capacitor charge trigger |

GPIOs 0–2 are on ADC1, which remains functional when WiFi is active. ADC2 channels on the C3 are unusable with WiFi — avoid them.

---

## Circuit Description

### Section 1 — Voltage divider (GPIO 0)

```
Probe A+ ──┬── 180 kΩ ──┬── GPIO 0 (ADC sense)
           │             │
         (open)        20 kΩ ─── GND
                         │
                      Zener 3.6 V (cathode to node, anode to GND)
Probe B−  ────────────── GND
```

The 180 kΩ / 20 kΩ divider scales 33 V → 3.3 V at the ADC pin.

`V_in = V_adc × (180 + 20) / 20 = V_adc × 10`

The 3.6 V Zener clamps the ADC node against transient overvoltage.

### Section 2 — Resistance / continuity / diode (GPIO 1, 3, 4, 5)

```
GPIO 3 ── 100 Ω ──┐
GPIO 4 ──  1 kΩ ──┼── Probe A+ (measurement node)
GPIO 5 ── 10 kΩ ──┘        │
                        10 kΩ (protect) ── GPIO 1 (ADC sense)
Probe B−  ──────────────── GND
```

One reference pin is driven HIGH at a time; the others are set to INPUT (high-Z) to avoid parallel paths corrupting the measurement.

With R_ref driving 3.3 V and R_x between probe node and GND:

`V_node = 3.3 × R_x / (R_ref + R_x)`

`R_x = R_ref × V_node / (3.3 − V_node)`

Auto-ranging: start with 10 kΩ. If ADC reads > 97% of scale (R_x >> R_ref), stay. If ADC < 15%, step down to 1 kΩ, then 100 Ω.

For diode testing: drive GPIO 3 (100 Ω ref), read V_node. Since the diode's anode is at the probe node and cathode is at GND: `Vf = V_node`.

### Section 3 — Capacitance (GPIO 2, 6)

```
GPIO 6 ── 10 kΩ ── Probe A+ (same node as Section 2)
                       │
                    C_x (unknown)
                       │
Probe B−  ──────────── GND
                       │
                   GPIO 2 (ADC sense, direct)
```

Sections 2 and 3 share the same probe nodes. The inactive reference pins (GPIO 3/4/5) are set to INPUT during capacitance measurement, so they do not affect the RC circuit.

**RC timing method:**
1. Drive GPIO 6 LOW for 300 ms — discharge C_x
2. Drive GPIO 6 HIGH, start timer
3. Poll GPIO 2 ADC until reading ≥ 2089 mV (63.2% of 3300 mV = one time constant)
4. `C = t / R_charge` where R_charge = 10,000 Ω, t in seconds

GPIO 2 connects directly to the probe node — no protection resistor — to preserve measurement speed.

---

## Building the Circuit

### 1. Place the ESP32-C3

Straddle the ESP32-C3 across the centre gap of the breadboard. Leave working space on both sides.

### 2. Power rails

- Connect **3V3 pin → + power rail**
- Connect **GND pin → − power rail**
- Place 100 nF capacitor across the rails close to the board

### 3. Voltage divider (Section 1)

Place 180 kΩ and 20 kΩ in series between an empty row (the input node) and GND. Connect the Zener across the 20 kΩ (cathode to input node, anode to GND). Wire the input node to **GPIO 0**.

The red voltage probe connects to the far end of the 180 kΩ; black probe to GND.

### 4. Measurement node (Sections 2 and 3)

Choose a clear area for the measurement node — a single row that will carry:

| Connection | Via |
|-----------|-----|
| GPIO 3 | 100 Ω resistor |
| GPIO 4 | 1 kΩ resistor |
| GPIO 5 | 10 kΩ resistor |
| GPIO 1 | 10 kΩ protect resistor |
| GPIO 2 | Direct jumper |
| GPIO 6 | 10 kΩ charge resistor |
| Red probe | Direct (crocodile clip) |

Black probe to GND.

### 5. Check before powering on

- [ ] 3V3 → + rail; GND → − rail; 100 nF across rails
- [ ] 180 kΩ + 20 kΩ divider; Zener across bottom; node → GPIO 0
- [ ] 100 Ω from measurement node → GPIO 3
- [ ] 1 kΩ from measurement node → GPIO 4
- [ ] 10 kΩ (reference) from measurement node → GPIO 5
- [ ] 10 kΩ (protect) from measurement node → GPIO 1
- [ ] Direct wire: measurement node → GPIO 2
- [ ] 10 kΩ (charge) from GPIO 6 → measurement node
- [ ] Red probe → measurement node; black probe → GND

---

## Firmware

Flash `esp32meter.ino` (ESP32-C3 target) via USB using `build.bat COM_PORT`, or upload OTA from the Lab firmware's System tab.

To switch back to Lab firmware, use the meter's own Firmware Update card in the browser.

---

## Connecting

1. Power the board via USB
2. Connect to WiFi: **ESP32Meter_XXXX** (no password)
3. Open browser: **http://192.168.4.1**

Voltage and resistance update automatically every 2 seconds. Capacitance requires pressing **Measure**.

> **Phone users:** Turn off mobile data before connecting, or the phone will prefer 4G over the board's WiFi.

---

## Measurements

### Voltage

Red probe to +, black to −. Measures 0–33 V DC. The reading updates on every poll.

Accuracy is limited by the ADC's linearity (±1–2% typical mid-range) and resistor tolerances. Use measured resistor values in `config.h` for best accuracy.

Does not measure AC voltage or current.

### Resistance

Red probe to one leg, black to the other. Polarity does not matter for resistors.

Auto-ranging selects the reference that keeps the ADC reading mid-scale. The range in use is shown below the value.

- Below ~50 Ω: CONTINUITY shown in green
- Above ~500 kΩ: shown as Open

### Continuity

A result of the resistance measurement. Any reading below 50 Ω (configurable via `CONTINUITY_OHMS` in `config.h`) highlights the card in green. Useful for verifying connections and checking for shorts.

### Diode / LED

Red probe to **anode**, black to **cathode** (stripe end).

The meter drives 100 Ω reference from GPIO 3, reads V_node, and displays it as Vf. Classifications:

| Vf range | Type |
|----------|------|
| < 0.1 V | Short circuit |
| 0.1 – 0.4 V | Schottky |
| 0.4 – 0.75 V | Silicon diode |
| 0.75 – 1.5 V | LED (IR / red) |
| 1.5 – 2.5 V | LED (yellow / green) |
| > 2.5 V | LED (blue / white) |
| > 3.1 V | Open or reversed |

Reversed probes give a near-3.3 V reading (reverse-biased = no current = V_node → 3.3 V).

Note: test current through 100 Ω at 3.3 V supply is approximately (3.3 − Vf) / 100 — around 26 mA for a silicon diode. LEDs will light dimly during the test.

### Capacitance

Press **Measure**. The measurement takes up to 3 seconds for large capacitors.

Reliable range: ~10 nF to ~1000 µF. Below 5 nF the `micros()` timer resolution limits accuracy. Above ~1000 µF the charge time exceeds the 3-second timeout.

Electrolytics read slightly low due to ESR — acceptable for identification.

---

## ADC Notes

The firmware uses `analogReadMilliVolts()` (ESP32 Arduino Core v2+), which applies factory efuse calibration automatically. This is more accurate than scaling raw counts, particularly at low and high extremes.

For the capacitance timing loop, `analogReadMilliVolts()` is also used (threshold: 2089 mV = 63.2% of 3300 mV).

To improve voltage measurement accuracy: measure your actual 180 kΩ and 20 kΩ resistors with the meter itself, and update `VDIV_RATIO` in `config.h` accordingly.

---

## Known Limitations

| Limitation | Detail |
|-----------|--------|
| No AC measurement | ADC only reads DC |
| No current measurement | Would require a shunt resistor and differential amplifier |
| Caps < 5 nF | Timer resolution limits accuracy |
| Caps > 1000 µF | Exceeds 3 s charge timeout |
| ADC non-linearity | ±1–2% mid-range; worse at extremes |
| Single-ended voltage only | Measures relative to GND; cannot measure floating or differential signals |

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| Voltage reads low for batteries | Probes on wrong circuit node | Red probe to far side of 180 kΩ, not to the divider junction |
| Resistance always shows Open | Missing connection between probe node and GPIO 1 or 5 | Check the 10 kΩ protect and reference resistors |
| Resistance reads wrong range | Reference resistor connected to wrong GPIO | Check GPIO 3 = 100 Ω, GPIO 4 = 1 kΩ, GPIO 5 = 10 kΩ |
| Diode shows Open when connected | Probes reversed | Red to anode (+), black to cathode (stripe) |
| Capacitance always times out | No capacitor connected, or charge circuit missing | Check GPIO 6 → 10 kΩ → probe node; GPIO 2 → probe node |
| Capacitance reads much too high | GPIO 2 not connected directly; 10 kΩ protect in path | GPIO 2 must connect directly to probe node, no protect resistor |
| All readings unstable | Missing decoupling capacitor | Add 100 nF across 3V3 and GND close to the board |

---

## What's Next

### Coding stream

Write your own Arduino sketches for the ESP32-C3. Suggested progression: digital output (LED blink), digital input (button), PWM (LED dimmer), analogue read (potentiometer), combining inputs and outputs. The same breadboard and components apply.

### Hardware stream

Solder the meter circuit onto stripboard with an IC socket for the ESP32. This makes a permanent, portable instrument. The soldering guide covers stripboard layout, socket placement, joint technique, and testing.
