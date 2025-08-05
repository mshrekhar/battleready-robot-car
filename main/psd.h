#ifndef PSD_H
#define PSD_H

#include "vive510.h"

// Pin definitions
#define PSD_SENSOR_PIN 4 // pin receiving signal from PSD sensor

// External variables for sensor coordinates
extern float psd_distance;

// Function declarations
void updatePsdSensor();

#endif 