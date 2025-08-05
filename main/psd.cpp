#include "psd.h"
#include "utils.h"
#include "logger.h"
#include "PsdSensor.h"
#include <Arduino.h>

// Define buffer size constant
#define PSD_BUFFER_SIZE 3

// Define the distance variable (actual definition of extern variable)
float psd_distance = 0;

PsdSensor psd(PSD_SENSOR_PIN);

void initPsdSensor() {
    // Initialize distance variable
    psd_distance = 0;
    
    logMessage("PSD sensor initialized");
}

void updatePsdSensor() {
    // Read analog value
    psd.update();
    static float psd_buffer[PSD_BUFFER_SIZE];
    static int buffer_index = 0;
    
    psd_buffer[buffer_index] = psd.getDistance();
    buffer_index = (buffer_index + 1) % PSD_BUFFER_SIZE;
    
    psd_distance = median(psd_buffer, PSD_BUFFER_SIZE);
    
    // Log distance for debugging
    // logMessage("PSD distance: " + String(psd_distance) + " cm");
}
