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
    
    if(temperature_deg =< 25){
      digitalWrite(LEDB, HIGH);
    } else{
      digitalWrite(LEDB, LOW);
    }

    if(20 =< temperature_deg =< 36){
      digitalWrite(LEDG, HIGH);
    } else {
      digitalWrite(LEDG, LOW);
    }

    if(temperature_deg >= 32){
      digitalWrite(LEDR, HIGH); 
    } else {
      digitalWrite(LEDR, LOW);
    }
  }
}