#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cmath>

#define PCA9685_ADDRESS 0x40 // Default I2C address for PCA9685
#define MODE1 0x00
#define PRESCALE 0xFE

// Define registers for each PWM channel
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

// Initialize the PCA9685
void initPCA9685(int file) {
    ioctl(file, I2C_SLAVE, PCA9685_ADDRESS);
    // Set the MODE1 register to 0x10 to sleep
    i2c_smbus_write_byte_data(file, MODE1, 0x10);

    // Set the prescale to get a 50Hz frequency (for servos)
    int prescale_value = std::round(25000000.0 / (4096 * 50) - 1);
    i2c_smbus_write_byte_data(file, PRESCALE, prescale_value);

    // Wake up the PCA9685
    i2c_smbus_write_byte_data(file, MODE1, 0x00);
    usleep(500);
    i2c_smbus_write_byte_data(file, MODE1, 0x80); // Auto-increment enabled
}

// Set PWM for a specific channel
void setPWM(int file, int channel, int on, int off) {
    int reg_base = LED0_ON_L + 4 * channel;
    i2c_smbus_write_byte_data(file, reg_base, on & 0xFF);
    i2c_smbus_write_byte_data(file, reg_base + 1, on >> 8);
    i2c_smbus_write_byte_data(file, reg_base + 2, off & 0xFF);
    i2c_smbus_write_byte_data(file, reg_base + 3, off >> 8);
}

// Convert angle to PWM pulse width
int angleToPWM(int angle) {
    int pulse_min = 150;   // Min pulse length out of 4096
    int pulse_max = 600;   // Max pulse length out of 4096
    int pulse = pulse_min + (angle * (pulse_max - pulse_min) / 180);
    return pulse;
}

int main() {
    const char *device = "/dev/i2c-1";
    int file = open(device, O_RDWR);
    if (file < 0) {
        std::cerr << "Failed to open I2C device" << std::endl;
        return 1;
    }

    initPCA9685(file);

    // Set angles for four servo motors (channel 0 to channel 3)
    int angles[4] = {0, 45, 90, 135}; // Replace with desired angles

    for (int i = 0; i < 4; i++) {
        int pulse = angleToPWM(angles[i]);
        setPWM(file, i, 0, pulse);
        usleep(500000); // Short delay between setting each servo
    }

    close(file);
    return 0;
}
