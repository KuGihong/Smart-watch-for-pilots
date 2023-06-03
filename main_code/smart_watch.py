import rospy
from std_msgs.msg import Float32, String

class HeartRateMonitor:
    def __init__(self):
        rospy.init_node('heart_rate_monitor')
        self.sub = rospy.Subscriber('arduino_beatAvg', Float32, self.process_heart_rate)
        self.pub = rospy.Publisher('arduino_word', String, queue_size=1)

    def process_heart_rate(self, msg):
        beat_avg = msg.data
        rospy.loginfo("Received beatAvg: %.2f", beat_avg)

        if 60 <= beat_avg <= 100:
            word = ""
        else:
            word = "DANGER"

        self.pub.publish(word)

    def run(self):
        rospy.spin()

if __name__ == '__main__':
    monitor = HeartRateMonitor()
    monitor.run()
