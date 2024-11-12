#include <iostream>
#include <wiringPi.h>
#include <unistd.h>  // For usleep
#include "Adafruit_PWMServoDriver.h"

// Set up PCA9685
Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver();
const int servoChannel = 0;

// Function to map angle to pulse width
int angleToPulse(int angle) {
    int minPulse = 500;  // in microseconds
    int maxPulse = 2500; // in microseconds
    return minPulse + (angle * (maxPulse - minPulse) / 180);
}

// Function to set servo angle
void setServoAngle(int angle) {
    int pulse = angleToPulse(angle);
    pca.writeMicroseconds(servoChannel, pulse);
}

int main() {
    wiringPiSetup();  // Initialize WiringPi
    pca.begin();
    pca.setPWMFreq(50);  // Set frequency to 50 Hz for servos

    try {
        // Simulate 360-degree rotation by moving back and forth
        while (true) {
            setServoAngle(0);
            usleep(200000);  // sleep for 200 ms

            setServoAngle(30);
            usleep(200000);  // sleep for 200 ms
        }
    } catch (...) {
        // Graceful exit
        setServoAngle(90);  // Neutral position (optional)
    }

    return 0;
}
