import rospy
from std_msgs.msg import String
import serial

message = ""

def callback(data):
    global message
    #rospy.loginfo("Received message: %s", data.data)
    message = data.data
    
def listener():
    rospy.Subscriber("aircraft_status", String, callback)
    
if __name__ == "__main__":
    rospy.init_node("aircraft_status")
    #ser = serial.Serial('/dev/ttyACM0', 9600)
    listener()
    rate = rospy.Rate(1/5)
    
    while not rospy.is_shutdown():
        if message == "lg_warning_on":
            #ser.write(message.encode('utf-8'))
            rospy.loginfo("Sent message to Smartwatch: %s", message)
        rate.sleep()
    
    #ser.close()