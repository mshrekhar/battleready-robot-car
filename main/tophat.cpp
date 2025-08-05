#include "tophat.h"
#include "webserver.h"
#include "logger.h"
#include <Arduino.h>

// Define the tophat health variable
int current_health = 100;  // Initialize to 100%

void initializeTophat() {
    // Enable internal pullups
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);
    
    // Initialize I2C master
    Wire.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
    
    logMessage("Initializing I2C...");
    logMessage("SDA Pin: " + String(SDA_PIN) + ", SCL Pin: " + String(SCL_PIN));
    logMessage("I2C Frequency: " + String(I2C_FREQ) + " Hz");
    logMessage("Slave Address: 0x" + String(I2C_SLAVE_ADDR, HEX));
}

uint8_t updateTophat(uint8_t data) {
    // Send data to slave
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(data);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        logMessage("I2C Write Error: " + String(error));
        switch(error) {
            case 1: logMessage("Data too long to fit in transmit buffer"); break;
            case 2: logMessage("Received NACK on transmit of address"); break;
            case 3: logMessage("Received NACK on transmit of data"); break;
            case 4: logMessage("Other error"); break;
        }
    }
    return error;
}

uint8_t readFromTophat() {
    // Request data from slave
    uint8_t bytesReceived = Wire.requestFrom(I2C_SLAVE_ADDR, 1);
    uint8_t byteIn = 0;

    if (bytesReceived > 0) {
        while (Wire.available()) {
            byteIn = Wire.read();
            // logMessage("Received: 0x" + String(byteIn, HEX));
        }
    } else {
        // logMessage("No bytes received from slave");
        return 100;
    }
    return byteIn;
} 

void syncWithTophat() {
    uint8_t error = updateTophat(wifi_packets_used);
    current_health = (int)readFromTophat();  // Convert hex byte to decimal integer
    // logMessage("Current health: " + String(current_health));
    resetWifiUsage();
}