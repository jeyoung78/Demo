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
servo1 = servo.Servo(pca.channels[0])
servo2 = servo.Servo(pca.channels[1])
servo3 = servo.Servo(pca.channels[2])
servo4 = servo.Servo(pca.channels[3])

# Set custom pulse width range for both servos (optional)
servo1.set_pulse_width_range(min_pulse=500, max_pulse=2500)
servo2.set_pulse_width_range(min_pulse=500, max_pulse=2500)
servo3.set_pulse_width_range(min_pulse=500, max_pulse=2500)
servo4.set_pulse_width_range(min_pulse=500, max_pulse=2500)

try:
    # Simulate 360-degree rotation by moving both servos back and forth
    while True:
        # Move both servos to 0 degrees
        servo1.angle = 0
        servo2.angle = 0
        servo3.angle = 0
        servo4.angle = 0
        time.sleep(0.2)

        # Move both servos to 30 degrees
        servo1.angle = 30
        servo2.angle = 30
        servo3.angle = 30
        servo4.angle = 30
        time.sleep(0.2)

except KeyboardInterrupt:
    # Turn off the servos when done
    servo1.angle = None
    servo2.angle = None
    pca.deinit()
