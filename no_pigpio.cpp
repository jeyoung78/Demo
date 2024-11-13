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

// Write a single byte to a specific register
void writeByte(int file, uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    if (write(file, buffer, 2) != 2) {
        std::cerr << "Failed to write to I2C device" << std::endl;
    }
}

// Initialize the PCA9685
void initPCA9685(int file) {
    ioctl(file, I2C_SLAVE, PCA9685_ADDRESS);
    // Set the MODE1 register to 0x10 to sleep
    writeByte(file, MODE1, 0x10);

    // Set the prescale to get a 50Hz frequency (for servos)
    int prescale_value = std::round(25000000.0 / (4096 * 50) - 1);
    writeByte(file, PRESCALE, prescale_value);

    // Wake up the PCA9685
    writeByte(file, MODE1, 0x00);
    usleep(500);
    writeByte(file, MODE1, 0x80); // Auto-increment enabled
}

// Set PWM for a specific channel
void setPWM(int file, int channel, int on, int off) {
    int reg_base = LED0_ON_L + 4 * channel;

    writeByte(file, reg_base, on & 0xFF);        // ON low byte
    writeByte(file, reg_base + 1, on >> 8);      // ON high byte
    writeByte(file, reg_base + 2, off & 0xFF);   // OFF low byte
    writeByte(file, reg_base + 3, off >> 8);     // OFF high byte
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

    // Define back-and-forth movement parameters
    int min_angle = 0;   // Minimum angle
    int max_angle = 180; // Maximum angle
    int delay = 1000000; // Delay between movements in microseconds (1 second)
    std::cerr << "start" << std::endl;
    while (true) { // Infinite loop to keep moving back and forth
        // Move to max_angle
        for (int i = 0; i < 1; i++) {
            int pulse = angleToPWM(1600);
            setPWM(file, i, 0, 1600);
        }
        usleep(delay); // Wait before changing direction

        // Move to min_angle
        for (int i = 0; i < 1; i++) {
            int pulse = angleToPWM(0);
            setPWM(file, i, 0, 40);
        }
        usleep(delay); // Wait before changing direction
    }

    close(file);
    return 0;
}

