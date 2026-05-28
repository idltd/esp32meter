// ─── 06 — Analogue to LED ────────────────────────────────────────────────────
//
// Turn the potentiometer to change the LED brightness.
// This connects an analogue input directly to an analogue output.
//
// Hardware:
//   Potentiometer: left outer → GND, right outer → 3V3, wiper → GPIO 1
//   GPIO 8 → 330 Ω resistor → LED → GND
//
// The challenge: analogRead() returns 0–4095, but analogWrite() takes 0–255.
// map() rescales one range to another:
//   map(value, fromLow, fromHigh, toLow, toHigh)

const int POT_PIN = 1;
const int LED_PIN = 8;

void setup() {
    Serial.begin(115200);
}

void loop() {
    int raw        = analogRead(POT_PIN);
    int brightness = map(raw, 0, 4095, 0, 255);

    analogWrite(LED_PIN, brightness);

    Serial.print("ADC: ");
    Serial.print(raw);
    Serial.print("\t Brightness: ");
    Serial.println(brightness);

    delay(30);
}
