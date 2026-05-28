// ─── 02 — Fade ───────────────────────────────────────────────────────────────
//
// Fades an LED in and out smoothly.
//
// Hardware:
//   GPIO 8 → 330 Ω resistor → LED (long leg) → GND (short leg)
//   Same circuit as 01-blink — no changes needed.
//
// How it works:
//   analogWrite(pin, value) sets a PWM duty cycle from 0 (off) to 255 (full on).
//   The pin switches on and off thousands of times per second. The higher the
//   duty value, the longer it stays on each cycle, and the brighter the LED
//   appears. Your eyes average it out — you see brightness, not flicker.
//
// Try changing the delay inside the for loops.
// Try making it pause at full brightness before fading out.

const int LED_PIN = 8;

void setup() {
    // analogWrite handles its own pin setup — no pinMode needed.
}

void loop() {
    // Fade in: step from 0 (off) to 255 (full brightness)
    for (int level = 0; level <= 255; level++) {
        analogWrite(LED_PIN, level);
        delay(6);
    }

    // Pause at full brightness
    delay(200);

    // Fade out: step back down to 0
    for (int level = 255; level >= 0; level--) {
        analogWrite(LED_PIN, level);
        delay(6);
    }

    // Pause in the dark
    delay(200);
}
