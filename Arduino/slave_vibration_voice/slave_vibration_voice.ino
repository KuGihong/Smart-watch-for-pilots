#define UTIME 200
#include <Wire.h>
#include <Arduino.h>

#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"
#include "Vocab_US_TI99.h"
#include "Vocab_US_Clock.h"

Talkie voice;

String morsecode[] =
{
  "-----", ".----", ".---", "...--", "....-", //0 ~ 4
  ".....", "-....", "--...", "---..", "----.",  //5 ~ 9
  "", "", "", "", "", "", "", //: ; < = > ? @
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",  //A ~ G
  "....", "..", ".---", "-.-", ".-..", "--", "-.",  //H ~ N
  "---", ".--.", "--.-", ".-.", "...", "-", "..-",  //O ~ U
  "...-", ".--", "-..-", "-.--", "--.."  //V ~ Z
};

String receivedWord;

void setup() {
  pinMode(11, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  
  unsigned long startTime = millis();
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
  while (millis() - startTime < 4000);
#endif
  // Just to know which program is running on my Arduino
  voice.say(sp4_READY);
}

void loop() {  
  if (receivedWord == "\n"){
    receivedWord = "";
  }
  Morse(receivedWord); 
  delay(2000);
}

void receiveEvent(int numBytes) {
  receivedWord = "";
  
  while (Wire.available()) {
    char c = Wire.read();
    receivedWord += c;
  }
  
  if (receivedWord != "") {
    Serial.println(receivedWord);
  }
}

void Morse(String input)
{
  Voice(input);
  for(int i = 0; i < input.length(); i++)
  {
    if(input[i] == ' ')
    {
      delay(UTIME*7);   
      continue;
    }
    
    String code = morsecode[input[i]-'0'];
    int vibration = 0;
    
    for(int j = 0; j< code.length(); j++)
    {
      
      if(code[j] == '.')
      {
        vibration = UTIME;
      }
      else if(code[j] == '-')
      {
        vibration = UTIME * 3;
      }
        digitalWrite(11, HIGH); //start vibration
        delay(vibration);
        digitalWrite(11, LOW);
        delay(UTIME);
    }
    Serial.println(code);
    delay(400);
  }    
}

void Voice(String input)
{
  if(input == "DANGER"){
    voice.say(sp2_DANGER);
  }
  else if(input == "WARNING"){    
    voice.say(sp4_WARNING);
  }
}
