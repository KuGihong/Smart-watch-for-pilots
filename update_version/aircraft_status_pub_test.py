import rospy
from std_msgs.msg import String

def publish_mode():
    rospy.init_node("test_publisher") 
    pub = rospy.Publisher("aircraft_status", String, queue_size=10)

    while not rospy.is_shutdown():
        message = "lg_warning_on"
        pub.publish(message)  
        rospy.loginfo("Published message: %s", message)

if __name__ == "__main__":
    try:
        publish_mode()
    except rospy.ROSInterruptException:
        pass
