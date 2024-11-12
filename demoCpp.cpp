#include <pigpio.h>
#include <iostream>
#include <unistd.h>

// PCA9685 settings
#define I2C_ADDR 0x40  // Default address for PCA9685
#define SERVO_CHANNEL 0  // Channel for the servo
#define MIN_PULSE 500   // Minimum pulse width
#define MAX_PULSE 2500  // Maximum pulse width

// Helper function to write to PCA9685 registers
void setPCA9685Register(int handle, uint8_t reg, uint8_t value) {
    i2cWriteByteData(handle, reg, value);
}

void initializePCA9685(int handle) {
    setPCA9685Register(handle, 0x00, 0x10);  // Enter sleep mode
    setPCA9685Register(handle, 0xFE, 121);   // Set prescale for 50Hz frequency
    setPCA9685Register(handle, 0x00, 0x00);  // Wake up from sleep mode
    usleep(500);
}

void setServoAngle(int handle, int channel, int angle) {
    int pulseWidth = MIN_PULSE + (MAX_PULSE - MIN_PULSE) * angle / 180;
    int onTime = 0;
    int offTime = pulseWidth * 4096 / 20000;  // Convert pulse width to 12-bit range

    i2cWriteByteData(handle, 0x06 + 4 * channel, onTime & 0xFF);
    i2cWriteByteData(handle, 0x07 + 4 * channel, onTime >> 8);
    i2cWriteByteData(handle, 0x08 + 4 * channel, offTime & 0xFF);
    i2cWriteByteData(handle, 0x09 + 4 * channel, offTime >> 8);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize GPIO." << std::endl;
        return -1;
    }

    int handle = i2cOpen(1, I2C_ADDR, 0);
    if (handle < 0) {
        std::cerr << "Failed to open I2C." << std::endl;
        gpioTerminate();
        return -1;
    }

    initializePCA9685(handle);

    try {
        while (true) {
            setServoAngle(handle, SERVO_CHANNEL, 0);
            usleep(200000);
            setServoAngle(handle, SERVO_CHANNEL, 30);
            usleep(200000);
        }
    } catch (...) {
        setServoAngle(handle, SERVO_CHANNEL, 0);  // Reset to default
        i2cClose(handle);
        gpioTerminate();
    }

    i2cClose(handle);
    gpioTerminate();
    return 0;
}
