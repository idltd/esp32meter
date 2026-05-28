// ─── 05 — Analogue Read ──────────────────────────────────────────────────────
//
// Reads a continuously varying voltage and prints the value.
//
// Hardware option A — potentiometer (best):
//   Left outer leg  → GND
//   Right outer leg → 3V3
//   Middle leg (wiper) → GPIO 1
//
// Hardware option B — no potentiometer:
//   Short wire from GPIO 1 (just touch it with a damp finger to see a reading)
//
// analogRead() returns 0–4095:
//   0    = 0 V at the pin
//   4095 = 3.3 V at the pin
//
// The ESP32's ADC has 12 bits of resolution: 2^12 = 4096 possible values.
// A standard Arduino Uno only has 10 bits (0–1023). More bits = more precision.
//
// Open the Serial Monitor at 115200 baud.

const int POT_PIN = 1;

void setup() {
    Serial.begin(115200);
    Serial.println("Analogue read ready. Turn the potentiometer.");
    Serial.println("---");
}

void loop() {
    int   raw   = analogRead(POT_PIN);
    float volts = raw * 3.3f / 4095.0f;

    Serial.print("Raw: ");
    Serial.print(raw);
    Serial.print("\t Volts: ");
    Serial.print(volts, 3);
    Serial.println(" V");

    delay(100);
}
