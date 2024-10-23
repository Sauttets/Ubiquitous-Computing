#include <WiFiNINA.h>

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  digitalWrite(LEDB, LOW);              
  digitalWrite(LEDR, HIGH);
  delay(500);
  digitalWrite(LEDR, LOW);              
  digitalWrite(LEDG, HIGH);              
  delay(500);
  digitalWrite(LEDG, LOW);              
  digitalWrite(LEDB, HIGH);
  delay(500);  
}