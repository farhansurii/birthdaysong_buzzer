# ESP32 Birthday Wish Box 🎂

A small, battery-powered device that delivers a personalized audio-visual birthday surprise. When powered on, it plays the "Happy Birthday" song on a buzzer while displaying synchronized lyrics on an OLED screen. After the song, it shows a custom wish message. The entire sequence can be replayed anytime with the press of a button.

---

## Features ✨

* **Automated Greeting:** Plays a complete birthday sequence automatically on power-up.
* **Audio-Visual Experience:** Features the "Happy Birthday" song on a passive buzzer synchronized with lyrics on a 0.96" OLED display.
* **Personalized:** Easily customizable nickname and final wish message directly in the code.
* **Replayable:** A dedicated pushbutton allows the entire sequence to be played again.
* **Portable:** Designed to be battery-powered with a main ON/OFF switch for portability.

---

## Hardware Components ⚙️

You'll need the following components to build this project:

* **Microcontroller:** ESP32 Development Board (e.g., ESP32 DEVKIT V1)
* **Display:** 0.96" OLED Display (128x64, I2C Interface)
* **Sound:** 3-Pin Passive Buzzer Module (this project assumes the module's VCC requires 5V for optimal volume)
* **Input:**
    * Tactile Pushbutton (for replaying the sequence)
    * Slide Switch (or any ON/OFF switch for main power)
* **Power:** A battery source. Examples:
    * A 3.7V LiPo battery (note: this will supply ~3.7V to the buzzer module, not 5V).
    * A 5V USB Power Bank.
    * A pack of 4xAA batteries (~6V).
    * A 9V battery.
    *(The power source should connect to the ESP32's VIN pin, which can handle voltages above 3.3V).*
* **Misc:** Jumper Wires, Breadboard (for prototyping)

---

## Wiring & Connections 🔌

Connect the components to the ESP32 as follows.

### Main Power

* **Battery (+) Positive Terminal** → **One leg of the ON/OFF Switch**
* **Other leg of the ON/OFF Switch** → **VIN Pin** on the ESP32
* **Battery (-) Negative Terminal** → **GND Pin** on the ESP32

### OLED Display (I2C)

* `ESP32 GPIO22` → `SCL` pin on the OLED
* `ESP32 GPIO21` → `SDA` pin on the OLED
* `ESP32 3V3` → `VCC` pin on the OLED
* `ESP32 GND` → `GND` pin on the OLED

### 3-Pin Buzzer Module

* `ESP32 GPIO18` → `SIG` (or I/O, S) pin on the Buzzer Module
* `ESP32 5V (or VUSB)` → `VCC` pin on the Buzzer Module
    * *Note: This provides 5V for optimal buzzer volume. If your board doesn't have a 5V output pin, you can power the ESP32 and this module from an external 5V source.*
* `ESP32 GND` → `GND` pin on the Buzzer Module

### Pushbutton (Replay)

* `ESP32 GPIO23` → **One leg** of the Pushbutton
* `ESP32 GND` → **The other leg** of the Pushbutton

---

## Software & Setup 💻

1.  **IDE:** This project is coded using the [Arduino IDE](https://www.arduino.cc/en/software).
2.  **Board Manager:** Make sure you have the ESP32 board support package installed. Follow the instructions [here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
3.  **Libraries:** Install the following libraries through the Arduino IDE's Library Manager (`Sketch` > `Include Library` > `Manage Libraries...`):
    * `Adafruit GFX Library`
    * `Adafruit SSD1306`

---

## How to Use 🚀

1.  **Power On:** Flip the main switch to the ON position.
2.  **Enjoy:** The device will automatically display a "Happy B'Day!" message, followed by the song with synchronized lyrics. A final wish message is shown at the end.
3.  **Replay:** After the sequence is complete, press the pushbutton. The entire sequence will start over from the beginning.
4.  **Power Off:** Flip the main switch to the OFF position to save battery.

---

## Customization 🔧

You can easily personalize the greeting by modifying these lines at the top of the `.ino` sketch file:

* To change the name:
    ```cpp
    #define NICKNAME "Sahabat"
    ```
* To change the final wish message:
    ```cpp
    #define WISH_TEXT "Semoga panjang umur, sehat selalu, dan semua keinginanmu tercapai! Amin."
    ```
* To change the GPIO pins used, modify the pin definition constants:
    ```cpp
    const int speakerPin = 18;
    const int buttonPin = 23;
    ```

---

## License

This project is open-source and licensed under the MIT License. See the `LICENSE` file for more details.
