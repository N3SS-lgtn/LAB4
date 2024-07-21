#include "mbed.h"
#include "LIS3DSH.h"

// Initialize the serial interface
UnbufferedSerial pc(USBTX, USBRX);

// Initialize the I2C interface for the accelerometer
I2C i2c(I2C_SDA, I2C_SCL);

// Initialize the accelerometer
LIS3DSH acc(i2c);

// Buffer for serial data
char buffer[32];
volatile bool data_ready = false;

// Function to compute the pitch angle from accelerometer data
float computeAngle(float x, float y, float z) {
    // Calculate the pitch angle in degrees
    float angle = atan2(y, sqrt(x * x + z * z)) * 180.0 / M_PI;
    return angle;
}

// Interrupt service routine for serial input
void serialISR() {
    pc.read(buffer, sizeof(buffer));
    data_ready = true;
}

int main() {
    pc.baud(9600); // Set baud rate
    pc.attach(&serialISR, SerialBase::RxIrq); // Attach interrupt

    pc.printf("Starting accelerometer pitch calculation...\n");

    while (true) {
        // Check if data is ready
        if (data_ready) {
            // Reset the flag
            data_ready = false;

            // Process the input
            switch (buffer[0]) {
                case 'a': {
                    // Read the accelerometer values
                    float x = acc.readX();
                    float y = acc.readY();
                    float z = acc.readZ();

                    // Compute the pitch angle
                    float pitch = computeAngle(x, y, z);

                    // Print the pitch angle
                    pc.printf("Pitch angle: %.2f degrees\n", pitch);
                    break;
                }
                // Add cases for other sensors or commands as needed
                default:
                    pc.printf("Unknown command\n");
                    break;
            }
        }
        // Short delay to prevent busy-waiting
        wait_us(10000);
    }
}