#include <Arduino.h>
#include <Wire.h>
const unsigned long interval = 2000; // 2초 (단위: 밀리초)
unsigned long previousMillis = 0;
float beatAvg;
String msg;

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  beatAvg = 150.00;
  msg = "DANGER"; // std_msgs::String 메시지에 문자열 데이터 
  char sendData[30];
  unsigned long currentMillis = millis();  
  // 일정 시간(interval)마다 데이터 전송
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // 이전 시간 업데이트

  sprintf(sendData, "%.2f,%s", beatAvg, msg.c_str());
  Serial1.write(sendData);
  }
}
