#include "webserver.h"
#include "motorcontrol.h"
#include "autodrive.h"
#include "vivesensors.h"
#include "logger.h"
#include "wallfollowing.h"
#include "ultrasonicsensors.h"
#include "psd.h"
#include "autoattack.h"

// WiFi credentials
const char* ssid = "robotChris";
const char* password = "";

int testLog2 = 0;

// HTML server instance definition
HTML510Server webServer(80);

// Define the wifi_packets_used variable that was declared extern in the header
volatile uint8_t wifi_packets_used = 0;

void handleRoot() {
  webServer.sendhtml(body);
  wifi_packets_used++;
}

void handleDuty() {
  float speed = webServer.getVal();
  setSpeedInput(speed);
  wifi_packets_used++;
}

void handleForward() {
  moveForward();
  wifi_packets_used++;
}

void handleLeft() {
  turnLeft();
  wifi_packets_used++;
}

void handleRight() {
  turnRight();
  wifi_packets_used++;
}

void handleBackward() {
  moveBackward();
  wifi_packets_used++;
}

void handleStop() {
  stopMotors();
  isAutoMode = false;
  isWallFollowing = false;
  isAutoAttacking = false;
  wifi_packets_used++;
}

void handleAuto() {
  isAutoAttacking = true;
  // isWallFollowing = false;
  // setNextWaypoint();  // Start with the first waypoint
  wifi_packets_used++;
}

void handleWaypoint() {
  // Get x and y coordinates from the request
  // Format: /waypoint?x=1000&y=2000
  String xStr = webServer.getText();  // Get "x=1000"
  String yStr = webServer.getText();  // Get "y=2000"
  
  // Parse the coordinates
  int x = xStr.substring(2).toInt();  // Remove "x=" and convert to int
  int y = yStr.substring(2).toInt();  // Remove "y=" and convert to int
  
  // Add the waypoint
  addWaypoint(x, y);
  wifi_packets_used++;
}

void handleClearWaypoints() {
  clearWaypoints();
  wifi_packets_used++;
}

void handleWall() {
  isWallFollowing = true;
  isAutoMode = false;
  wifi_packets_used++;
}

void handleOrientationLog() {
  char buf[500];
  snprintf(buf, sizeof(buf), "angle = %.1f\ncurrent_slope = %.1f\ntarget_slope = %.1f\nfront_x = %d, front_y=%d\nback_x = %d, back_y = %d\npsd = %.1f\nultrasonic_left = %.1f\n", 
    orientationDiffAngle, slope1, slope2, front_x, front_y, back_x, back_y, psd_distance, left_sensor_distance);
  // snprintf(buf, sizeof(buf), "%d\n%d\n%d\n", testLog1, testLog2, isAutoMode);
  webServer.sendplain(buf);
}

void handleLogs() {
    String logs = getAllLogs();
    webServer.sendplain(logs);
}

void handleResetVive() {
    resetVive = true;  // Set the flag to trigger reset in updateVive()
    logMessage("Vive reset requested");
}

void setupWebServer() {
  // Setup for WiFi
  IPAddress myIP(192,168,1,104);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(myIP, IPAddress(192,168,1,1), IPAddress(255,255,255,0));

  // Setup for HTML messaging 
  webServer.begin();
  webServer.attachHandler("/duty?val=", handleDuty);
  webServer.attachHandler("/ ", handleRoot);
  webServer.attachHandler("/forward ", handleForward);
  webServer.attachHandler("/backward ", handleBackward);
  webServer.attachHandler("/right", handleRight);
  webServer.attachHandler("/left", handleLeft);
  webServer.attachHandler("/stop", handleStop);
  webServer.attachHandler("/auto", handleAuto);
  webServer.attachHandler("/orientation", handleOrientationLog);
  webServer.attachHandler("/logs", handleLogs);
  webServer.attachHandler("/wall", handleWall);
  webServer.attachHandler("/waypoint", handleWaypoint);
  webServer.attachHandler("/clearwaypoints", handleClearWaypoints);
  webServer.attachHandler("/resetvive", handleResetVive);
}

void resetWifiUsage() {
  wifi_packets_used = 0;
}