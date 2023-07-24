#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>

MAX30105 particleSensor;

const byte RATE_SIZE = 3;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
float beatAvg;

ros::NodeHandle nh;
std_msgs::Float32 irValueMsg;
std_msgs::Float32 beatAvgMsg;
ros::Publisher irValuePub("ir_value", &irValueMsg);
ros::Publisher beatAvgPub("beat_avg", &beatAvgMsg);

std_msgs::String statusMsg;
ros::Publisher statusPub("beat_status", &statusMsg);

void setup()
{
  Serial.begin(57600);
  Serial.println("Initializing...");
  Wire.begin();

  nh.initNode();
  nh.advertise(irValuePub);
  nh.advertise(beatAvgPub);
  nh.advertise(statusPub);

  // Initialize sensor
  if (!particleSensor.begin(Wire1, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);
}

void loop()
{
  long irValue = particleSensor.getIR();
  std_msgs::String str_msg; // std_msgs::String 메시지 선언

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
      
      beatAvgMsg.data = beatAvg;
      beatAvgPub.publish(&beatAvgMsg);
    }
  }

  if (beatAvg >= 40 && beatAvg <= 60) {
    str_msg.data = "WARNING"; // std_msgs::String 메시지에 문자열 데이터 할당
    statusPub.publish(&str_msg);
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("WARNING"); // "WARNING" 문자열 전송
    Wire.endTransmission(); // 전송 종료      

  } else if (beatAvg >= 60 && beatAvg <= 100) {
    str_msg.data = "NORMAL"; // std_msgs::String 메시지에 문자열 데이터 할당
    statusPub.publish(&str_msg);
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("\n"); // 개행 문자 전송
    Wire.endTransmission(); // 전송 종료
    
  } else {
    str_msg.data = "DANGER"; // std_msgs::String 메시지에 문자열 데이터 할당
    statusPub.publish(&str_msg);
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("DANGER"); // "DANGER" 문자열 전송
    Wire.endTransmission(); // 전송 종료    
  }

  if (irValue < 50000){
    str_msg.data = "NoFinger"; // std_msgs::String 메시지에 문자열 데이터 할당
    statusPub.publish(&str_msg);
    Wire.beginTransmission(4); // 통신을 위한 슬레이브 주소 4로 전송 시작
    Wire.write("\n"); // 개행 문자 전송
    Wire.endTransmission(); // 전송 종료
  }

  irValueMsg.data = (float)irValue; // int에서 float로 변경
  irValuePub.publish(&irValueMsg);

  nh.spinOnce(); // 노드 핸들이 메시지를 처리하고 토픽 발행을 수행합니다.
}
