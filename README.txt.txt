# DarkRay RGB Controller

## Project Overview
This project is an Arduino-based RGB LED strip controller with NeoPixel LEDs and an OLED display.  
The interface includes buttons and a rotary encoder for easy navigation through menus to select colors, effects, and settings such as speed and brightness.

---

## Required Libraries
Make sure you have installed the following Arduino libraries (via Library Manager):

- Adafruit_GFX
- Adafruit_SSD1306
- Adafruit_NeoPixel
- Wire

---

## Hardware Connections

| Component           | Arduino Pin | Description                      |
|---------------------|-------------|--------------------------------- |
| Button UP           | D2          | Navigate Up in the menu          |
| Button DOWN         | D3          | Navigate Down in the menu        |
| Button SELECT       | D4          | Select / Enter menu option       |
| Button BACK         | D5          | Go back / exit submenu           |
| OLED Display (I2C)  | SDA → A4    | I2C Data Line                    |
|                     | SCL → A5    | I2C Clock Line                   |
| NeoPixel LED Strip  | D6          | Data input for LED strip         |
| Rotary Encoder CLK  | D7          | Rotary encoder clock pin         |
| Rotary Encoder DT   | D8          | Rotary encoder data pin          |
| Rotary Encoder SW   | D9          | Rotary encoder push-button pin   |

> **Power Notes:**  
> - Use a separate 5V power supply for the NeoPixel strip (recommended 2A or higher).  
> - Connect all grounds (GND) together to ensure common reference.

---

## Features

- OLED menu interface to select:
  - Color
  - Effect (rainbow, blink, bounce, wave, fire, ice, and more)
  - Settings (speed and brightness)
  - About section with project info

- Control via push buttons or rotary encoder:
  - Rotate to navigate menu options
  - Press rotary encoder or button to select

---

## Usage Instructions

1. Upload the Arduino sketch to your board (Arduino Uno or Nano recommended).
2. Connect the hardware according to the table above.
3. Power the system and wait for the welcome screen on the OLED.
4. Use buttons or rotary encoder to navigate and customize LED effects.

---

## Developer Information

- Developer: AmirMahdi  
- Version: v1.0 Pro  
- Date: 2025/06/05  
- Made in Iran 🇮🇷

---

## Notes

- Rotary encoder and buttons can be used interchangeably for menu navigation.
- NeoPixel effects run continuously with adjustable speed and brightness.
- Debounce implemented for buttons and rotary encoder inputs.
- OLED display updates the current menu state dynamically.

---

Thank you for using DarkRay RGB Controller!
