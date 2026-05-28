# Coding Stream — ESP32-C3 Arduino Sketches

Seven progressive sketches covering digital output, PWM, digital input, and analogue I/O. Each sketch is self-contained and heavily commented. Build the circuit incrementally — each sketch adds at most one new component.

---

## Hardware Reference

| Pin | Component | Notes |
|-----|-----------|-------|
| GPIO 8 | LED via 330 Ω to GND | Output for all LED sketches |
| GPIO 9 | Button to GND | INPUT_PULLUP; pressed = LOW |
| GPIO 1 | Potentiometer wiper | ADC1, reliable with WiFi active |
| GPIO 10 | Passive buzzer to GND | Sketch 07 only |

**Potentiometer wiring:** left outer leg → GND, right outer leg → 3V3, middle (wiper) → GPIO 1.

All sketches target **ESP32-C3** (FQBN: `esp32:esp32:esp32c3`). Requires ESP32 Arduino Core v3.x for `analogWrite()` support.

To compile and flash:
```
arduino-cli compile --fqbn esp32:esp32:esp32c3 01-blink
arduino-cli upload  --fqbn esp32:esp32:esp32c3 -p COM_PORT 01-blink
```

Serial Monitor (sketches 03–06): `arduino-cli monitor -p COM_PORT --config baudrate=115200`

---

## 01 — Blink

**New concepts:** `pinMode`, `digitalWrite`, `delay`, the setup/loop structure.

**Circuit:** LED + 330 Ω on GPIO 8.

The canonical first sketch. The two `delay(500)` values control on and off time independently — making them unequal is a useful first experiment.

---

## 02 — Fade

**New concepts:** `analogWrite`, PWM duty cycle (0–255), `for` loop.

**Circuit:** Same as 01.

Uses `analogWrite()` (LEDC-backed in Core v3). The 256-step range is an Arduino convention. The `for` loop increments `level` from 0 → 255 then back, with a short `delay()` between steps to control the fade speed.

The pauses at full and zero brightness (`delay(200)`) give the animation a natural rhythm — remove them to see the difference.

---

## 03 — Button

**New concepts:** `INPUT_PULLUP`, `digitalRead`, edge detection, Serial output.

**Circuit:** Button on GPIO 9.

The sketch prints only on state *change* (edge detection via `lastState`), not on every loop iteration. This keeps the Serial Monitor readable.

`INPUT_PULLUP` is the standard safe default for switches: pin held HIGH through internal resistor, pressed LOW by shorting to GND. Floating input pins (plain `INPUT`) give unreliable readings when nothing is driving the line.

---

## 04 — Button + LED

**New concepts:** Combining input and output, debouncing, toggle state.

**Circuit:** Button on GPIO 9, LED on GPIO 8.

Two versions in one file:

**Part A** (hold to light): simple conditional in `loop()` — LED mirrors button state.

**Part B** (toggle): edge-detect on the falling edge (HIGH → LOW transition), flip a `bool` state variable, write the new state to the LED. The `delay(20)` is a minimal software debounce — mechanical contacts bounce electrically for ~5–20 ms on press; without debounce, one physical press can register as several.

---

## 05 — Analogue Read

**New concepts:** `analogRead`, 12-bit ADC (0–4095), scaling to volts.

**Circuit:** Potentiometer wiper on GPIO 1.

GPIO 1 is on ADC1, which remains reliable when WiFi is active (ADC2 channels are unusable with WiFi on the ESP32). For the coding stream WiFi isn't running, but the habit is worth forming.

The voltage conversion `raw * 3.3f / 4095.0f` applies the nominal 3.3 V reference. For better accuracy use `analogReadMilliVolts()` (factory-calibrated, same as the meter firmware). The simpler formula is used here to keep the concept clear.

---

## 06 — Analogue to LED

**New concepts:** `map()`, connecting sensor range to output range.

**Circuit:** Potentiometer on GPIO 1, LED on GPIO 8.

`map(raw, 0, 4095, 0, 255)` performs a linear rescaling. Worth noting: `map()` uses integer arithmetic — for better precision at small values, scale to a wider intermediate range or use floating-point arithmetic manually.

The Serial output here is for verification, not required for function. Removing it speeds up the loop slightly.

---

## 07 — Tone (bonus)

**New concepts:** `tone()`, `noTone()`, frequency, arrays of structs.

**Circuit:** Passive buzzer on GPIO 10.

`tone(pin, freq, duration_ms)` uses LEDC internally. The note table (`TUNE[][2]`) is a 2D array — each row is `{frequency, duration}`. A frequency of 0 triggers `noTone()` for rests.

The `delay(dur + 30)` after each note includes a 30 ms gap. Without this the notes blur together — `tone()` with duration doesn't wait for the note to finish before returning.

**Active vs passive buzzer:** An active buzzer contains its own oscillator — it buzzes at one fixed pitch regardless of the frequency you drive it at. `tone()` only works with a passive buzzer (plain piezo element). Passive buzzers are usually a bare plastic disc or a small module with no IC on it.

---

## Suggested Extensions

| Idea | Concepts used |
|------|--------------|
| Reaction timer: random LED delay, button measures response time | `millis()`, random(), multiple I/O |
| Light sensor: LDR + analogue read controls LED brightness | Voltage divider, analogue read, map |
| Combination lock: button sequence unlocks LED | State machine, array comparison |
| Morse code sender: button → dots and dashes via tone | Timing, encoding |
| Traffic light: timed sequence on three LEDs | Multiple outputs, timing |
