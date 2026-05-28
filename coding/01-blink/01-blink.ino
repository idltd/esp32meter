// ─── 01 — Blink ──────────────────────────────────────────────────────────────
//
// Turns an LED on and off once per second.
//
// Hardware:
//   GPIO 8 → 330 Ω resistor → LED (long leg) → GND (short leg)
//
// This is the "hello world" of hardware programming. If you can make an LED
// blink, you have confirmed that your board, your wiring, and your code are
// all working.
//
// Try changing the two delay() values. What happens if you make them very
// different from each other? What happens if you make them both very small?

const int LED_PIN = 8;

void setup() {
    // Tell the ESP32 that this pin is an output — it will drive voltage,
    // not read it.
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);   // 3.3 V on the pin — LED on
    delay(500);                    // wait 500 ms (half a second)
    digitalWrite(LED_PIN, LOW);    // 0 V on the pin — LED off
    delay(500);
}
