#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "html510.h"

// External declaration of the HTML server instance
extern HTML510Server webServer;

// Function to log messages to both Serial and Web
void logMessage(const String& message);

// Function to get all logs as a single string
String getAllLogs();

#endif 