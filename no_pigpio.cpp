#include <iostream>
#include <pigpio.h>
#include <unistd.h>

const int PCA9685_ADDRESS = 0x40;  // I2C address of PCA9685
const int MODE1 = 0x00;
const int PRESCALE = 0xFE;
const int LED0_ON_L = 0x06;
const int FREQUENCY = 50;  // Standard servo frequency is 50Hz

int pca9685;

void setPWMFreq(int freq) {
    float prescaleval = 25000000.0 / 4096 / freq - 1;
    int prescale = static_cast<int>(prescaleval + 0.5);

    // Put PCA9685 into sleep mode to set prescaler
    gpioI2CWriteByteData(pca9685, MODE1, 0x10);  // Enter sleep mode
    gpioI2CWriteByteData(pca9685, PRESCALE, prescale);
    gpioI2CWriteByteData(pca9685, MODE1, 0x80);  // Restart and set to normal mode
    usleep(5000);
}

void setPWM(int channel, int on, int off) {
    gpioI2CWriteByteData(pca9685, LED0_ON_L + 4 * channel, on & 0xFF);
    gpioI2CWriteByteData(pca9685, LED0_ON_L + 4 * channel + 1, on >> 8);
    gpioI2CWriteByteData(pca9685, LED0_ON_L + 4 * channel + 2, off & 0xFF);
    gpioI2CWriteByteData(pca9685, LED0_ON_L + 4 * channel + 3, off >> 8);
}

void moveServo(int channel, float angle) {
    int pulse_length = static_cast<int>((angle / 180.0) * 500 + 150);  // Map angle to pulse width
    setPWM(channel, 0, pulse_length);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed." << std::endl;
        return -1;
    }

    // Open PCA9685 on I2C
    pca9685 = i2cOpen(1, PCA9685_ADDRESS, 0);
    if (pca9685 < 0) {
        std::cerr << "Failed to open PCA9685 on I2C." << std::endl;
        gpioTerminate();
        return -1;
    }

    setPWMFreq(FREQUENCY);

    // Move each servo to a specified angle
    moveServo(0, 90);   // Move servo on channel 0 to 90 degrees
    moveServo(1, 45);   // Move servo on channel 1 to 45 degrees
    moveServo(2, 135);  // Move servo on channel 2 to 135 degrees
    moveServo(3, 0);    // Move servo on channel 3 to 0 degrees

    usleep(1000000);  // Hold position for 1 second

    // Example of moving servos to another position
    moveServo(0, 180);  // Channel 0 to 180 degrees
    moveServo(1, 90);   // Channel 1 to 90 degrees
    moveServo(2, 0);    // Channel 2 to 0 degrees
    moveServo(3, 45);   // Channel 3 to 45 degrees

    // Close I2C and terminate pigpio
    i2cClose(pca9685);
    gpioTerminate();

    return 0;
}
