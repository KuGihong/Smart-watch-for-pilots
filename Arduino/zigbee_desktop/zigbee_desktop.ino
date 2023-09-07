#include <Arduino.h>

void setup()
{
    Serial.begin(9600);
}

void loop()
{
  if (Serial.available())
  {
    String receivedData = Serial.readString();
    Serial.println(receivedData);
  }
}
