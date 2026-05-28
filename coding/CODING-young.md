# Coding with the ESP32-C3 — Let's Write Some Programs

---

## You're going to control real things with code

Not on a screen. Not in a simulation. An actual LED that lights up because of something you wrote.

That's the difference between software and hardware programming. The code you write here changes the physical world.

---

## What you need on your breadboard

You'll build this up step by step. Start with just the LED for the first two sketches.

### The LED (sketches 01 and 02)

```
ESP32-C3 GPIO 8  →  330 Ω resistor  →  LED long leg (+)
                                        LED short leg (−)  →  GND
```

The resistor protects the LED. Without it, too much current flows and the LED burns out — sometimes in under a second.

### Add a button (sketches 03 and 04)

```
ESP32-C3 GPIO 9  →  button leg 1
                    button leg 2  →  GND
```

No resistor needed here — you'll use the ESP32's internal pull-up.

### Add a potentiometer (sketches 05 and 06)

A potentiometer is a dial with three legs:

```
Left leg   →  GND
Right leg  →  3V3
Middle leg →  GPIO 1
```

Turning the dial changes the voltage at the middle leg from 0 V to 3.3 V.

### Add a buzzer (sketch 07 — bonus)

```
ESP32-C3 GPIO 10  →  passive buzzer  →  GND
```

Must be a **passive** buzzer (not an active one). See the sketch for notes on telling them apart.

---

## How to load a sketch

1. Open `01-blink/01-blink.ino` in the Arduino IDE (or use arduino-cli)
2. Select your board: **ESP32C3 Dev Module** under *esp32 by Espressif Systems*
3. Select the COM port your board is on
4. Click Upload (the → arrow)
5. Watch the LED

If you're using arduino-cli from the terminal:
```
arduino-cli compile --fqbn esp32:esp32:esp32c3 01-blink
arduino-cli upload  --fqbn esp32:esp32:esp32c3 -p COM5 01-blink
```

---

## Sketch 01 — Blink

**What it does:** Turns the LED on for half a second, off for half a second, forever.

**Upload it, watch the LED blink.**

**Things to try:**

- Change both `delay(500)` values to `delay(100)`. How does it look?
- Make the first delay much longer than the second. What pattern do you get?
- Change it to blink three times quickly, then pause. (Hint: you'll need more `digitalWrite` and `delay` lines.)

> `delay(500)` pauses the program for 500 milliseconds. The chip does nothing during a delay — it just waits. This works fine for simple blink, but later you'll learn why it becomes a problem in more complex programs.

---

## Sketch 02 — Fade

**What it does:** Fades the LED in and out smoothly using PWM.

**Same circuit as sketch 01. Upload it.**

**Things to try:**

- Change `delay(6)` inside the for loops to `delay(1)`. The fade becomes almost instant.
- Change it to `delay(20)`. Now it's a slow, dramatic fade.
- Make it fade in quickly and fade out slowly (different delays in each loop).
- Can you make it "breathe" — slow in, slow out, like a sleeping indicator light?

> `for (int level = 0; level <= 255; level++)` — this is a *for loop*. It runs the code inside it 256 times, with `level` going from 0 up to 255. Each time around, `level` is one higher. `level++` means "add one to level."

---

## Sketch 03 — Button

**What it does:** Tells you in the Serial Monitor when you press and release the button.

**Add the button to your breadboard. Upload the sketch.**

**Open the Serial Monitor** to see the output. In the Arduino IDE: Tools → Serial Monitor (set to 115200 baud). In arduino-cli: `arduino-cli monitor -p COM5 --config baudrate=115200`

**Things to try:**

- Press the button quickly many times. Does it catch every press?
- Hold it down. What happens in the Serial Monitor?
- What does `INPUT_PULLUP` do? Try changing it to `INPUT` and see what the pin reads when nothing is connected. (Floating pins give random readings — this is why pull-ups exist.)

> Notice that pressed = LOW (not HIGH). This feels backwards. It's because `INPUT_PULLUP` holds the pin at HIGH when nothing pulls it down. Pressing the button connects the pin to GND, which pulls it to LOW. Most buttons and switches in real hardware work this way.

---

## Sketch 04 — Button + LED

**What it does:** Part A — hold the button to light the LED. Part B — press to toggle.

**Same circuit as before — both button and LED connected.**

The sketch starts in Part B (toggle) mode. To try Part A first, uncomment the Part A section and comment out Part B. (In the Arduino IDE: select the Part A code, press Ctrl+/ to toggle comments.)

**Things to try:**

- In Part A: hold the button. Release it. Does the LED follow exactly?
- In Part B: press the button once. Press it again. Notice it stays on or off between presses.
- Try removing the `delay(20)` in Part B. Press the button quickly — does it still toggle reliably? (That delay is *debouncing* — mechanical buttons bounce electrically for a few milliseconds when pressed.)
- **Challenge:** can you make the LED blink while it's "on", and stay dark while it's "off"? (Hint: you'll need to combine the fade loop with a button check.)

---

## Sketch 05 — Analogue Read

**What it does:** Reads the potentiometer position and prints numbers to the Serial Monitor.

**Add the potentiometer. Upload the sketch. Open the Serial Monitor.**

Turn the potentiometer. Watch the numbers change from 0 (one end) to 4095 (other end).

**Things to try:**

- Can you find the exact centre position where the raw reading is closest to 2047?
- The sketch also prints the voltage in volts. At the centre position, is it close to 1.65 V (half of 3.3 V)?
- Try using a wire from GPIO 1 to 3V3 — what does it read? Wire to GND?
- **No potentiometer?** Touch GPIO 1 with a damp finger. You'll see a reading somewhere in the middle (skin resistance + the pin's internal pull-up creates a voltage divider).

> `analogRead()` gives you 4096 possible values (0 to 4095). That's 12 *bits* of resolution — the chip is measuring in steps of about 0.8 millivolts. A standard Arduino Uno only has 10 bits (0–1023). Neither is "wrong" — more bits means finer detail.

---

## Sketch 06 — Analogue to LED

**What it does:** Turning the potentiometer directly controls the LED brightness.

**Same circuit — potentiometer and LED both connected. Upload and try it.**

**Things to try:**

- Turn the potentiometer all the way to each end. What happens at the extremes?
- The Serial Monitor shows both the raw ADC value and the brightness value. At ADC = 2047, what is the brightness value? Why?
- **Challenge:** make the relationship inverse — fully clockwise = off, anticlockwise = full brightness. (Hint: `map(raw, 0, 4095, 255, 0)`)
- **Challenge:** add the button from sketch 04. Button toggles whether the potentiometer controls brightness or not.

> `map(raw, 0, 4095, 0, 255)` rescales a number from one range to another. It's one of the most useful functions in Arduino. You'll find yourself using it constantly to connect sensors (which output one range) to outputs (which need a different range).

---

## Sketch 07 — Tone (bonus)

**What it does:** Plays a tune through a buzzer. Requires a passive piezo buzzer.

**Add the buzzer: one leg to GPIO 10, other to GND. Upload the sketch.**

The ESP32 will play Twinkle Twinkle Little Star on repeat.

**Things to try:**

- Change a few of the frequencies to different values. What makes a note sound higher or lower?
- Add a few more notes. The comments in the sketch list the frequencies for a full octave.
- Change the duration values. What does doubling all of them do?
- **Challenge:** edit the TUNE array to play a different song. Happy Birthday? Mary Had a Little Lamb?
- **Challenge:** add the button from sketch 03. Button press plays one note; silence when released.

> Musical notes are just frequencies — vibrations per second, measured in Hertz. Middle A is 440 Hz — the string vibrates 440 times per second. Double the frequency and you go up an octave. This is physics, not music theory: the relationship between notes is mathematical, and has been since ancient Greece.

---

## Where next?

You've gone from blinking an LED to controlling brightness from a dial and making sounds from code. The components are simple but the ideas are fundamental — inputs, outputs, digital, analogue, timing, state.

Everything in electronics is some combination of what you've just done.

**Ideas to build from here:**

- A reaction timer: LED lights up at a random time, button press measures how fast you reacted
- A light sensor (use the analogue input with a light-dependent resistor instead of a potentiometer) that turns on an LED when it gets dark
- A combination lock: press the button in a specific pattern to turn on the LED
- Morse code: press the button and send dots and dashes

The components haven't changed. Only what the code does with them.
