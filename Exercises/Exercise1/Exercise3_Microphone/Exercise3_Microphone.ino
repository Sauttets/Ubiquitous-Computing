#include <WiFiNINA.h>
#include <PDM.h>

bool LED_SWITCH = false;
static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

void setup() {
  Serial.begin(9600);
  pinMode(LEDB, OUTPUT);
  while (!Serial);
  // Configure the data receive callback
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      Serial.println(sampleBuffer[i]);

      if (sampleBuffer[i] > 10000 || sampleBuffer[i] <= -10000) {
        LED_SWITCH = !LED_SWITCH;
        if (LED_SWITCH) {
          Serial.println();
          digitalWrite(LEDB, HIGH);
          Serial.println("ON!");
          Serial.println();
          delay(1000);
        }
        else {
          Serial.println();
          digitalWrite(LEDB, LOW);
          Serial.println("OFF!");
          Serial.println();
          delay(1000);
        }
      }
    }

    // Clear the read count
    samplesRead = 0;
  }
}

void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();
  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);
  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}