#include <pigpio.h>
#include <iostream>
#include <cmath>

#define PCA9685_ADDR 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE

class PCA9685 {
public:
    PCA9685(int address) {
        gpioInitialise();  // Initialize pigpio
        i2c_handle = i2cOpen(1, address, 0);
        reset();
    }

    ~PCA9685() {
        i2cClose(i2c_handle);  // Close the I2C handle
        gpioTerminate();       // Terminate pigpio
    }

    void reset() {
        i2cWriteByteData(i2c_handle, PCA9685_MODE1, 0x00);
    }

    void setPWMFreq(float freq) {
        float prescaleVal = 25000000.0 / 4096.0 / freq - 1.0;
        int prescale = static_cast<int>(std::floor(prescaleVal + 0.5));

        int oldmode = i2cReadByteData(i2c_handle, PCA9685_MODE1);
        int newmode = (oldmode & 0x7F) | 0x10;  // sleep
        i2cWriteByteData(i2c_handle, PCA9685_MODE1, newmode);  // go to sleep
        i2cWriteByteData(i2c_handle, PCA9685_PRESCALE, prescale);
        i2cWriteByteData(i2c_handle, PCA9685_MODE1, oldmode);
        gpioDelay(5000);
        i2cWriteByteData(i2c_handle, PCA9685_MODE1, oldmode | 0x80);  // wake up
    }

    void setPWM(int channel, int on, int off) {
        i2cWriteByteData(i2c_handle, 0x06 + 4 * channel, on & 0xFF);
        i2cWriteByteData(i2c_handle, 0x07 + 4 * channel, on >> 8);
        i2cWriteByteData(i2c_handle, 0x08 + 4 * channel, off & 0xFF);
        i2cWriteByteData(i2c_handle, 0x09 + 4 * channel, off >> 8);
    }

private:
    int i2c_handle;
};

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio." << std::endl;
        return 1;
    }

    PCA9685 pwm(PCA9685_ADDR);
    pwm.setPWMFreq(50);  // Set frequency to 50Hz for servo control

    int servoChannel = 0;  // Channel to which the servo is connected
    int minPulse = 150;    // Min pulse length out of 4096
    int maxPulse = 600;    // Max pulse length out of 4096

    while (true) {
        // Move servo to minimum position
        pwm.setPWM(servoChannel, 0, minPulse);
        gpioDelay(1000000);  // 1 second delay

        // Move servo to maximum position
        pwm.setPWM(servoChannel, 0, maxPulse);
        gpioDelay(1000000);  // 1 second delay
    }

    gpioTerminate();  // Clean up pigpio before exiting
    return 0;
}
