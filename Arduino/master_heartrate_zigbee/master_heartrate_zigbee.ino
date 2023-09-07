#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const unsigned long interval = 2000; // 2초 (단위: 밀리초)
unsigned long previousMillis = 0;

const byte RATE_SIZE = 3;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
float beatAvg;
String msg;

void setup()
{
  Serial1.begin(9600);
  Wire.begin();

  // Initialize sensor
  if (!particleSensor.begin(Wire1, I2C_SPEED_FAST))
  {
    Serial1.write("MAX30105 was not found.");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (beatAvg >= 40 && beatAvg <= 60) {
    msg = "WARNING"; // std_msgs::String 메시지에 문자열 데이터 할당
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("WARNING"); // "WARNING" 문자열 전송
    Wire.endTransmission(); // 전송 종료      

  } else if (beatAvg >= 60 && beatAvg <= 100) {
    msg = "NORMAL"; // std_msgs::String 메시지에 문자열 데이터 할당
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("\n"); // 개행 문자 전송
    Wire.endTransmission(); // 전송 종료
    
  } else {
    msg = "DANGER"; // std_msgs::String 메시지에 문자열 데이터 할당
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("DANGER"); // "DANGER" 문자열 전송
    Wire.endTransmission(); // 전송 종료    
  }

  if (irValue < 50000){
    msg = "No Signal"; // std_msgs::String 메시지에 문자열 데이터 할당
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("\n"); // 개행 문자 전송
    Wire.endTransmission(); // 전송 종료
  }
  char sendData[30];
  unsigned long currentMillis = millis();  
  // 일정 시간(interval)마다 데이터 전송
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // 이전 시간 업데이트

  sprintf(sendData, "%.2f,%s", beatAvg, msg.c_str());
  Serial1.write(sendData);
  }  
}
