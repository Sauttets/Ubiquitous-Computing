#include <Arduino_LSM6DSOX.h>
#include <WiFiNINA.h>

void setup() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  if (IMU.temperatureAvailable()) {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);

    Serial.print("Temperature = ");
    Serial.print(temperature_deg);
    Serial.print("°C");
    Serial.println();
    
    digitalWrite(LEDB, temperature_deg < 25 ? HIGH : LOW);
    digitalWrite(LEDG, (temperature_deg > 20 && temperature_deg < 36) ? HIGH : LOW);
    digitalWrite(LEDR, temperature_deg > 32 ? HIGH : LOW);

  }
}