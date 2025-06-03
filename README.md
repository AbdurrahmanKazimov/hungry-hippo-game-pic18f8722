# Hungry Hippo Game on PIC18F8722

This project implements a simple "Hungry Hippo" game on the PIC18F8722 microcontroller. The game uses interrupts and a 7-segment display to simulate a hippo trying to reach a blinking prize on an LED strip.

## Features

- Interrupt-driven user input (button on PORTB0).
- Timer0 interrupt for blinking prize, gravity effect, and score degradation.
- 7-segment display shows the total score.
- Soft reset and hard reset mechanisms for hippo size.
- LED strip on PORTD represents the game board and hippo's size and position.

## Hardware

- PIC18F8722 microcontroller @ 40 MHz
- LEDs connected to PORTD
- 7-segment displays driven by PORTH and PORTJ
- Input button on PORTB0

## How to Use

- Upload the code to the PIC18F8722.
- Press the button to move the hippo up.
- Gravity pulls the hippo down every 350 ms.
- The hippo grows when reaching the blinking prize.
- The score is shown on 7-segment displays.

## Files

- `hippo_game.c`: Source code for the game.

