// ─── 04 — Button + LED ───────────────────────────────────────────────────────
//
// Hold the button to light the LED. Release to turn it off.
// Then: press once to toggle — LED stays on or off between presses.
//
// Hardware:
//   GPIO 8 → 330 Ω resistor → LED → GND
//   GPIO 9 → button → GND

const int LED_PIN    = 8;
const int BUTTON_PIN = 9;

// ── Part A: hold to light ────────────────────────────────────────────────────
// Uncomment this version and comment out Part B to try it first.
/*
void setup() {
    pinMode(LED_PIN,    OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        digitalWrite(LED_PIN, HIGH);   // button held: LED on
    } else {
        digitalWrite(LED_PIN, LOW);    // button released: LED off
    }
}
*/

// ── Part B: press to toggle ───────────────────────────────────────────────────
// Each press flips the LED between on and off.
// The tricky part: you need to act on the moment the button is pressed
// (the falling edge), not while it is held. Otherwise one press fires many times.

bool     ledState  = false;
int      lastState = HIGH;

void setup() {
    pinMode(LED_PIN,    OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    int state = digitalRead(BUTTON_PIN);

    // Detect the falling edge — the moment the button goes from HIGH to LOW
    if (lastState == HIGH && state == LOW) {
        ledState = !ledState;                       // flip
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        delay(20);                                  // debounce: ignore bounce noise
    }

    lastState = state;
}
