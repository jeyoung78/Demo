#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <cmath>

#define PCA9685_ADDR 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE

class PCA9685 {
public:
    PCA9685(int bus, int address) : i2c_address(address) {
        char filename[20];
        snprintf(filename, 19, "/dev/i2c-%d", bus);
        file = open(filename, O_RDWR);
        if (file < 0) {
            std::cerr << "Failed to open the I2C bus\n";
            exit(1);
        }
        if (ioctl(file, I2C_SLAVE, i2c_address) < 0) {
            std::cerr << "Failed to acquire bus access and/or talk to slave\n";
            exit(1);
        }
        reset();
    }

    ~PCA9685() {
        close(file);
    }

    void reset() {
        writeReg(PCA9685_MODE1, 0x00);
    }

    void setPWMFreq(float freq) {
        float prescaleVal = 25000000.0 / 4096.0 / freq - 1.0;
        int prescale = static_cast<int>(std::floor(prescaleVal + 0.5));
        int oldmode = readReg(PCA9685_MODE1);
        int newmode = (oldmode & 0x7F) | 0x10;  // sleep
        writeReg(PCA9685_MODE1, newmode);       // go to sleep
        writeReg(PCA9685_PRESCALE, prescale);
        writeReg(PCA9685_MODE1, oldmode);
        usleep(5000);
        writeReg(PCA9685_MODE1, oldmode | 0x80);  // wake up
    }

    void setPWM(int channel, int on, int off) {
        writeReg(0x06 + 4 * channel, on & 0xFF);
        writeReg(0x07 + 4 * channel, on >> 8);
        writeReg(0x08 + 4 * channel, off & 0xFF);
        writeReg(0x09 + 4 * channel, off >> 8);
    }

private:
    int file;
    int i2c_address;

    void writeReg(int reg, int value) {
        char buffer[2];
        buffer[0] = reg;
        buffer[1] = value;
        if (write(file, buffer, 2) != 2) {
            std::cerr << "Failed to write to the i2c bus\n";
        }
    }

    int readReg(int reg) {
        if (write(file, &reg, 1) != 1) {
            std::cerr << "Failed to write to the i2c bus for reading\n";
        }
        char buffer[1];
        if (read(file, buffer, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus\n";
        }
        return buffer[0];
    }
};

int main() {
    PCA9685 pwm(1, PCA9685_ADDR);
    pwm.setPWMFreq(50);  // Set frequency to 50Hz for servo control

    int motorChannels[4] = {0, 1, 2, 3};  // Define channels for the four motors
    int minPulse = 0;                   // Min pulse length out of 4096
    int maxPulse = 4096;                   // Max pulse length out of 4096

    while (true) {
        // Move all motors to minimum position
        for (int i = 0; i < 4; i++) {
            pwm.setPWM(motorChannels[i], 0, minPulse);
        }
        usleep(1000000);  // 1 second delay

        // Move all motors to maximum position
        for (int i = 0; i < 4; i++) {
            pwm.setPWM(motorChannels[i], 0, maxPulse);
        }
        usleep(1000000);  // 1 second delay
    }

    return 0;
}
