#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <WiFiNINA.h>
#include <PDM.h>

Madgwick MwFilter;
unsigned long microsPerReading, microsPrevious;

// noise variables
static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

//LED varables
unsigned long blinkInterval = 500;
unsigned long previousMillisRed = 0;
unsigned long previousMillisBlue = 0;
unsigned long previousMillisGreen = 0;
unsigned long redBlinkEndTime = 0;
unsigned long blueBlinkEndTime = 0;
unsigned long greenBlinkEndTime = 0;
bool ledRedState = LOW;
bool ledBlueState = LOW;
bool ledGreenState = LOW;

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

  MwFilter.begin(25);

  // Initialize variables to pace updates to the correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
}

void loop() {
  int temperature_deg = 0;
  float ax, ay, az;
  float gx, gy, gz;
  float pitch;
  unsigned long currentMillis = millis();
  unsigned long microsNow;

  if (IMU.temperatureAvailable()) {
    IMU.readTemperature(temperature_deg);

    if (temperature_deg <= 25) {
      redBlinkEndTime = millis() + 10000;
    }
  }

  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // Read accelerometer and gyroscope data from LSM6DSOX
    if (IMU.readAcceleration(ax, ay, az) && IMU.readGyroscope(gx, gy, gz)) {
      MwFilter.updateIMU(gx, gy, gz, ax, ay, az);
      pitch = MwFilter.getPitch();

      if (pitch < 80.0) {
        blueBlinkEndTime = millis() + 10000;
      }
      microsPrevious = microsPrevious + microsPerReading;
    }
  }


  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      Serial.println(sampleBuffer[i]);
      if (sampleBuffer[i] > abs(10000)) {
          greenBlinkEndTime = millis() + 10000;
      }
    }

    // Clear the read count
    samplesRead = 0;
  }

  printStats(temperature_deg, pitch);

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


  // Blink Blue LED
  if (currentMillis < greenBlinkEndTime) {
    if (currentMillis - previousMillisGreen >= blinkInterval) {
      previousMillisGreen = currentMillis;
      ledGreenState = !ledGreenState;
      digitalWrite(LEDG, ledGreenState ? HIGH : LOW);
    }
  } else {
    digitalWrite(LEDG, LOW);
  }
}

void printStats(int temp, float pitch) {
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.print("°C");
  Serial.print("  Pitch = ");
  Serial.print(pitch);
  Serial.println();
}

void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();
  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);
  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
