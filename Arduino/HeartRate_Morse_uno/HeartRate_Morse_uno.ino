#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>

#define UTIME 200

MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
int MorsePin = 3;

ros::NodeHandle nh;
std_msgs::Float32 beatAvgMsg;
ros::Publisher arduinoBeatAvgPub("arduino_beatAvg", &beatAvgMsg, sizeof(beatAvgMsg));
void receivedWordCallback(const std_msgs::String& msg) {
  String word = msg.data;
  Morse(word);
}
ros::Subscriber<std_msgs::String> wordSub("arduino_word", receivedWordCallback);

String morsecode[] = {
//  "-----", ".----", ".---", "...--", "....-", // 0 ~ 4
//  ".....", "-....", "--...", "..", "----.",  // 5 ~ 9
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",  // A ~ G
  "....", "..", ".---", "-.-", ".-..", "--", "-.",  // H ~ N
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",  // O ~ U  
//  "...-", ".--", "-..-", "-.--", "--..",  // V ~ Z
};

void Morse(String input) {  
  for (int i = 0; i < input.length(); i++) {
    if (input[i] == ' ') {
      delay(UTIME * 7);
      continue;
    }

    String code;
    int vibration = 0;

    if (input[i] >= '0' && input[i] <= '9') {
      code = morsecode[input[i] - '0'];
    } else if (input[i] >= 'A' && input[i] <= 'Z') {
      code = morsecode[input[i] - 'A' + 10];
    } else {
      // Handle unsupported characters or special symbols
      continue;
    }

    for (int j = 0; j < code.length(); j++) {
      if (code[j] == '.') {
        vibration = UTIME;
      } else if (code[j] == '-') {
        vibration = UTIME * 3;
      }
      digitalWrite(MorsePin, HIGH); // Start vibration
      delay(vibration);
      digitalWrite(MorsePin, LOW);
      delay(UTIME);
    }
  }
}
void setup()
{
  nh.initNode();
  nh.advertise(arduinoBeatAvgPub);
  nh.subscribe(wordSub);
  Serial.begin(115200);
  pinMode(MorsePin, OUTPUT); // Morse

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("no finger");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
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

      beatAvgMsg.data = beatAvg;
      arduinoBeatAvgPub.publish(&beatAvgMsg);
    }
  }

  nh.spinOnce();
}
