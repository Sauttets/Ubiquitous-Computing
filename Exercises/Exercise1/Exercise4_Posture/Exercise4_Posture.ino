#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <WiFiNINA.h>

Madgwick filter;
unsigned long microsPerReading, microsPrevious;

void setup() {
  pinMode(LEDB, OUTPUT);
  Serial.begin(9600);

  // Start the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  filter.begin(25);
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;

  // Check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {
    if (IMU.readAcceleration(ax, ay, az) && IMU.readGyroscope(gx, gy, gz)) {
      filter.updateIMU(gx, gy, gz, ax, ay, az);
      pitch = filter.getPitch();

      // Print the orientation (heading, pitch, roll)
      Serial.print("Orientation: ");
      Serial.print(pitch);
      Serial.println();

    if(abs(pitch) < 80.0){
      digitalWrite(LEDB, HIGH);
    } else {
      digitalWrite(LEDB, LOW);
    }
      microsPrevious = microsPrevious + microsPerReading;
    }
  }
}
