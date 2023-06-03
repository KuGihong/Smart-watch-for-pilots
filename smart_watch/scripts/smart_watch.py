#!/usr/bin/python3

import rospy
from std_msgs.msg import Float64, String

# Define the threshold heart rate values
MIN_HEART_RATE = 60
MAX_HEART_RATE = 100

class HeartRateMonitor:
    def __init__(self):
        rospy.init_node('heart_rate_monitor', anonymous=True)
        rospy.Subscriber('arduino_beatAvg', Float64, self.beatAvgCallback)
        rospy.Subscriber('arduino_irValue', Float64, self.irValueCallback)
        rospy.Subscriber('arduino_sensorStatus', String, self.sensorStatusCallback)
        self.arduino_pub = rospy.Publisher('arduino_command', String, queue_size=10)
        rospy.loginfo("Waiting for sensor initialization...")

    def beatAvgCallback(self, data):
        beat_avg = data.data
        rospy.loginfo("Received beatAvg: %.2f", beat_avg)

        if beat_avg < MIN_HEART_RATE or beat_avg > MAX_HEART_RATE:
            # Heart rate is outside the safe range, send "DANGER" to Arduino
            rospy.loginfo("Sending DANGER to Arduino!")
            self.send_command("DANGER")

    def irValueCallback(self, data):
        ir_value = data.data
        if ir_value < 50000:
            rospy.loginfo("No finger?")

    def sensorStatusCallback(self, data):
        status = data.data
        rospy.loginfo("Sensor status: %s", status)

    def send_command(self, command):
        self.arduino_pub.publish(command)

    def run(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        monitor = HeartRateMonitor()
        monitor.run()
    except rospy.ROSInterruptException:
        pass


