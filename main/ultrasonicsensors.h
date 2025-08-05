#ifndef ULTRASONICSENSORS_H
#define ULTRASONICSENSORS_H

#include <Arduino.h>

// Pin definitions
#define LEFTTRIGPIN 12
#define LEFTECHOPIN 8

// Ultrasonic states
enum class UltrasonicState {
    IDLE,
    TRIGGER_LOW,
    TRIGGER_HIGH,
    WAITING_ECHO,
    READING_ECHO
};

// External variables for sensor readings
extern float left_sensor_distance;

// Function declarations
void initUltrasonicSensors();
void updateUltrasonicSensors();
float getLeftSensorDistance();

// Non-blocking functions
void startUltrasonicReading();
bool isUltrasonicReadingComplete();
void handleUltrasonicState();

#endif 