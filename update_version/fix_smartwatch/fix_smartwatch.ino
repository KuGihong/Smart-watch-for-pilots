#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#define I2C_ADDRESS 4
#define SIGNAL_VALUE  50000

MAX30105 particleSensor;

const unsigned long interval = 2000; // 2초 (단위: 밀리초)
unsigned long previousMillis = 0;
int delayTime = 1000;

const byte RATE_SIZE = 3;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float bpm;
float beat_avg;
String beat_status;

void setup()
{
  Serial1.begin(9600);
  Wire.begin();    

  // 심박수 센서 체크
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
  // XBee 시리얼 데이터 확인
  String receivedData = Serial1.readString();
  if (receivedData == "lg_warning_on"){
    Wire.beginTransmission(4);
    Wire.write("LG");
    Wire.endTransmission();
    delay(1000);
  }
      
  // 평균 심박수 측정
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    bpm = 60 / (delta / 1000.0);

    if (bpm < 255 && bpm > 20)
    {
      rates[rateSpot++] = (byte)bpm;
      rateSpot %= RATE_SIZE;

      beat_avg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beat_avg += rates[x];
      beat_avg /= RATE_SIZE;
    }
  }
  // 심박수 분류
  if (beat_avg >= 40 && beat_avg <= 60) {
    beat_status = "WARNING";              // std_msgs::String 메시지에 문자열 데이터 할당
    Wire.beginTransmission(I2C_ADDRESS);  // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("WARNING");                // "WARNING" 문자열 전송
    Wire.endTransmission();               // 전송 종료      

  } else if (beat_avg >= 60 && beat_avg <= 100) {
    beat_status = "NORMAL";
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write("\n");
    Wire.endTransmission();
    
  } else if(beat_avg > 100){
    beat_status = "DANGER";
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write("DANGER");
    Wire.endTransmission();   
  }

  if (irValue < SIGNAL_VALUE){
    beat_avg = 0;
    beat_status = "No Signal";
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write("\n");
    Wire.endTransmission();
  } 
//   char sendData[30];
   // 일정 시간(interval)마다 데이터 전송
   unsigned long currentMillis = millis();  
   if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // 이전 시간 업데이트
//    sprintf(sendData, "%.2f,%s", beat_avg, beat_status.c_str());
//    Serial1.write(sendData);
  }  
}
