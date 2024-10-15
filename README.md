# Reaction Time Game with Arduino

A reaction time game where an LED lights up, and the player must move the joystick as quickly as possible in the correct direction. The player's reaction time and score are displayed on an LCD screen.

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Installation](#installation)
- [Installation](#installation)
- [Usage](#usage)

## Project Overview
The reaction time game randomly lights up an LED, and the player must quickly move the joystick in the corresponding direction. The game records the player's reaction time and updates the score after each round, displayed on a 1602 I2C LCD screen.

## Features
- Randomly lights up one of four LEDs.
- Records and displays reaction time in milliseconds.
- Keeps track of the player's score and displays it on an LCD.
- Easy to reset and replay the game.

## Hardware Requirements
Here are the components you will need:

| Component        | Quantity | Image                                      |
|------------------|----------|--------------------------------------------|
| Arduino Uno      | 1        | ![Arduino Uno](img/arduino-uno.png)        |
| LED (any color)  | 4        | ![LED](img/led.png)                        |
| 220Ω Resistor    | 4        | ![Resistor](img/resistor.png)              |
| Joystick Module  | 1        | ![Joystick](img/joystick.png)              |
| Button           | 1        | ![Button](img/button.png)                  |
| I2C LCD (1602)   | 1        | ![LCD](img/lcd.png)                        |
| Breadboard       | 1        | ![Breadboard](img/breadboard.png)          |
| Jumper Wires     | As needed| ![Jumper Wires](img/jumper-wires.png)      |

## Circuit Diagram

Below is a brief description of how to wire the components:
- **LEDs**: Connect each LED to a digital pin on the Arduino (pins 3, 4, 5, 6). Use a 220Ω resistor in series with each LED.
- **Joystick Module**: Connect X and Y pins to analog pins A0 and A1. GND goes to GND, and VCC goes to 5V.
- **Button**: The button pin is connected to pin 2.
- **LCD I2C**: Connect SDA to A4, SCL to A5. VCC to 5V, and GND to GND.

### Wiring Diagram

![Circuit Diagram](img/circuit-diagram.PNG)

## Installation

### 1. Install the required libraries
- Open Arduino IDE and go to **Sketch > Include Library > Manage Libraries...**
- Search for and install the `LiquidCrystal_I2C` library for the I2C LCD display.

### 2. Upload the code
- Open the provided `.ino` file in the Arduino IDE.
- Under **Tools > Board**, select **Arduino Uno**.
- Under **Tools > Port**, select the correct port for your Arduino.
- Click the upload button to upload the code to your Arduino.

## Usage

### Start the game:
- Once powered on, press the start button to begin the game.
- The LCD will display "Get Ready!" and then randomly light up one of the LEDs.

### Move the joystick:
- Quickly move the joystick in the corresponding direction (up, down, left, or right) as soon as the LED lights up.

### Score and reaction time:
- The LCD will display your reaction time in milliseconds and update your score after each round.

### Game over:
- If you make an incorrect joystick movement, the game will display "Game Over" along with your final score.

### Restart the game:
- After the game ends, press the button to restart.