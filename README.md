
# M5Cardputer Audio Recorder

This project demonstrates how to use the **M5Cardputer** and the **SPM1423 MEMS Microphone** to record audio and save it as a `.wav` file on an SD card. The program detects key presses to start and stop recording and dynamically generates filenames to avoid overwriting existing files. Additionally, it applies software amplification to the audio signal.

## Features

- **Record Audio**: Press any key on the M5Cardputer's keyboard to start or stop recording.
- **Dynamic Filenames**: Automatically generates sequential filenames (`recording.wav`, `recording1.wav`, `recording2.wav`, etc.) to avoid overwriting files.
- **Audio Amplification**: Amplifies the recorded audio to boost the volume. This is adjustable via an amplification factor.
- **Visual Feedback**: Displays messages on the M5Cardputer's screen to show the current recording status.

## Hardware Requirements

- **M5Cardputer**
- **SPM1423 MEMS Microphone** (connected via I2S)
- **SD Card**

## Software Requirements

- **M5Cardputer Library**: Used for controlling the device, keyboard, and screen.
- **Arduino IDE**: To upload the code to your M5Cardputer.

## Installation

1. **Clone the repository**:
    ```bash
    git clone https://github.com/yourusername/m5cardputer-audio-recorder.git
    ```

2. **Set up Arduino IDE**:
   - Install the required libraries (`M5Cardputer`, `driver/i2s`, and `SD`).
   - Set your board to **M5Stack** in the Arduino IDE.

3. **Upload the code**:
   - Open the `m5cardputer-audio-recorder.ino` file in Arduino IDE.
   - Connect your M5Cardputer and upload the code.

## Usage

1. **Insert an SD card** into the M5Cardputer.
2. **Power on the M5Cardputer** and wait for the display message.
3. **Press any key** on the M5Cardputer keyboard to start recording.
4. **Press any key** again to stop recording.
5. The recordings are saved in `.wav` format on the SD card with sequential filenames.

### Audio Amplification

By default, an amplification factor of 3x is applied to boost the volume of the recorded audio. This can be modified in the code:

```cpp
const int amplificationFactor = 3;  // Modify this value to adjust amplification
```

Be cautious with higher values as they may cause distortion.


## Contributions

Feel free to submit pull requests or open issues to improve this project. Contributions are welcome!
