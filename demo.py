import time
from adafruit_pca9685 import PCA9685
from adafruit_motor import servo
import board
import busio

# Set up I2C communication and PCA9685
i2c = busio.I2C(board.SCL, board.SDA)
pca = PCA9685(i2c)
pca.frequency = 50

# Set up continuous rotation servo on channel 0
continuous_servo = servo.ContinuousServo(pca.channels[0])

try:
    while True:
        # Set the servo to rotate forward at half speed
        continuous_servo.throttle = 0.5
        time.sleep(2)

        # Stop the servo
        continuous_servo.throttle = 0
        time.sleep(1)

        # Set the servo to rotate backward at half speed
        continuous_servo.throttle = -0.5
        time.sleep(2)

except KeyboardInterrupt:
    # If interrupted, stop the servo
    continuous_servo.throttle = 0
    pca.deinit()
