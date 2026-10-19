<img width="894" height="715" alt="image" src="https://github.com/user-attachments/assets/3278dff1-327f-41b5-8eb0-ec1da65e4692" /># DeciBit Sound Monitor (Tinkercad Prototype)

An Arduino sound monitor prototype built in Tinkercad Circuits. It reads a sound level and two frequency inputs, classifies them, shows them on a 16x2 LCD, and logs the values over Serial.

This was part of **DeciBit**, a first-year engineering project at Western University (Jan-May 2026) about measuring noise pollution and its effects on local wildlife.

## Features

- **Sound level:** reads an analog input on A0 and maps it to 30-100 dB.
- **Noise categories:** Quiet (under 50 dB), Moderate (under 75 dB), Loud (75 dB and over).
- **Frequency:** reads two analog inputs (A1, A2), maps each to 100-2000 Hz, and adds them (capped at 9999).
- **Frequency bands:** Low (under 500), Mid (under 2000), High (2000 and over).
- **LCD display:** 3 screens, cycled with a pushbutton: Time + dB, Time + frequency, and a compact summary.
- **Power switch and LED:** a slide switch turns the system on and off, and the LED shows it is running.
- **Serial logging:** prints `{time, dB, frequency}` once per second at 9600 baud.

## Hardware and pins

| Part | Pin |
| --- | --- |
| Sound level input | A0 |
| Frequency input 1 | A1 |
| Frequency input 2 | A2 |
| Display-mode button | D2 |
| Power slide switch | D6 |
| Status LED | D7 |
| LCD (RS, E, D4, D5, D6, D7) | 12, 11, 5, 4, 3, 10 |

## Circuit

<img width="894" height="715" alt="1775802205770" src="https://github.com/user-attachments/assets/16784c7b-31cb-4354-a7eb-ef070a4b46b5" />


## How to run

**In Tinkercad:**
1. Open your circuit in Tinkercad Circuits.
2. Open the **Code** editor, switch to **Text**, and paste in `decibit_sound_monitor.ino`.
3. Click **Start Simulation**.

**On a real Arduino:**
1. Open `decibit_sound_monitor.ino` in the Arduino IDE.
2. Wire the parts using the pin table above.
3. Upload, then open the Serial Monitor at 9600 baud.

The code uses the built-in `LiquidCrystal` library, so nothing extra needs installing.

## Notes

- The frequency values are simulated analog inputs, not a real FFT or microphone analysis.
- Sampling happens every 1 second (`sampleInterval`).

## Credits

Created by Deepesh Arun, Hariraam Thiruchelvam, Hisham Hashim, Abdullah Ashraf, Thomas Joyner, and Abraham Tekie.
