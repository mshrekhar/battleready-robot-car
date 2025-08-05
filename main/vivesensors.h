#ifndef VIVESENSORS_H
#define VIVESENSORS_H

#include <Arduino.h>

// Pin definitions
#define VIVESIGNALPIN1 42 // pin receiving signal from Vive circuit 1
#define VIVESIGNALPIN2 36 // pin receiving signal from Vive circuit 2

// External variables for sensor coordinates
extern uint16_t front_x;
extern uint16_t front_y;
extern uint16_t back_x;
extern uint16_t back_y;
extern uint16_t center_x;
extern uint16_t center_y;

// Reset flag
extern bool resetVive;
extern bool checkForFlipping;

// Function declarations
void initViveSensors();
void updateVive();
void resetViveCoordinates();
void resetViveBuffer();

#endif 