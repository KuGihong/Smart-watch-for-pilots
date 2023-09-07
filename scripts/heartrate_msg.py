import rospy
from heart_rate_msg.msg import heart_rate  # Import the custom message

import serial

def publish_heart_rate(beat_avg, beat_status):
    # Initialize the ROS node with the name 'heart_rate_publisher'
    rospy.init_node('heart_rate_publisher', anonymous=True)

    # Create a ROS publisher for the 'heart_rate' topic with the custom message type
    pub = rospy.Publisher('heart_rate', heart_rate, queue_size=10)

    # Create an instance of the custom message
    msg = heart_rate()

    # Populate the message fields with the data
    msg.header.stamp = rospy.Time.now()  # Current time
    msg.beat_avg = beat_avg
    msg.beat_status = beat_status

    # Publish the message
    pub.publish(msg)

    # Print the data being published
    if msg.beat_status == "WARNING":
        rospy.logwarn("Beat Average: %.2f, Beat Status: %s", msg.beat_avg, msg.beat_status)
    elif msg.beat_status == "DANGER":
        rospy.logerr("Beat Average: %.2f, Beat Status: %s", msg.beat_avg, msg.beat_status)
    else:
        rospy.loginfo("Beat Average: %.2f, Beat Status: %s", msg.beat_avg, msg.beat_status)

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM1', 9600)  # Serial port settings (change '/dev/ttyACM1' to your actual port)
    while not rospy.is_shutdown():
        line = ser.readline().decode().strip()  # Read and decode serial data
        if line:
            beat_avg, beat_status = line.split(",")  # Split CSV data
            beat_avg = float(beat_avg)  # Convert string to float

            # Publish the data as a ROS message
            publish_heart_rate(beat_avg, beat_status)
