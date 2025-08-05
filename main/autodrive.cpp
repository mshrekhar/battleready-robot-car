#include "autodrive.h"
#include "motorcontrol.h"
#include "vivesensors.h"
#include "logger.h"

#define ERROR_MARGIN 300
#define ORIENTATION_TOLERANCE 5.0f  // degrees
#define COURSE_CORRECTION_THRESHOLD 10.0f  // degrees

// Waypoint management
static Waypoint waypoints[MAX_WAYPOINTS];
static int numWaypoints = 0;
static int currentWaypointIndex = 0;

// Navigation state
bool isAutoMode = false;
bool reachedTarget = false;
float orientationDiffAngle = 0;
bool isOrienting = true;
float slope1 = 0;
float slope2 = 0;
int testLog1 = 0;

// Current destination
static uint16_t destination_x = 0;
static uint16_t destination_y = 0;

float calculateAngleDifference(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3) {
    // Calculate vectors
    float v1x = x2 - x1;  // Current orientation vector (back to front)
    float v1y = y2 - y1;
    float v2x = x3 - x1;  // Target orientation vector (back to destination)
    float v2y = y3 - y1;
    
    // Calculate dot product and magnitudes
    float dot_product = v1x * v2x + v1y * v2y;
    float v1_mag = sqrt(v1x * v1x + v1y * v1y);
    float v2_mag = sqrt(v2x * v2x + v2y * v2y);
    
    // Calculate angle between vectors using dot product formula
    float angle = acos(dot_product / (v1_mag * v2_mag)) * 180.0f / PI;
    
    // Determine sign of angle using cross product
    float cross_product = v1x * v2y - v1y * v2x;
    if (cross_product < 0) {
        angle = -angle;
    }
    
    return angle;
}

bool isAtPosition(uint16_t target_x, uint16_t target_y) {
    // Check if current position is within error margin of target
    float dx = front_x - target_x;
    float dy = front_y - target_y;
    float distance = sqrt(dx*dx + dy*dy);
    
    return distance <= ERROR_MARGIN;
}

void addWaypoint(uint16_t x, uint16_t y) {
    if (numWaypoints < MAX_WAYPOINTS) {
        waypoints[numWaypoints].x = x;
        waypoints[numWaypoints].y = y;
        numWaypoints++;
        logMessage("Added waypoint " + String(numWaypoints) + ": (" + String(x) + ", " + String(y) + ")");
    } else {
        logMessage("Cannot add more waypoints - maximum reached");
    }
}

void clearWaypoints() {
    numWaypoints = 0;
    currentWaypointIndex = 0;
    destination_x = 0;
    destination_y = 0;
    logMessage("Cleared all waypoints");
}

void setNextWaypoint() {
    if (currentWaypointIndex < numWaypoints) {
        destination_x = waypoints[currentWaypointIndex].x;
        destination_y = waypoints[currentWaypointIndex].y;
        isOrienting = true;  // Reset orientation for new waypoint
        logMessage("Moving to waypoint " + String(currentWaypointIndex + 1) + 
                  ": (" + String(destination_x) + ", " + String(destination_y) + ")");
        reachedTarget = false;
    }
}

bool hasMoreWaypoints() {
    return currentWaypointIndex < numWaypoints;
}

int getCurrentWaypointIndex() {
    return currentWaypointIndex;
}

int getTotalWaypoints() {
    return numWaypoints;
}

void setDestination(uint16_t target_x, uint16_t target_y) {
    destination_x = target_x;
    destination_y = target_y;
    isOrienting = true;
}

void drive() {
    // logMessage("Current destination: " + String(destination_x) + "," + String(destination_y));
    
    // If no waypoints, don't drive
    if (numWaypoints == 0) {
        stopMotors();
        return;
    }

    // If we've reached the current waypoint, move to the next one
    if (isAtPosition(destination_x, destination_y)) {
        currentWaypointIndex++;
        reachedTarget = true;
        if (currentWaypointIndex < numWaypoints) {
            setNextWaypoint();
        } else {
            logMessage("Reached all waypoints!");
            // stopMotors();
            isAutoMode = false;
            return;
        }
    }

    // // Print coordinates for debugging
    // logMessage("Current: " + String(front_x) + "," + String(front_y) + 
    //           " Destination: " + String(destination_x) + "," + String(destination_y));

    // Calculate angle difference directly using vectors
    float angle_diff = calculateAngleDifference(back_x, back_y, front_x, front_y, destination_x, destination_y);
    
    // Print orientation info for debugging
    // logMessage("Orienting - Angle diff: " + String(angle_diff));
    orientationDiffAngle = angle_diff;
    
    if (isOrienting) {
        // If angle difference is small enough, switch to movement mode
        if (abs(angle_diff) < ORIENTATION_TOLERANCE) {
            isOrienting = false;
            stopMotors();
            moveForward();
            return;
        }
        
        // Turn in the appropriate direction
        turnRight();
        // if (angle_diff > 0) {
        //     turnRight();
        // } else {
        //     turnLeft();
        // }
    } else {
        // // Check if we need to reorient
        // float angle_diff = calculateAngleDifference(back_x, back_y, front_x, front_y, destination_x, destination_y);
        
        // // If we're significantly off course, go back to orientation mode
        // if (abs(angle_diff) > COURSE_CORRECTION_THRESHOLD) {
        //     isOrienting = true;
        //     return;
        // }
        
        // Continue moving forward
        moveForward();
    }
} 
