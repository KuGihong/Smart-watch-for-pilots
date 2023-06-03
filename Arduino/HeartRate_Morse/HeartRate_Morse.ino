#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>

#define UTIME 200

MAX30105 particleSensor;

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred
float beatsPerMinute;
float beatAvg;
int MorsePin = 3;

ros::NodeHandle nh;
std_msgs::Float64 beatAvgMsg;
std_msgs::Float64 irValueMsg;
std_msgs::String sensorStatusMsg;
ros::Publisher arduinoBeatAvgPub("arduino_beatAvg", &beatAvgMsg);
ros::Publisher arduinoIRValuePub("arduino_irValue", &irValueMsg);
ros::Publisher arduinoSensorStatusPub("arduino_sensorStatus", &sensorStatusMsg);

void receivedWordCallback(const std_msgs::String & receivedWord);
ros::Subscriber<std_msgs::String> wordSub("received_word", receivedWordCallback);


String morsecode[] = {
  "-----", ".----", ".---", "...--", "....-", // 0 ~ 4
  ".....", "-....", "--...", "..", "----.",  // 5 ~ 9
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",  // A ~ G
  "....", "..", ".---", "-.-", ".-..", "--", "-.",  // H ~ N
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",  // O ~ U
  "...-", ".--", "-..-", "-.--", "--..",  // V ~ Z
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
    Serial.println(code);
  }
}

void receivedWordCallback(const std_msgs::String& receivedWord) {
  String word = receivedWord.data;
  Morse(word);
}

void setup() {
  // Initialize the ROS node
  nh.initNode();
  nh.advertise(arduinoBeatAvgPub);
  nh.advertise(arduinoIRValuePub);
  nh.advertise(arduinoSensorStatusPub);
  nh.subscribe(wordSub);

  // Other setup code
  pinMode(MorsePin, OUTPUT); // Morse

  Serial.begin(115200);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use default I2C port, 400kHz speed
    sensorStatusMsg.data = "Sensor initialization failed";
    arduinoSensorStatusPub.publish(&sensorStatusMsg);
    while (1);
  } else {
    sensorStatusMsg.data = "Sensor initialization successful";
    arduinoSensorStatusPub.publish(&sensorStatusMsg);
  }
  sensorStatusMsg.data = "Place your index finger on the sensor with steady pressure.";
  arduinoSensorStatusPub.publish(&sensorStatusMsg);

  particleSensor.setup(); // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
}

void loop() {
  // Handle ROS communication
  nh.spinOnce();

  long irValue = particleSensor.getIR();

  // Publish irValue
  irValueMsg.data = irValue;
  arduinoIRValuePub.publish(&irValueMsg);

  if (checkForBeat(irValue) == true) {
    // We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
      rateSpot %= RATE_SIZE; // Wrap variable

      // Take average of readings
      beatAvg = 0.0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;

      // Publish beatAvg value
      beatAvgMsg.data = beatAvg;
      arduinoBeatAvgPub.publish(&beatAvgMsg);
    }
  }
}
