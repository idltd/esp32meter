# ESP32 Meter — Build Your Own Test Instrument

---

## What you're going to build

A multimeter. A real one.

Not a toy version — an actual instrument that electricians and engineers use every day. Yours will measure voltage, resistance, capacitance, and test diodes and LEDs. You'll build it yourself from scratch on a breadboard, and it'll work through your browser, same as the lab.

The difference from the lab project is that here, *you* build the circuit. The ESP32 doesn't come with measurement built in — it's just a chip with some inputs and outputs. The resistors and components you add around it are what turn it into a meter.

Understanding why each component is there is half the point.

---

## What you'll need

**The ESP32-C3 board** (the same one from the lab kit, or a new one)

**Resistors:**

| Value | Quantity | What it's for |
|-------|----------|---------------|
| 180 kΩ | 1 | Top of the voltage divider |
| 20 kΩ | 1 | Bottom of the voltage divider |
| 10 kΩ | 2 | ADC protection + capacitor charge |
| 1 kΩ | 1 | Mid-range resistance reference |
| 100 Ω | 1 | Low-range resistance reference + diode test |

**Other components:**

| Part | Quantity | What it's for |
|------|----------|---------------|
| 3.6 V Zener diode | 1 | Protects the voltage input |
| 100 nF capacitor | 1 | Keeps the power supply steady |
| Crocodile clip leads | 2 | Red (+) and black (−) test probes |
| Breadboard | 1 | The building surface |
| Jumper wires | Several | Connecting things up |

> You might be wondering: why does a resistance meter need reference resistors? Good question. Come back to that after you've built it and tried it out — the answer will make more sense then.

---

## Getting the firmware

The ESP32 Meter firmware needs to be loaded onto the board. If you've already done this — you have the `ESP32Meter_XXXX` WiFi network when the board is plugged in — you're ready to go.

If you're moving from the Lab project and want to keep using the same board:

1. Download the ESP32 Meter `.bin` file
2. Connect to your lab board's WiFi (`ESP32Lab_XXXX`)
3. Open the browser, go to the **System** tab, scroll to **Firmware Update**
4. Upload the meter `.bin` file
5. The board restarts with the meter firmware

You haven't broken the lab firmware — you can always flash it back the same way.

---

## Building the circuit

The circuit has three separate sections. Build them one at a time, test as you go.

### Place the ESP32-C3

Put the ESP32-C3 on the breadboard so it straddles the centre gap — one row of pins on each side. Leave a few columns of space on both sides for the external components.

### Set up the power rails

Connect the board's **3V3 pin** to the **+** power rail running along the top of the breadboard.

Connect one of the board's **GND pins** to the **−** rail.

Then place the 100 nF capacitor across the two rails close to the board (one leg in +, one leg in −). This smooths out any wobbles in the power supply and helps the ADC give steady readings.

---

### Section 1 — Voltage measurement

This section lets the meter measure voltages up to 33 V without damaging the ESP32. The ESP32 can only handle 3.3 V on its pins — if you connected a 9 V battery directly, it would break the chip. So you build a **voltage divider** that shrinks the incoming voltage down to a safe level.

The divider uses two resistors: 180 kΩ and 20 kΩ. Together they shrink the voltage by a factor of 10 — so 33 V becomes 3.3 V, which the chip can safely read.

**Wire it up:**

1. Place the **180 kΩ resistor** on the breadboard. One leg connects to an empty row — call this the **voltage input node**. The other leg will connect to your red probe.
2. Place the **20 kΩ resistor** with one leg at the voltage input node and the other leg in the **−** rail (GND).
3. Place the **3.6 V Zener diode** with its **cathode** (the leg marked with a stripe) at the voltage input node, and its **anode** at the **−** rail.
4. Run a jumper wire from the **voltage input node** to **GPIO 0** on the ESP32-C3.

The Zener diode is a safety net. If you accidentally connect a voltage higher than 3.6 V directly to the node, the Zener clamps it before it reaches the chip. Think of it as a pressure relief valve.

**Your red voltage probe** (crocodile clip) will connect to the far end of the 180 kΩ resistor — the side that faces away from the ESP32. Your black probe connects to GND.

---

### Section 2 — Resistance, continuity and diode testing

This section uses three reference resistors to measure unknown resistances. The idea: you already know the value of your reference. By measuring the voltage at the point where your unknown component and the reference meet, you can work out the unknown.

**Wire it up:**

1. Pick a clear area of breadboard. This section has a central **measurement node** — a row where several things connect. Call it the **probe node**.

2. Place the **100 Ω resistor** with one leg at the probe node and the other leg jumping to **GPIO 3** on the ESP32.

3. Place the **1 kΩ resistor** with one leg at the probe node and the other leg jumping to **GPIO 4**.

4. Place the **10 kΩ reference resistor** with one leg at the probe node and the other leg jumping to **GPIO 5**.

5. Place the **10 kΩ protect resistor** with one leg at the probe node and the other leg jumping to **GPIO 1**.

   (This protect resistor sits between the probe node and the ADC input. It limits the current if you accidentally touch the probes to something live — protecting the chip.)

6. Run a jumper from the probe node to **GPIO 2** as well. (This one connects directly, with no protection resistor — it's the capacitance ADC input, which needs to read the voltage quickly.)

7. Run another jumper from **GPIO 6** through the **second 10 kΩ resistor** to the probe node. (This is the capacitance charge circuit — more on that in a moment.)

**Your red resistance/diode/cap probe** connects to the probe node. **Black probe** connects to GND.

---

### Section 3 — Capacitance

Good news: you've already built it. The GPIO 6 → 10 kΩ → probe node connection in Section 2 is the capacitance charging circuit. When measuring a capacitor, the meter drives GPIO 6 high, and current flows through the 10 kΩ resistor to charge the capacitor. GPIO 2 watches for when it reaches 63% of full charge — and the time that takes tells you the capacitance.

No extra components needed. Everything shares the same probe node.

---

### Check your wiring

Before powering up, go through this list:

- [ ] 3V3 pin → + power rail
- [ ] GND pin → − power rail
- [ ] 100 nF capacitor across the power rails
- [ ] 180 kΩ → voltage input node; 20 kΩ from node to GND; Zener cathode at node
- [ ] Voltage input node → GPIO 0
- [ ] 100 Ω from probe node → GPIO 3
- [ ] 1 kΩ from probe node → GPIO 4
- [ ] 10 kΩ (reference) from probe node → GPIO 5
- [ ] 10 kΩ (protect) from probe node → GPIO 1
- [ ] Direct wire from probe node → GPIO 2
- [ ] 10 kΩ (charge) from GPIO 6 → probe node

---

## Powering it up

Plug the board into USB power. Wait a few seconds.

On your phone or laptop, look for a WiFi network called **ESP32Meter_XXXX**.

Connect to it (no password needed).

Open a browser and go to **http://192.168.4.1**

You should see the ESP32 Meter interface. Four measurement cards: Voltage, Resistance, Diode, and Capacitance. They'll show dashes until you connect the probes to something.

---

## Using the meter

### Measuring voltage

Connect the **red probe to the + terminal** of something and the **black probe to − or GND**.

The voltage card updates every two seconds.

**Things to try:**

- A AA battery: should read around 1.5 V (or a bit less if it's been used)
- Two AA batteries in series: around 3 V
- A 9 V battery: around 9 V
- The 3.3 V output of the ESP32 itself (the 3V3 pin to GND): should read 3.3 V

> The meter can read up to 33 V safely. Don't connect anything above that — the Zener can only protect up to a point.

Notice: the reading changes as soon as you connect or disconnect. It doesn't wait for you to press anything. That's because voltage measurement is *passive* — the circuit doesn't do anything to what you're measuring, it just observes.

---

### Measuring resistance

Connect the **red probe to one leg of a resistor** and the **black probe to the other leg**.

The resistance card updates every two seconds. It auto-ranges — starts with the 10 kΩ reference and switches down if your resistor is small.

**Things to try:**

- Measure some of the spare resistors from the kit. Look at the colour bands — is the reading close to what the colours say?
- Measure your finger (skin-to-skin across two fingers). You're around 50,000–500,000 Ω depending on how dry your hands are.
- Measure a pencil line. Graphite conducts slightly.
- Measure a short piece of wire. Should read very close to 0 Ω.

**Reading colour codes:**

Resistors have coloured bands that tell you their value. The first two or three bands are digits; the next is a multiplier. There are reference charts for these — you don't need to memorise them — but after a while the common ones (red-red-brown = 220 Ω, yellow-violet-red = 4.7 kΩ, brown-black-orange = 10 kΩ) start to look familiar.

> You just built three known resistors (100 Ω, 1 kΩ, 10 kΩ) into the meter circuit. Measure each one from your spare parts against what the colour code says. How accurate is the meter?

---

### Continuity testing

If resistance is below 50 Ω, the resistance card turns green and shows **CONTINUITY**.

This is one of the most useful things a meter does. A continuity check tells you if two points in a circuit are electrically connected. If they're supposed to be connected and aren't, you have a broken wire or a bad joint. If they're *not* supposed to be connected and are, you have a short circuit.

**Things to try:**

- Touch both probes to the same wire or breadboard row. Continuity.
- Touch one probe to the + rail and one to the − rail. No continuity — they're not connected. (If they *were*, you'd have a short circuit and the board might not work.)
- Touch the probes to two ends of a jumper wire. Continuity.
- Deliberately leave a connection slightly out of a breadboard hole and probe each end.

---

### Testing diodes

Connect the **red probe to the anode (longer leg)** and the **black probe to the cathode (shorter leg, stripe end)**.

The diode card shows the **forward voltage** (Vf) — the voltage drop across the diode when current flows through it — and identifies the type.

**Things to try:**

- Test the Zener diode from the kit: it should read as a silicon diode (Vf around 0.6–0.7 V) — because forward-biased, a Zener behaves just like a regular diode.
- Try reversing the probes. The reading should jump to around 3.3 V and show "Open or reversed." Current doesn't flow through a diode backwards.
- If you have a spare LED, test it. Blue and white LEDs should show Vf around 2.8–3.2 V. Red LEDs around 1.8–2.0 V.

> Why do different coloured LEDs have different voltages? It comes down to the semiconductor material used to make them. Each colour uses a different material with a different energy gap — and the voltage you measure is directly related to that energy. Blue and white LEDs use gallium nitride; red and yellow use different materials altogether. The colour of light and the voltage are connected.

---

### Testing LEDs

LEDs are diodes, so the diode test works for them. But there's more to see.

Connect the LED with its **longer leg (anode) to the red probe** and **shorter leg (cathode) to the black probe**. The LED will light up faintly during the test — the meter drives just enough current through it.

**Things to try:**

- Test every colour LED you have. Note the Vf for each.
- Does the Vf reading match the colour as described above?
- Reverse the probes. Does the LED light up? (It shouldn't — that's the whole point of a diode.)
- Connect a red LED and a blue LED in series (cathode of first to anode of second). What does the meter read? What does 'in series' do to voltages?

---

### Measuring capacitors

Press the **Measure** button in the capacitance card.

The meter discharges the capacitor, then charges it through a 10 kΩ resistor, and times how long it takes to reach 63% of full charge. That time divided by the resistance gives the capacitance — a method called RC timing.

**Things to try:**

- Measure the 100 nF (0.1 µF) decoupling capacitor from the kit. The reading should be close to 100 nF (though small capacitors are harder to measure accurately).
- If you have any other capacitors lying around, measure them and compare to the markings on the body.
- What happens if you press Measure with nothing connected?

> Why 63%? That number comes from the mathematics of how capacitors charge. After exactly one *RC time constant* (resistance × capacitance), the voltage across the capacitor is always at 63.2% of its final value — no matter what the values are. That's not a coincidence or an engineering choice; it falls out of the equations naturally. You'll encounter this number again if you go further in electronics.

---

## What you've learned

Look at what you've built and what you've been measuring:

The **voltage divider** uses two resistors in a ratio to scale down a voltage. That same technique is used everywhere: to set bias voltages in amplifiers, to create reference levels, to interface sensors that run at different voltages.

The **reference resistor network** compares an unknown resistance against known values. The ratio of voltages tells you the ratio of resistances. Simple maths, but it's the same principle at the heart of a professional bench multimeter.

The **RC timing** for capacitance is a direct application of a fundamental law of electronics. The time constant τ = RC is one of the first formulas you'd see in an electronics course.

None of these are tricks or simplifications. This is how it actually works.

---

## What's next?

You've built a working instrument. You understand — at least roughly — what each component does and why it's there. That puts you ahead of most people who use electronics every day.

From here:

---

**→ The hardware stream**

If you want to build something permanent, start with the soldering guide. You'll solder the meter circuit onto stripboard — a permanent board that keeps everything in place. The ESP32 sits in a socket so you can still move it to other projects.

Soldering takes practice. The first joints aren't always pretty. But after a few you'll be making reliable connections that last for years, and you'll have something you built with your hands that actually works.

---

The meter will be useful wherever you take it next.
