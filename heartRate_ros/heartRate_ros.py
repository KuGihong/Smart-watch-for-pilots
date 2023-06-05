import rospy
from std_msgs.msg import Float32

def beat_avg_callback(data):
    beat_avg = data.data
    rospy.loginfo("Received beatAvg: %.2f", beat_avg)

rospy.init_node('beat_avg_subscriber', anonymous=True)
rospy.Subscriber('arduino_beatAvg', Float32, beat_avg_callback)
rospy.spin()
