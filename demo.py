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
    # Start a full rotation
    continuous_servo.throttle = 1.0  # Full speed forward
    
except KeyboardInterrupt:
    # Stop the servo if interrupted
    continuous_servo.throttle = 0
    pca.deinit()
