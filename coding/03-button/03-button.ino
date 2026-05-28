// ─── 03 — Button ─────────────────────────────────────────────────────────────
//
// Reads a button and prints its state to the Serial Monitor.
//
// Hardware:
//   Button: one leg to GPIO 9, other leg to GND
//   No resistor needed — we use the built-in pull-up.
//
// INPUT_PULLUP connects the pin internally to 3.3 V through a resistor.
// When the button is open, the pin reads HIGH (3.3 V, nothing to pull it down).
// When the button is pressed, it connects the pin directly to GND — LOW wins.
// So: not pressed = HIGH, pressed = LOW. This feels backwards at first but
// becomes natural quickly.
//
// Open the Serial Monitor at 115200 baud to see the output.
// In arduino-cli: arduino-cli monitor -p COM_PORT --config baudrate=115200

const int BUTTON_PIN = 9;

int lastState = HIGH;   // track previous state so we only print on change

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Ready — press the button.");
}

void loop() {
    int state = digitalRead(BUTTON_PIN);

    if (state != lastState) {
        if (state == LOW) {
            Serial.println("Pressed");
        } else {
            Serial.println("Released");
        }
        lastState = state;
    }
}
