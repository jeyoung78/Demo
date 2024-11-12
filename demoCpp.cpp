#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <cmath>

#define PCA9685_ADDR 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE

class PCA9685 {
public:
    PCA9685(int address) {
        fd = wiringPiI2CSetup(address);
        reset();
    }

    void reset() {
        wiringPiI2CWriteReg8(fd, PCA9685_MODE1, 0x00);
    }

    void setPWMFreq(float freq) {
        float prescaleVal = 25000000.0 / 4096.0 / freq - 1.0;
        int prescale = static_cast<int>(std::floor(prescaleVal + 0.5));

        int oldmode = wiringPiI2CReadReg8(fd, PCA9685_MODE1);
        int newmode = (oldmode & 0x7F) | 0x10;  // sleep
        wiringPiI2CWriteReg8(fd, PCA9685_MODE1, newmode);  // go to sleep
        wiringPiI2CWriteReg8(fd, PCA9685_PRESCALE, prescale);
        wiringPiI2CWriteReg8(fd, PCA9685_MODE1, oldmode);
        delay(5);
        wiringPiI2CWriteReg8(fd, PCA9685_MODE1, oldmode | 0x80);  // wake up
    }

    void setPWM(int channel, int on, int off) {
        wiringPiI2CWriteReg8(fd, 0x06 + 4 * channel, on & 0xFF);
        wiringPiI2CWriteReg8(fd, 0x07 + 4 * channel, on >> 8);
        wiringPiI2CWriteReg8(fd, 0x08 + 4 * channel, off & 0xFF);
        wiringPiI2CWriteReg8(fd, 0x09 + 4 * channel, off >> 8);
    }

private:
    int fd;
};

int main() {
    wiringPiSetup();
    PCA9685 pwm(PCA9685_ADDR);
    pwm.setPWMFreq(50);  // Set frequency to 50Hz for servo control

    int servoChannel = 15;  // Channel to which the servo is connected
    int minPulse = 150;    // Min pulse length out of 4096
    int maxPulse = 600;    // Max pulse length out of 4096

    while (true) {
        // Move servo to minimum position
        pwm.setPWM(servoChannel, 0, minPulse);
        delay(1000);

        // Move servo to maximum position
        pwm.setPWM(servoChannel, 0, maxPulse);
        delay(1000);
    }

    return 0;
}

    gpioTerminate();
    return 0;
}
