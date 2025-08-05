#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

// Speeds
// #define DEFAULT_SPEED_A 0.83
// #define DEFAULT_SPEED_B 0.78
#define DEFAULT_SPEED_A 0.92
#define DEFAULT_SPEED_B 0.85
#define WALL_SPEED_A 0.83
#define WALL_SPEED_B 0.78
#define DEFAULT_TURNING_SPEED 0.70
#define WALL_TURNING_SPEED 0.44
#define ATTACK_TURNING_SPEED 0.44

// Motor control functions
void setupMotorControl();
void updateMotorControl();
void setSpeedInput(float speed);
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMotors();

// External declarations for variables that need to be accessed by other files
extern float speedInput;
extern float speedControlA;
extern float speedControlB;
extern char direction;
extern float summederrorA;
extern float summederrorB;

#endif 