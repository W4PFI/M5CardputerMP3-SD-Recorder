#include <M5Cardputer.h>  // Use M5Cardputer library instead of M5Stack
#include <driver/i2s.h>
#include <SD.h>

// I2S configuration for the SPM1423 MEMS microphone
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = 64000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 1024,
};

// I2S pin configuration (adjust for M5Cardputer)
i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,  // Not used for PDM
    .ws_io_num = 43,                  // Clock pin
    .data_out_num = I2S_PIN_NO_CHANGE, // No output needed
    .data_in_num = 46,                // Data pin
};

// WAV file header structure
struct WAVHeader {
    char riff_header[4];    // "RIFF"
    uint32_t wav_size;      // Size of the wav portion of the file, excluding the first 8 bytes
    char wave_header[4];    // "WAVE"
    char fmt_header[4];     // "fmt "
    uint32_t fmt_chunk_size;// Size of the fmt chunk (16 for PCM)
    uint16_t audio_format;  // Audio format (1 for PCM)
    uint16_t num_channels;  // Number of channels (1 for mono)
    uint32_t sample_rate;   // Sampling rate (e.g., 64,000)
    uint32_t byte_rate;     // Number of bytes per second (sample_rate * num_channels * bits_per_sample / 8)
    uint16_t sample_alignment; // num_channels * bits_per_sample / 8
    uint16_t bit_depth;     // Bits per sample
    char data_header[4];    // "data"
    uint32_t data_bytes;    // Number of bytes in the data portion of the file
};

void displayMsg(String message) {
    int x = 0;  // Starting X position
    int y = 0;  // Starting Y position
    int maxWidth = 240;  // Screen width in pixels
    int maxHeight = 135;  // Screen height in pixels
    int lineHeight = 20;  // Line height based on text size
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);  // Clear the screen

    M5Cardputer.Lcd.setTextColor(TFT_WHITE);
    M5Cardputer.Lcd.setTextSize(2);  // Adjust text size as needed
    
    String currentLine = "";
    
    for (int i = 0; i < message.length(); i++) {
        currentLine += message[i];
        
        // Check if current line exceeds the screen width or it's a newline character
        int lineWidth = M5Cardputer.Lcd.textWidth(currentLine);
        if (lineWidth > maxWidth || message[i] == '\n') {
            M5Cardputer.Lcd.setCursor(x, y);
            M5Cardputer.Lcd.println(currentLine);  // Print the line
            currentLine = "";  // Reset for the next line
            y += lineHeight;  // Move cursor to the next line

            // If we've reached the bottom of the screen, clear and reset
            if (y + lineHeight > maxHeight) {
                y = 0;  // Reset Y to the top of the screen
                M5Cardputer.Lcd.fillScreen(TFT_BLACK);  // Clear the screen
            }
        }
    }
    
    // Print any remaining text that didn’t trigger a wrap
    if (currentLine.length() > 0) {
        M5Cardputer.Lcd.setCursor(x, y);
        M5Cardputer.Lcd.println(currentLine);
    }
}

// Function to create the WAV file header
void createWAVHeader(WAVHeader *header, uint32_t sample_rate, uint16_t bit_depth, uint16_t num_channels, uint32_t data_size) {
    memcpy(header->riff_header, "RIFF", 4);
    header->wav_size = data_size + 36; // Data size + header size - 8
    memcpy(header->wave_header, "WAVE", 4);
    memcpy(header->fmt_header, "fmt ", 4);
    header->fmt_chunk_size = 16;
    header->audio_format = 1;         // PCM format
    header->num_channels = num_channels;
    header->sample_rate = sample_rate;
    header->byte_rate = sample_rate * num_channels * bit_depth / 8;
    header->sample_alignment = num_channels * bit_depth / 8;
    header->bit_depth = bit_depth;
    memcpy(header->data_header, "data", 4);
    header->data_bytes = data_size;
}

void setup() {
    M5Cardputer.begin();  // Initialize M5Cardputer (instead of M5.begin)
    Serial.begin(115200);

    // Initialize SD card
    if (!SD.begin()) {
        Serial.println("SD Card initialization failed!");
        return;
    }

    // Check free space (wonder why I added this!)
    if (SD.totalBytes() - SD.usedBytes() < 2048 ) {
        Serial.print("Not enough space on the SD Card!");
        return;
    }
   
    // I2S setup
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);

    // Open the WAV file on the SD card
    File audioFile = SD.open("/test01.wav", FILE_WRITE);
    if (!audioFile) {
        Serial.println("Failed to open file for writing!");
        return;
    }
    
    // Write WAV header placeholder
    WAVHeader header;
    uint32_t data_size = 0;  // We'll update this after recording
    createWAVHeader(&header, 64000, 16, 1, data_size);
    audioFile.write((uint8_t*)&header, sizeof(WAVHeader));

    // Start recording
    const int bufferSize = 1024;
    int16_t i2sBuffer[bufferSize];
    size_t bytes_read;

    String msg = "Recording started!";
    displayMsg(msg);
    Serial.println(msg);

    for (int i = 0; i < 200; i++) {  // Record in chunks (adjust for longer recordings)
      i2s_read(I2S_NUM_0, (void*)i2sBuffer, bufferSize * sizeof(int16_t), &bytes_read, portMAX_DELAY);

      if (bytes_read > 0) {
        Serial.print("Bytes read: ");
        Serial.println(bytes_read);
        audioFile.write((uint8_t*)i2sBuffer, bytes_read);
        data_size += bytes_read;
      } else {
        Serial.println("No data read from I2S.");
      }
    }
    
    msg = "Recording finished!";
    displayMsg(msg);
    Serial.println(msg);

    // Update the WAV header with the correct data size
    audioFile.seek(0);
    createWAVHeader(&header, 64000, 16, 1, data_size);
    audioFile.write((uint8_t*)&header, sizeof(WAVHeader));
    audioFile.close();

    Serial.println("Saved to SD card.");
}

void loop() {
    // Do nothing in the loop
}
