#include "utils.h"

// Helper function to get median
float median(float values[], int size) {
    // Sort the array
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (values[j] > values[j + 1]) {
                float temp = values[j];
                values[j] = values[j + 1]; 
                values[j + 1] = temp;
            }
        }
    }
    
    // Return middle value
    return values[size / 2];
} 