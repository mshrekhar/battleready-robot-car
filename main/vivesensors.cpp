#include "vivesensors.h"
#include "logger.h"
#include "vive510.h"
#include <Arduino.h>

// Create Vive tracker instances
Vive510 front(VIVESIGNALPIN1);
Vive510 back(VIVESIGNALPIN2);

// Define the coordinate variables (these are the actual definitions of the extern variables)
uint16_t front_x;
uint16_t front_y;
uint16_t back_x;
uint16_t back_y;
uint16_t center_x;
uint16_t center_y;

// Reset flag
bool resetVive = false;
bool checkForFlipping = false;

// Median filter size
#define MEDIAN_FILTER_SIZE 11

uint32_t getMedian(uint32_t* buffer) {
    uint32_t temp[MEDIAN_FILTER_SIZE];
    memcpy(temp, buffer, MEDIAN_FILTER_SIZE * sizeof(uint32_t));
    for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE - 1; i++) {
        for (uint8_t j = 0; j < MEDIAN_FILTER_SIZE - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                uint32_t t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    return temp[MEDIAN_FILTER_SIZE / 2];
}

// Median filter function
uint32_t medianFilter(uint32_t newValue, uint32_t* buffer, uint8_t& index, uint8_t& count) {
    // static uint8_t index = 0;
    // static uint8_t count = 0;  // tracks how many valid entries are in the buffer

    if (checkForFlipping) {
         // Accept unconditionally until buffer is filled at least once
        if (count < MEDIAN_FILTER_SIZE && newValue > 2000 && newValue < 8000) {
            buffer[index] = newValue;
            index = (index + 1) % MEDIAN_FILTER_SIZE;
            count++;
        } else {
            // Copy buffer to compute current median
            uint32_t temp[MEDIAN_FILTER_SIZE];
            memcpy(temp, buffer, MEDIAN_FILTER_SIZE * sizeof(uint32_t));
            for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE - 1; i++) {
                for (uint8_t j = 0; j < MEDIAN_FILTER_SIZE - i - 1; j++) {
                    if (temp[j] > temp[j + 1]) {
                        uint32_t t = temp[j];
                        temp[j] = temp[j + 1];
                        temp[j + 1] = t;
                    }
                }
            }

            
            uint32_t median = getMedian(buffer);
            // logMessage("current median: " + String(median));

            // Only accept if within 250 of current median
            if (!(newValue < 2000 || newValue > 8000) && abs((int32_t)newValue - (int32_t)median) <= 500) {
                buffer[index] = newValue;
                index = (index + 1) % MEDIAN_FILTER_SIZE;
            }
        }

        return getMedian(buffer);
    } else {
        buffer[index] = newValue;
        index = (index + 1) % MEDIAN_FILTER_SIZE;
        return getMedian(buffer);
    }

   

    // Return median of current buffer
    // uint32_t temp2[MEDIAN_FILTER_SIZE];
    // memcpy(temp2, buffer, MEDIAN_FILTER_SIZE * sizeof(uint32_t));
    // for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE - 1; i++) {
    //     for (uint8_t j = 0; j < MEDIAN_FILTER_SIZE - i - 1; j++) {
    //         if (temp2[j] > temp2[j + 1]) {
    //             uint32_t t = temp2[j];
    //             temp2[j] = temp2[j + 1];
    //             temp2[j + 1] = t;
    //         }
    //     }
    // }
}


void initViveSensors() {
    front.begin();
    back.begin();
    // logMessage("Vive trackers started");
    
    // Initialize variables
    front_x = 0;
    front_y = 0;
    back_x = 0;
    back_y = 0;
    center_x = 0;
    center_y = 0;
}

void updateVive() {
     static uint32_t x_buffer1[MEDIAN_FILTER_SIZE] = {0};
        static uint8_t index_x1 = 0;
        static uint8_t count_x1 = 0; 
        static uint32_t y_buffer1[MEDIAN_FILTER_SIZE] = {0};
        static uint8_t index_y1 = 0;
        static uint8_t count_y1 = 0;

         static uint32_t x_buffer2[MEDIAN_FILTER_SIZE] = {0};
        static uint8_t index_x2 = 0;
        static uint8_t count_x2 = 0;

        static uint32_t y_buffer2[MEDIAN_FILTER_SIZE] = {0};
        static uint8_t index_y2 = 0;
        static uint8_t count_y2 = 0;


    if (resetVive) {
        front_x = 0;
        front_y = 0;
        back_x = 0;
        back_y = 0;
        center_x = 0;
        center_y = 0;

        for (int i = 0; i < MEDIAN_FILTER_SIZE; i++) {
            x_buffer1[i] = 0;
            y_buffer1[i] = 0;
            x_buffer2[i] = 0;
            y_buffer2[i] = 0;
        }
        index_x1 = 0;
        index_y1 = 0;
        index_x2 = 0;
        index_y2 = 0;
        count_x1 = 0;
        count_y1 = 0;
        count_x2 = 0;
        count_y2 = 0;

        resetVive = false;
    }
    // Process first sensor
    if (front.status() == VIVE_RECEIVING) {
       
        
        uint16_t x_raw = front.xCoord();
        uint16_t y_raw = front.yCoord();
        // logMessage("front raw: " + String(x_raw) + "," + String(y_raw));

        front_x = medianFilter(x_raw, x_buffer1, index_x1, count_x1);
        front_y = medianFilter(y_raw, y_buffer1, index_y1, count_y1);
        
        if (front_x > 8000 || front_y > 8000 || front_x < 2000 || front_y < 2000) {
            // logMessage("vive1 out of range");
            front_x = 0;
            front_y = 0;
        }

        // For now, use front coordinates as center
        // center_x = front_x;
        // center_y = front_y;

        // Print coordinates for debugging
        // logMessage("front: " + String(front_x) + "," + String(front_y));
    }
    else {
        // front_x = 0;
        // front_y = 0;
        front.sync(5);
        // logMessage("vive 1 not receiving");
    }
    // delay(100);
    // Process second sensor
    if (back.status() == VIVE_RECEIVING) {
       
        
        uint16_t x_raw = back.xCoord();
        uint16_t y_raw = back.yCoord();
        // logMessage("back raw: " + String(x_raw) + "," + String(y_raw));
        
        back_x = medianFilter(x_raw, x_buffer2, index_x2, count_x2);
        back_y = medianFilter(y_raw, y_buffer2, index_y2, count_y2);
        
        if (back_x > 8000 || back_y > 8000 || back_x < 1000 || back_y < 1000) {
            // logMessage("vive2 out of range");
            back_x = 0;
            back_y = 0;
        }

        // Print coordinates for debugging
        // logMessage("back: " + String(back_x) + "," + String(back_y));
    }
    else {
        // back_x = 0;
        // back_y = 0;
        back.sync(5);
        // logMessage("vive 2 not receiving");
    }
} 