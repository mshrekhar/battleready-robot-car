#ifndef AUTODRIVE_H
#define AUTODRIVE_H

#include <Arduino.h>

// Target position for testing autonomous driving
#define TARGET_X 4500
#define TARGET_Y 3000

// Maximum number of waypoints
#define MAX_WAYPOINTS 10

// Waypoint structure
struct Waypoint {
    uint16_t x;
    uint16_t y;
};

extern bool isAutoMode;
extern float orientationDiffAngle;
extern float slope1;
extern float slope2;
extern int testLog1;
extern bool reachedTarget;

// Navigation commands
void setDestination(uint16_t target_x, uint16_t target_y);
void drive();

// Waypoint management
void addWaypoint(uint16_t x, uint16_t y);
void clearWaypoints();
void setNextWaypoint();
bool hasMoreWaypoints();
int getCurrentWaypointIndex();
int getTotalWaypoints();

// Helper functions for orientation
float calculateSlope(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
float calculateAngleDifference(float slope1, float slope2);
bool isAtPosition(uint16_t target_x, uint16_t target_y);

#endif 