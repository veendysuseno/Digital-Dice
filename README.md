# Digital Dice

This project demonstrates a digital dice using an 8x8 Dot Matrix display and a vibration sensor. The dice will display random numbers between 1 and 6 when the vibration sensor is triggered.

## Components Used

- Arduino Uno
- 8x8 Dot Matrix Display
- Vibration Sensor
- 74HC595 Shift Registers (for controlling the Dot Matrix)
- Jumper Wires
- Breadboard

## Pin Configuration

- **GND_CLOCK**: Pin 2
- **GND_LATCH**: Pin 3
- **GND_DATA**: Pin 4
- **VCC_CLOCK**: Pin 10
- **VCC_LATCH**: Pin 9
- **VCC_DATA**: Pin 8
- **PIN_VIBRATION**: Pin 12

## Dice Shapes

The dice displays different shapes for numbers 1 through 6:

- **One Shape**: Single dot in the center.
- **Two Shape**: Two dots on opposite corners.
- **Three Shape**: Three dots in a diagonal pattern.
- **Four Shape**: Four dots in each corner.
- **Five Shape**: Five dots with the center and four corners.
- **Six Shape**: Six dots arranged in two vertical columns.

## Code Overview

```cpp
#include <Arduino.h>

// Pin Definitions
const int GND_CLOCK = 2;
const int GND_LATCH = 3;
const int GND_DATA = 4;
const int VCC_CLOCK = 10;
const int VCC_LATCH = 9;
const int VCC_DATA = 8;
const int PIN_VIBRATION = 12;

// Dice shapes (1-6)
const byte DICE_SHAPES[][8] = {
    { 0b00000000, 0b00000000, 0b00111100, 0b00111100, 0b00111100, 0b00111100, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000000, 0b11100111, 0b11100111, 0b11100111, 0b00000000, 0b00000000, 0b00000000 },
    { 0b00000000, 0b00000011, 0b00000011, 0b00011000, 0b00011000, 0b11000000, 0b11000000, 0b00000000 },
    { 0b11100111, 0b11100111, 0b11100111, 0b00000000, 0b00000000, 0b11100111, 0b11100111, 0b11100111 },
    { 0b11000011, 0b11000011, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b11000011, 0b11000011 },
    { 0b11000011, 0b11000011, 0b00000000, 0b11000011, 0b11000011, 0b00000000, 0b11000011, 0b11000011 }
};

const int DICE_ANIMATION_DURATION = 3000;
const int DICE_ANIMATION_CHANGE = 80;

int active_digit = 0;

void setup() {
    pinMode(GND_CLOCK, OUTPUT);
    pinMode(GND_LATCH, OUTPUT);
    pinMode(GND_DATA, OUTPUT);
    pinMode(VCC_CLOCK, OUTPUT);
    pinMode(VCC_LATCH, OUTPUT);
    pinMode(VCC_DATA, OUTPUT);
    pinMode(PIN_VIBRATION, INPUT_PULLUP);
    randomSeed(analogRead(A0));
}

void loop() {
    if (digitalRead(PIN_VIBRATION) == LOW) {
        dice_animation();
        active_digit = random(6);
    }

    update_dice();
}

void dice_animation() {
    long dice_animation_timer = millis() + DICE_ANIMATION_DURATION;
    while (dice_animation_timer > millis()) {
        for (int i = 0; i < 6; i++) {
            active_digit = i;
            long dice_transition_timer = millis() + DICE_ANIMATION_CHANGE;
            while (dice_transition_timer > millis()) {
                update_dice();
            }
        }
    }
}

void update_dice() {
    for (int i = 0; i < 8; i++) {
        digitalWrite(VCC_LATCH, LOW);
        shift_register_input(VCC_DATA, VCC_CLOCK, (1 << i), VCC_REGISTER);
        digitalWrite(VCC_LATCH, HIGH);

        digitalWrite(GND_LATCH, LOW);
        shift_register_input(GND_DATA, GND_CLOCK, DICE_SHAPES[active_digit][i], GND_REGISTER);
        digitalWrite(GND_LATCH, HIGH);

        delay(2); // Allow time for LED to light up (multiplexing)

        digitalWrite(GND_LATCH, LOW);
        shift_register_input(GND_DATA, GND_CLOCK, 0, GND_REGISTER);
        digitalWrite(GND_LATCH, HIGH);
    }
}

void shift_register_input(int data_pin, int clock_pin, byte shape, int type) {
    for (int j = 0; j < 8; j++) {
        if (type == VCC_REGISTER) {
            digitalWrite(data_pin, (shape & (1 << VCC_MAPPING[j])));
        } else {
            digitalWrite(data_pin, !(shape & (1 << GND_MAPPING[j])));
        }
        digitalWrite(clock_pin, LOW);
        digitalWrite(clock_pin, HIGH);
    }
}
```

## How to Use

1. Connect the 8x8 Dot Matrix display and the vibration sensor to the specified pins on the Arduino.
2. Upload the code to your Arduino.
3. Shake or tap the vibration sensor to trigger the dice animation.
4. Observe the random number displayed on the Dot Matrix display.

## Notes

- Ensure that all connections are secure to avoid any signal issues.
- The dice shape display uses multiplexing to control the 8x8 matrix. Adjust DICE_ANIMATION_DURATION and DICE_ANIMATION_CHANGE for different animation effects.
