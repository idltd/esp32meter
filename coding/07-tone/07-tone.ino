// ─── 07 — Tone ───────────────────────────────────────────────────────────────
//
// Plays musical notes through a buzzer.
//
// Hardware:
//   Passive buzzer (piezo): one leg to GPIO 10, other leg to GND
//   A small speaker with a 100 Ω resistor in series also works.
//
//   NOTE: Use a *passive* buzzer — it needs the ESP32 to drive it at a
//   frequency. An *active* buzzer has its own oscillator and just buzzes at
//   one fixed pitch no matter what frequency you send — not useful here.
//   Passive buzzers are usually black, slightly see-through, and have no
//   circuit board on them.
//
// tone(pin, frequency)       — start a tone at the given frequency in Hz
// tone(pin, frequency, ms)   — play it for a fixed duration
// noTone(pin)                — stop the tone
//
// Musical notes as frequencies (middle octave):
//   C4 = 262 Hz,  D4 = 294,  E4 = 330,  F4 = 349
//   G4 = 392,     A4 = 440,  B4 = 494,  C5 = 523

const int BUZZER_PIN = 10;

// A simple tune: Twinkle Twinkle (first line)
// Each entry is {frequency_hz, duration_ms}. Frequency 0 = rest.
const int TUNE[][2] = {
    {262, 400},  // C  — Twin-
    {262, 400},  // C  — -kle
    {392, 400},  // G  — twin-
    {392, 400},  // G  — -kle
    {440, 400},  // A  — lit-
    {440, 400},  // A  — -tle
    {392, 800},  // G  — star
    {  0, 200},  //    — (rest)
    {349, 400},  // F  — how
    {349, 400},  // F  — I
    {330, 400},  // E  — won-
    {330, 400},  // E  — -der
    {294, 400},  // D  — what
    {294, 400},  // D  — you
    {262, 800},  // C  — are
    {  0, 400},  //    — (rest)
};

const int NOTE_COUNT = sizeof(TUNE) / sizeof(TUNE[0]);

void setup() {
    // nothing to set up
}

void loop() {
    for (int i = 0; i < NOTE_COUNT; i++) {
        int freq = TUNE[i][0];
        int dur  = TUNE[i][1];

        if (freq == 0) {
            noTone(BUZZER_PIN);
        } else {
            tone(BUZZER_PIN, freq, dur);
        }

        // Small gap between notes so they sound separate
        delay(dur + 30);
    }

    // Pause before repeating
    noTone(BUZZER_PIN);
    delay(1500);
}
