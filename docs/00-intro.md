# Getting Started with Microcontrollers

---

## Everything around you has a brain

Your phone is a computer. You probably knew that.

But so is your washing machine. And your TV remote. And the traffic light on the corner. And the smoke alarm on your ceiling. And the car your parents drive. And the router that gives your house WiFi. And most modern toys, most kitchen appliances, and increasingly, most things that plug in or run on batteries.

None of those devices have screens or keyboards. You can't type on a washing machine. But somewhere inside each one, there's a tiny chip — smaller than your thumbnail — running a program. Watching for inputs. Making decisions. Sending outputs. All day, every day, without you thinking about it.

That chip is called a **microcontroller**.

---

## What is a microcontroller?

A microcontroller is a computer on a single chip.

Not a powerful computer — it won't run games or browse the internet. But it doesn't need to. It just needs to do one job, reliably, forever. Read a sensor. Turn something on. Send a signal. Count something.

The interesting thing is how *small* and *cheap* they've become. The chip you're going to work with costs around £3. It has a processor, memory, WiFi, and 22 connections to the outside world — all on something roughly the size of a stick of chewing gum.

When chips that capable cost that little, they end up inside everything.

---

## Meet the ESP32-C3

The board you have is built around a chip called the **ESP32-C3**, made by a company called Espressif.

Have a look at it. You'll see:

- **The chip itself** — the small square component in the middle, probably with "ESP32-C3" printed on it. Everything important is inside here.
- **A USB connector** — this is how you power it and, the first time, load your program onto it.
- **A small antenna** — usually a bent trace of metal at one end of the board. This is the WiFi aerial.
- **Two rows of pins along the sides** — these are the connections to the outside world. Each one is a tiny metal leg you can attach wires to.

Those pins are the whole point.

---

## What are the pins for?

Each pin on the ESP32-C3 is a connection between the chip and the real world.

Some pins can be **inputs** — they listen. Connect a button and the chip can detect when it's pressed. Connect a sensor and the chip can read a temperature, a distance, a light level.

Some pins can be **outputs** — they speak. Set a pin HIGH and it puts 3.3 volts on that leg. Connect an LED and it lights up. Connect a buzzer and it beeps. Connect a relay and you can switch something bigger on or off.

You decide, in your program, which pins do what. That's the whole game.

> You might be wondering: if each pin is either 3.3V or 0V — on or off — how do you make something *dimmer* instead of just on or off? Good question. The answer involves something called PWM. You'll find out about it when you get to the sensor experiments.

---

## WiFi, built in

Here's what makes the ESP32-C3 unusual for its price: it has WiFi built directly into the chip.

That means your project can create its own wireless network — and serve a web page to any phone or laptop that connects to it. No router needed. No internet needed. Just the board, some power, and any browser.

That's how the projects in this kit work. The board runs a tiny web server. You connect your phone to its WiFi. Your browser loads a page directly from the chip. Everything talks over your local network without ever touching the internet.

---

## Why does any of this matter?

Because the world runs on this stuff.

The engineers who designed the traffic lights in your town used microcontrollers. The people who built your family's car used thousands of them. The teams making medical devices, satellites, home automation systems, agricultural sensors, industrial machines — all microcontrollers.

More importantly: these chips are now cheap enough, and the tools to program them are now accessible enough, that you don't need to be a professional engineer to use them. You just need curiosity and a bit of time.

Learning to work with a microcontroller means learning to think in the same way as the devices around you. That's a useful thing to be able to do.

---

## What you're going to build

There are two projects in this series.

**ESP32 Lab** comes first. You'll connect real sensors to the board — temperature, distance, light, motion — and read live data from them in a browser. No coding required to get started. The goal is to get comfortable with the hardware: how to wire things up, how to read the output, what happens when something goes wrong.

**ESP32 Meter** comes second. You'll build a simple test instrument on a breadboard — a device that can measure voltage, identify resistors, test diodes, and measure capacitors. You'll use it to understand the components you've been connecting. And if you want to go further, you can solder a permanent version of it.

After that, the path splits. You can go deeper into coding — writing your own Arduino sketches, controlling pins directly, building your own sensor projects. Or you can go deeper into hardware — learning to solder, building circuits that last.

Or both.

---

## Before you start

You don't need to know anything about electronics or programming. Everything you need is explained as you go.

You do need:
- The ESP32-C3 board (pre-flashed with firmware for the first project)
- A phone, tablet, or laptop with a browser
- A USB cable for power (the board charges like a phone — you're not connecting it to a computer, just a USB charger or power bank)

The breadboard, wires, and components you'll need for each project are listed at the start of that booklet.

---

*Ready? Start with the ESP32 Lab booklet.*

---

&nbsp;

---

# Appendix — What's in the kit

These are the components you'll come across. You don't need to memorise any of this — come back to it when you find yourself wondering "wait, what is this thing exactly?"

---

## Resistor

A resistor slows down the flow of electricity. The more resistance, the less current gets through.

Resistance is measured in **ohms** (symbol: Ω). A 330Ω resistor lets through less current than a 100Ω resistor.

You can read the value from the coloured bands printed on the body — each colour is a digit. (There are charts for this; you don't need to memorise them.)

Resistors are used everywhere: to protect LEDs from burning out, to set voltage levels, to pull signals up or down to a known state. You'll use several in the meter project.

---

## Capacitor

A capacitor stores a small amount of electrical charge, then releases it.

Think of it like a tiny rechargeable bucket. Fill it with electricity — it holds it. Remove the source — it drains out. The bigger the capacitor, the more it holds, and the longer it takes to fill and empty.

Capacitance is measured in **farads** (F), though most capacitors you'll encounter are measured in microfarads (µF) or nanofarads (nF) — tiny fractions of a farad.

Capacitors are used to smooth out power supplies, filter signals, and time circuits. The meter can measure them.

---

## Diode

A diode only lets electricity flow in one direction.

One way: current flows through. The other way: it blocks. This makes diodes useful for protecting circuits from accidentally reversed connections, and for converting AC power to DC.

A diode has two ends: the **anode** (+) and the **cathode** (−). Current flows from anode to cathode — but not the other way. There's usually a stripe on the body to mark the cathode end.

---

## LED

An LED is a diode that emits light when current flows through it.

LED stands for **Light Emitting Diode**. Like all diodes, it only works one way round — connect it backwards and nothing happens (though it won't be damaged by this). It has a longer leg (anode, +) and a shorter leg (cathode, −).

LEDs come in many colours. Each colour has a slightly different **forward voltage** — the voltage drop across it when it's on. Red LEDs drop around 1.8V; blue and white LEDs drop around 3V. The meter can measure this.

Always use a resistor in series with an LED. Without one, too much current flows and the LED burns out quickly — sometimes instantly.

---

## Zener diode

A Zener diode is a special type of diode that, when connected in reverse, breaks down at a precise voltage — and holds that voltage very steadily.

This makes them useful as voltage regulators and protectors. In the meter circuit, the Zener diode protects the ESP32's input pin from voltages that would otherwise damage it.

---

## Breadboard

A breadboard is a plastic board with hundreds of small holes, connected underneath by metal strips. You push component legs and wire ends into the holes to make connections — no soldering needed.

The holes are connected in short rows across the middle, and in long columns along the top and bottom edges (the power rails). Learning to read a breadboard diagram — which holes are connected to which — is one of the first skills in electronics.

---

## Crocodile clip leads

Short cables with a spring-loaded metal clip at each end. You clip them onto component legs, wire ends, or anything metal to make a quick temporary connection. Named for the shape of the clip.

In the meter project, these are your test probes — the red clip goes to the + terminal of whatever you're measuring, and the black clip to the − terminal.
