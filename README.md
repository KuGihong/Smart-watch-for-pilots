# Smart-watch-for-pilots-Project
Pilot heart rate measurement, Morse code vibration integration module work

Use Ubuntu 18.04 ROS1 Melodic & 20.04 ROS1 Noetic

====== First thing you need to do to use the Arduino Due board ======

Modify ArduinoHardware.h code in ros_lib
  1. Line 75 iostream = &Serial1;
  -> change to iostream = &Serial
  2. Line 73 #if defined(USBCON) and !(defined(USE_USBCON))
  -> #if defined(USBCON) and !(defined(USE_USBCON)) and !(defined(_SAM3XA_))
  3. Add #define USE_USBCON before <ros.h> of Arduino code
  4. Change baud 115200 -> 57600
  4. Run rosserial after connecting with Programming Port

![ArduinoHardware](https://github.com/KuGihong/Smart-watch-for-pilots-Project/assets/113013130/a3cb5c08-76b3-4f36-9d4d-6e5a4f0f1209)
