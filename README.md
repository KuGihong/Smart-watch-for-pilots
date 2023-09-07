# Smart-Watch-For-Pilots
Pilot heart rate measurement, Morse code vibration integration module work

Use Ubuntu 18.04 ROS1 Melodic & 20.04 ROS1 Noetic

![순서도](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/62667120-5326-484b-bec4-5bb889dbd751)


====== If you want to use rosserial with Arduino Due board ======

Modify ArduinoHardware.h code in ros_lib
  1. Line 75 iostream = &Serial1; 
  -> iostream = &Serial
  3. Line 73 #if defined(USBCON) and !(defined(USE_USBCON))
  -> #if defined(USBCON) and !(defined(USE_USBCON)) and !(defined(_SAM3XA_))
  3. Add #define USE_USBCON before <ros.h> of Arduino code
  4. Change baud 115200 -> 57600
  4. Run rosserial after connecting with Programming Port

![ArduinoHardware](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/a3cb5c08-76b3-4f36-9d4d-6e5a4f0f1209)

===============================================

Use three Arduino Boards (I2C, zigbee)
  - Zigbee_transmitter, Master: Arduino Due(Input: Heart Rate)
  - Slave: Arduino Micro(Output: Vibration, Voice)
  - Zigbee_receiver: Arduino Uno(changeable)

The heart rate sensor location is as follows:

![심박센서 위치](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/0ad3e269-db79-44b9-a21c-d696e7a48c8b)

The measured heart rate is displayed as follows according to the provided Python file.
1. No signal and normal heart rate

![정상 심박](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/557fd8bc-8482-44e0-9c92-dc8792c21516)

2. Warning heart rate

![경고 심박](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/4c15c87e-1b37-4eb2-9cce-f0f6d1e45916)

3. Danger heart rate

![위험 심박](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/4d399d33-4c4e-4a5d-a530-f17c35a69960)
