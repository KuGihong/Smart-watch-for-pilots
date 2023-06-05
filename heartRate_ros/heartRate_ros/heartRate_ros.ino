#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#define USE_USBCON
#include <ros.h>
#include <std_msgs/Float64.h>

MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

ros::NodeHandle nh;
std_msgs::Float64 beatAvgMsg;
ros::Publisher arduinoBeatAvgPub("arduino_beatAvg", &beatAvgMsg);

void setup()
{
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  
  Serial.begin(57600);
  nh.initNode();
  nh.advertise(arduinoBeatAvgPub);
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

      beatAvgMsg.data = beatAvg;
      arduinoBeatAvgPub.publish(&beatAvgMsg);
    }
  }

  nh.spinOnce();
}
