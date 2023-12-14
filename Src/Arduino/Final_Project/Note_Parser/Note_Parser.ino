#include <LiquidCrystal.h>
#include <arduinoFFT.h>
#include <driver/i2s.h>

//microphone pin definitions
#define I2S_SCK 6
#define I2S_WS 5
#define I2S_SD 7
#define I2S_PORT I2S_NUM_0

//lcd display pin definitions
#define LCD_D4 17
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8
#define LCD_RS 18
#define LCD_E 21

//fft initialization stuff
#define FFT_BUFFER_SIZE 2048 //number of samples that will be used in fft calculation
//i am making this a circular buffer for continuous audio sampling
double fftBuffer[FFT_BUFFER_SIZE];
double tempBuffer[FFT_BUFFER_SIZE]; //this is the bufer that gets the fft applied to it
int fftBufferHead = 0;


void addToFFTBuffer(double sample) {
  fftBuffer[fftBufferHead] = sample;
  fftBufferHead = (fftBufferHead + 1) % FFT_BUFFER_SIZE;
}
int fftBufferIndex = 0;
double sampleRate = 4096.0;

double vReal[FFT_BUFFER_SIZE];
double frequencyBins[FFT_BUFFER_SIZE / 2];
double magnitudes[FFT_BUFFER_SIZE / 2];
double vImag[FFT_BUFFER_SIZE];
arduinoFFT FFT = arduinoFFT();


unsigned long startTime;
unsigned long endTime;

void resetVReal() {
  for (int i = 0; i < FFT_BUFFER_SIZE; i++) {
    vImag[i] = 0;
    tempBuffer[i] = 0;
  }
}

//buffer for sound sampling
#define bufferLen 64
int16_t sBuffer[bufferLen];


//lcd setup
const int rs = 18, en = 21, d4 = 17, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//LiquidCrystal lcd(18, 21, 17, 10, 9, 8);



const char* noteNames[] = {"Ab", "A ", "Bb", "B ", "C ", "C#", "D ", "Eb", "E ", "F ", "F#", "G "};
int noteNamesLength = sizeof(noteNames) / sizeof(noteNames[0]);
int noteNameIndex = 4; //the note frequencies we are interested in starts on a C
const int octaves[] = {0,1,2,3,4,5,6,7,8};
const int scaleSteps[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
int tuningAFrequency = 440;

const float noteFrequencies[] = {
    16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87,
    32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74,
    65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47,
    130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
    261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
    523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,
    1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53,
    2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07,
    4186.01, 4434.92, 4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13
};
int noteFrequenciesLength = sizeof(noteFrequencies) / sizeof(noteFrequencies[0]);

//sets config and installs the i2s drivers
void i2s_install() {
  const i2s_config_t i2s_config {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 32768,
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


//sets the pins
void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

//functions for the final output

void normalizeMagnitudes(double ceiling) {
  double max = 0;
  for (int i = 0; i < (FFT_BUFFER_SIZE / 2); i ++) {
    if (magnitudes[i] > max) {
      max = magnitudes[i];
    } 
  }
  double scaleFactor = (ceiling / max);

  for (int i = 0; i < (FFT_BUFFER_SIZE / 2); i++) {
    magnitudes[i] = magnitudes[i] * scaleFactor;
  }
}


double findFrequencyPeak() {
  double maxMagnitude = 0;
  int peakIndex = 0;

  for (int i = 1; i < (FFT_BUFFER_SIZE / 2) - 1; i++) {
    if (magnitudes[i] > magnitudes[i + 1] && magnitudes[i] > magnitudes[i - 1]) {
      if (magnitudes[i] > maxMagnitude) {
        maxMagnitude = magnitudes[i];
        peakIndex = i;
      }
    }
  }
  return frequencyBins[peakIndex]; 
}
//variation of findFrequencyPeak. Instead of finding the overall highest, it finds the first peak above a threshold (the fundimental frequency)
double findFirstFrequencyPeakAboveThreshold(double threshold) {
  double maxMagnitude = 0;
  int peakIndex = 0;
  bool foundPeak = false;

  for (int i = 1; i < (FFT_BUFFER_SIZE / 2) - 1; i++) {
    if (magnitudes[i] > threshold && magnitudes[i] > magnitudes[i + 1] && magnitudes[i] > magnitudes[i - 1]) {
      if (magnitudes[i] > maxMagnitude) {
        maxMagnitude = magnitudes[i];
        peakIndex = i;
        foundPeak = true;
      }
    if (foundPeak) {
      break;
    }
  }
  
  }

  if (foundPeak == false) {
    return double(0);
  }
  else {
    return frequencyBins[peakIndex];
  }
  
}

String findClosestNote(double frequency) {

  double difference = 1000;
  const char* closestNoteName = "";
  const char* currentNoteName = noteNames[noteNameIndex];

  for (int i = 0; i < noteFrequenciesLength; i++) {
    
    float currentFrequency = noteFrequencies[i];

    if (abs(currentFrequency - frequency) <= difference) {
      difference = frequency - currentFrequency;
      closestNoteName = currentNoteName;
    }

    noteNameIndex = (noteNameIndex + 1) % noteNamesLength; //loop around the note names and repeat as long as needed
    currentNoteName = noteNames[noteNameIndex];


  }
  String output = String(closestNoteName) + " " + String(difference) + " " + String(frequency);

  return output;
  
}







//setup
void setup() {

  

  Serial.begin(115200);
  Serial.println(" ");

  lcd.begin(16, 2);
  lcd.print("Hello, World!");  

  //delay(1000);

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  //delay(500);
}

//time to read the i2s data and plot it to serial
void loop() {
    startTime = micros();
    //plot a constant line so that the serial plotting window holds a constant range
    int rangelimit = 1500;
    /*
    Serial.print(rangelimit * -1);
    Serial.print(" ");
    Serial.print(rangelimit);
    Serial.print(" ");
    */

    
    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

    if (result == ESP_OK)
    {
      int16_t samples_read = bytesIn / 8;
      if (samples_read > 0) {
        float mean = 0.0;
        for (int16_t i = 0; i < samples_read; i++) {
          mean += (sBuffer[i]);
        }

        mean /= samples_read;

        //Serial.println(mean);
        
        //fftBuffer[fftBufferIndex] = mean;
        addToFFTBuffer(mean);

        fftBufferIndex = (fftBufferIndex + 1) % FFT_BUFFER_SIZE;

        //condition for performing fft
        if (fftBufferHead % 256 == 0) { 
          
          

          for (int i = 0; i < FFT_BUFFER_SIZE; i ++) {
            int circularIndex = (fftBufferHead + i) % FFT_BUFFER_SIZE;
            tempBuffer[i] = fftBuffer[circularIndex];
          }
          

          endTime = micros();
          //Serial.println((endTime - startTime));
          //perform fft
          //use a hamming window for the fft to reduce spectral leakage
          FFT.Windowing(tempBuffer, FFT_BUFFER_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);

          FFT.Compute(tempBuffer, vImag, FFT_BUFFER_SIZE, FFT_FORWARD);

          FFT.ComplexToMagnitude(tempBuffer, vImag, FFT_BUFFER_SIZE);
          
          for (int i = 0; i < FFT_BUFFER_SIZE / 2; i++) {
            double frequency = ((double)i * sampleRate) / (double)FFT_BUFFER_SIZE;
            double magnitude = tempBuffer[i];
            
            frequencyBins[i] = frequency;
            magnitudes[i] = magnitude;

            

            
            //Serial.println(frequency);
            
            //Serial.print(0);
            //Serial.print(" ");
            //Serial.print(magnitude);
            //Serial.print(" ");
            //Serial.println(frequency);
            //Serial.print(" ");
            //Serial.println(2048.0);
          }
          normalizeMagnitudes(50000);
          double peakFrequency = findFirstFrequencyPeakAboveThreshold(10000);
          String note = findClosestNote(peakFrequency);
          Serial.print("Note: ");
          Serial.print(note);
          Serial.print("  ");
          Serial.print(magnitudes[220]);
          Serial.print(" ");
          Serial.println(frequencyBins[220]);          
          
          resetVReal();
          

        }
        
      }

    }

  }


  


