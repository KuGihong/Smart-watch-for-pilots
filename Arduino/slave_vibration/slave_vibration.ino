#define UTIME 200
#include <Wire.h>

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
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
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
