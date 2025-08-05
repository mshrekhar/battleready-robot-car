#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include "HTML510.h"
#include <webPage.h>

// External declaration of the HTML server instance
extern HTML510Server webServer;

// WiFi credentials
extern const char* ssid;
extern const char* password;

// Wifi usage
extern volatile uint8_t wifi_packets_used;

// Web server functions
void handleRoot();
void handleDuty();
void handleForward();
void handleLeft();
void handleRight();
void handleBackward();
void handleStop();
void handleLogs();
void handleWaypoint();
void handleClearWaypoints();
void handleResetVive();

// Web server setup
void setupWebServer();

// Wifi usage
void resetWifiUsage();

#endif 