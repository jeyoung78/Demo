import time
import board
import busio
from adafruit_pca9685 import PCA9685
from adafruit_motor import servo

# Initialize I2C bus
i2c = busio.I2C(board.SCL, board.SDA)

# Initialize the PCA9685 using the I2C bus
pca = PCA9685(i2c)
pca.frequency = 50  # Set to 50 Hz for servo motors

# Create a servo object on channel 0 (leftmost pin)
my_servo = servo.Servo(pca.channels[0])

# Control the servo on the leftmost pin
try:
    print("Moving servo to 0 degrees")
    my_servo.angle = 0  # Move to 0 degrees
    time.sleep(1)

    print("Moving servo to 90 degrees")
    my_servo.angle = 90  # Move to 90 degrees (center position)
    time.sleep(1)

    print("Moving servo to 180 degrees")
    my_servo.angle = 180  # Move to 180 degrees
    time.sleep(1)

finally:
    # Safely turn off the PCA9685
    pca.deinit()
