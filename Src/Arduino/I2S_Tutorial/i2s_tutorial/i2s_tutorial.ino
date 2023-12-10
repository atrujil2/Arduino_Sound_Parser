#include <arduinoFFT.h>
#include <driver/i2s.h>

#define I2S_SCK 6
#define I2S_WS 5
#define I2S_SD 7

#define I2S_PORT I2S_NUM_0

#define bufferLen 64
int16_t sBuffer[bufferLen];

const char* noteNames[] = {"Ab", "A ", "Bb", "B ", "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G "};
const int octaves[] = {0,1,2,3,4,5,6,7,8};
const int scaleSteps[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
int tuningAFrequency = 440;



void i2s_install() {
  const i2s_config_t i2s_config {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0 , NULL);
}



void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}


void setup() {
  Serial.begin(115200);
  Serial.println(" ");

  delay(1000);

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  delay(500);
}

//time to read the i2s data and plot it to serial
void loop() {
  
    //plot a constant line so that the serial plotting window holds a constant range
    int rangelimit = 1500;
    Serial.print(rangelimit * -1);
    Serial.print(" ");
    Serial.print(rangelimit);
    Serial.print(" ");

    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

    if (result == ESP_OK)
    {
      int16_t samples_read = bytesIn / 8;
      if (samples_read > 0) {
        float mean = 0;
        for (int16_t i = 0; i < samples_read; i++) {
          mean += (sBuffer[i]);
        }

        mean /= samples_read;

        Serial.println(mean);
      }
    }


  }


  


