#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <WiFiNINA.h>
#include <PDM.h>

Madgwick MgFilter;
unsigned long microsPerReading, microsPrevious;
static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

//LED varables
unsigned long blinkInterval = 500;
unsigned long previousMillisRed = 0;
unsigned long previousMillisBlue = 0;
unsigned long redBlinkEndTime = 0;
unsigned long blueBlinkEndTime = 0;
bool ledRedState = false;
bool ledBlueState = false;

void setup() {
  pinMode(LEDB, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  Serial.begin(9600);

  // Start the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  MgFilter.begin(25);

  // Initialize variables to pace updates to the correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;
  unsigned long currentMillis = millis();

  // Check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // Read accelerometer and gyroscope data from LSM6DSOX
    if (IMU.readAcceleration(ax, ay, az) && IMU.readGyroscope(gx, gy, gz)) {
      MgFilter.updateIMU(gx, gy, gz, ax, ay, az);
      pitch = MgFilter.getPitch();

      if(abs(pitch) < 80.0){
        Serial.print("Bad Posture. FIX NOW: ");
        Serial.println(pitch);
        blueBlinkEndTime = millis() + 10000;
      }
      // Increment previous time, so we keep proper pace
      microsPrevious = microsPrevious + microsPerReading;
    }
  }

  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      if (abs(sampleBuffer[i]) > 10000){
        redBlinkEndTime = millis() + 10000;
        Serial.print("Noise levels to high: " );
        Serial.println(abs(sampleBuffer[i]));
      }
    }
    samplesRead = 0;
  }
  // Blink Red LED
  if (currentMillis < redBlinkEndTime) {
    if (currentMillis - previousMillisRed >= blinkInterval) {
      previousMillisRed = currentMillis;
      ledRedState = !ledRedState;
      digitalWrite(LEDR, ledRedState ? HIGH : LOW);
    }
  } else {
    digitalWrite(LEDR, LOW);
  }

  // Blink Blue LED
  if (currentMillis < blueBlinkEndTime) {
    if (currentMillis - previousMillisBlue >= blinkInterval) {
      previousMillisBlue = currentMillis;
      ledBlueState = !ledBlueState;
      digitalWrite(LEDB, ledBlueState ? HIGH : LOW);
    }
  } else {
    digitalWrite(LEDB, LOW);
  }

  if(currentMillis > blueBlinkEndTime && currentMillis > redBlinkEndTime){
    digitalWrite(LEDG, HIGH);
  } else{
    digitalWrite(LEDG, LOW);
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
