import time
from adafruit_pca9685 import PCA9685
from adafruit_motor import servo
import board
import busio

# Set up I2C communication and PCA9685
i2c = busio.I2C(board.SCL, board.SDA)
pca = PCA9685(i2c)
pca.frequency = 50

# Set up servos on channels 0 and 1
servo1 = servo.Servo(pca.channels[15])

# Set custom pulse width range for both servos (optional)
servo1.set_pulse_width_range(min_pulse=500, max_pulse=2500)

try:
    # Simulate 360-degree rotation by moving both servos back and forth
    while True:
        # Move both servos to 0 degrees
        servo1.angle = 0
        time.sleep(1)

        # Move both servos to 30 degrees
        servo1.angle = 180
        time.sleep(1)

except KeyboardInterrupt:
    # Turn off the servos when done
    servo1.angle = None
    servo2.angle = None
    pca.deinit()
