#include "ultrasonicsensors.h"
#include "utils.h"
#include "logger.h"
#include <Arduino.h>

// Define the distance variable (actual definition of extern variable)
float left_sensor_distance;

// Buffer for median filtering
#define ULTRASONIC_BUFFER_SIZE 10
static float ultrasonic_buffer[ULTRASONIC_BUFFER_SIZE] = {0};
static int buffer_index = 0;

// Interrupt variables
static volatile bool echo_received = false;
static volatile unsigned long echo_start_time = 0;
static volatile unsigned long echo_duration = 0;
static volatile bool reading_in_progress = false;

// Interrupt handler for echo pin
void IRAM_ATTR onEchoInterrupt() {
    if (reading_in_progress) {
        if (digitalRead(LEFTECHOPIN) == HIGH) {
            // Rising edge - start of echo
            echo_start_time = micros();
        } else {
            // Falling edge - end of echo
            echo_duration = micros() - echo_start_time;
            echo_received = true;
            reading_in_progress = false;
        }
    }
}

void initUltrasonicSensors() {
    // Configure pins
    pinMode(LEFTTRIGPIN, OUTPUT);
    pinMode(LEFTECHOPIN, INPUT);
    
    // Attach interrupt to echo pin
    attachInterrupt(digitalPinToInterrupt(LEFTECHOPIN), onEchoInterrupt, CHANGE);
    
    // Initialize distance variable
    left_sensor_distance = 0;
    logMessage("Ultrasonic sensors initialized");
}

void startUltrasonicReading() {
    if (!reading_in_progress) {
        reading_in_progress = true;
        echo_received = false;
        
        // Send trigger pulse
        digitalWrite(LEFTTRIGPIN, LOW);
        delayMicroseconds(2);
        digitalWrite(LEFTTRIGPIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(LEFTTRIGPIN, LOW);
        
        // logMessage("Trigger pulse sent");
    }
}

float getLeftSensorDistance() {
    if (echo_received) {
        // Calculate distance in cm
        float raw_distance = echo_duration * 0.0343 / 2;
        
        // Only use readings within reasonable range (2cm to 400cm)
        if (raw_distance >= 2.0 && raw_distance <= 400.0) {
            // logMessage("Raw distance: " + String(raw_distance) + " cm");
            
            // Update circular buffer
            ultrasonic_buffer[buffer_index] = raw_distance;
            buffer_index = (buffer_index + 1) % ULTRASONIC_BUFFER_SIZE;
            
            // Calculate median
            left_sensor_distance = median(ultrasonic_buffer, ULTRASONIC_BUFFER_SIZE);
            // logMessage("Median distance: " + String(left_sensor_distance) + " cm");
        } else {
            // logMessage("Invalid distance: " + String(raw_distance) + " cm");
        }
        
        // Reset for next reading
        echo_received = false;
    }
    
    return left_sensor_distance;
}
