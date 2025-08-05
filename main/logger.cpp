#include "logger.h"
#include "webserver.h"

// Buffer to store recent logs
#define MAX_LOG_ENTRIES 100
String logBuffer[MAX_LOG_ENTRIES];
int logIndex = 0;
bool bufferFull = false;

void logMessage(const String& message) {
    // Print to Serial
    // Serial.println(message);
    
    // Add to circular buffer
    // logBuffer[logIndex] = message;
    // logIndex = (logIndex + 1) % MAX_LOG_ENTRIES;
    // if (logIndex == 0) {
    //     bufferFull = true;
    // }
}

// Function to get all logs as a single string
String getAllLogs() {
    String allLogs = "";
    // int startIndex = bufferFull ? logIndex : 0;
    // int count = bufferFull ? MAX_LOG_ENTRIES : logIndex;
    
    // for (int i = 0; i < count; i++) {
    //     int currentIndex = (startIndex + i) % MAX_LOG_ENTRIES;
    //     allLogs += logBuffer[currentIndex] + "\n";
    // }
    
    return allLogs;
} 