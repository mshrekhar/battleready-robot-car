#include "PsdSensor.h"
#include <Arduino.h>
#include <math.h>
#include "logger.h"
PsdSensor::PsdSensor(int pin) : analogPin(pin), distanceCm(0) {
}

void PsdSensor::update() {
    int rawValue = analogRead(analogPin);
    float voltage = rawValue * (ADC_REF_VOLTAGE / ADC_RESOLUTION);
    // Approximate distance in cm based on SHARP GP2Y0A21YK0F curve
    distanceCm = 75.0 * pow(voltage, -1.15); // Empirical formula
}

float PsdSensor::getDistance() const {
    return distanceCm;
} 