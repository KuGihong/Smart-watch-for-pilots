#!/usr/bin/python3
import rospy
from std_msgs.msg import Float32, String

def ir_value_callback(data):
    rospy.loginfo("irValue: %.1f, beatAvg: %.1f, status: %s", data.data, beat_avg, beat_status)

def beat_avg_callback(data):
    global beat_avg
    beat_avg = data.data

def status_callback(data):
    global beat_status
    beat_status = data.data

def listener():
    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber("ir_value", Float32, ir_value_callback)
    rospy.Subscriber("beat_avg", Float32, beat_avg_callback)
    rospy.Subscriber("beat_status", String, status_callback)

    rospy.spin()

if __name__ == '__main__':
    beat_avg = 0.0
    beat_status = ""
    listener()
