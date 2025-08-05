#ifndef TOPHAT_H
#define TOPHAT_H

#include <Wire.h>

// I2C Configuration
#define I2C_SLAVE_ADDR 0x28
#define SDA_PIN 34
#define SCL_PIN 21
#define I2C_FREQ 40000  // 40kHz

// Extern variable for tophat health
extern int current_health;

// Function declarations
void initializeTophat();
void syncWithTophat();

#endif // TOPHAT_H 