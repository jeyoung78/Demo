import time
from adafruit_pca9685 import PCA9685
from adafruit_motor import servo
import board
import busio

# Set up I2C communication and PCA9685
i2c = busio.I2C(board.SCL, board.SDA)
pca = PCA9685(i2c)
pca.frequency = 50

# Set up standard servo on channel 0
my_servo = servo.Servo(pca.channels[0])
my_servo.set_pulse_width_range(min_pulse=500, max_pulse=2500)

try:
    # Simulate 360-degree rotation by moving back and forth
    while True:
        my_servo.angle = 0
        time.sleep(0.4)
        my_servo.angle = 180
        time.sleep(0.4)

except KeyboardInterrupt:
    # Turn off the servo when done
    my_servo.angle = None
    pca.deinit()
