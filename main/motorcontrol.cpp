#include "motorcontrol.h"
#include "logger.h"
#include "wallfollowing.h"
#include <Arduino.h>
#include "autodrive.h"

// Pin Definitions
const int DIR1A_PIN = 17;
const int DIR2A_PIN = 16;
const int SLP_PIN = 15;
const int DIR1B_PIN = 6;
const int DIR2B_PIN = 7;
const int ENCODERA_A1_PIN = 1;
const int ENCODERB_A2_PIN = 4;

// PID Values
float KpA = .0014583;
float KiA = .000009107;
float KdA = 0;
float KpB = .0022465;
float KiB = .00001427;
float KdB = 0;

// Motor control variables
float speedControlA = 0;
float speedControlB = 0;
float speedInput = 0;
float velocityA = 0;
float velocityA_1 = 0;
float velocityA_2 = 0;
float velocityA_3 = 0;
float velocityB = 0;
float velocityB_1 = 0;
float velocityB_2 = 0;
float velocityB_3 = 0;
char direction = 'F';
float uA = 0;
float uB = 0;
float summederrorA = 0;
float summederrorB = 0;

// Encoder variables
volatile long pulse_countA = 0;
volatile long pulse_countB = 0;

// Timing variables
unsigned long last_time = 0;
const unsigned long interval = 10;
portMUX_TYPE motorMux = portMUX_INITIALIZER_UNLOCKED;

float median_of_three(float a, float b, float c) {
  if ((a >= b && a <= c) || (a >= c && a <= b))
    return a;
  else if ((b >= a && b <= c) || (b >= c && b <= a))
    return b;
  else
    return c;
}

void IRAM_ATTR encoder_ISRA() {
  portENTER_CRITICAL_ISR(&motorMux);
  pulse_countA++;
  portEXIT_CRITICAL_ISR(&motorMux);  
}

void IRAM_ATTR encoder_ISRB() {
  portENTER_CRITICAL_ISR(&motorMux);
  pulse_countB++;
  portEXIT_CRITICAL_ISR(&motorMux);  
}

float pidControlA(int desired, int sensor) {
  float u;
  static int oldsensorA;
  int velocity = sensor - oldsensorA;
  summederrorA += (desired-sensor);
  u = (KpA * (desired-sensor)) + (KdA * velocity) + (KiA * summederrorA);
  if (desired-sensor < 0) summederrorA *= 0.5;
  if (summederrorA > 10000) summederrorA = 10000;
  if (u >= .94) u = .94;
  if (u < 0) u = 0;
  oldsensorA = sensor;
  if (u <= .25 && u != 0) u = .25;
  logMessage("Desired A: " + String(desired) + ", Sensor A: " + String(sensor) + ", uA: " + String(u));
  return u;
}

float pidControlB(int desired, int sensor) {
  float u;
  static int oldsensorB;
  int velocity = sensor - oldsensorB;
  summederrorB += (desired-sensor);
  u = (KpB * (desired-sensor)) + (KdB * velocity) + (KiB * summederrorB);
  if (desired-sensor < 0) summederrorB *= 0.5;
  if (summederrorB > 10000) summederrorB = 10000;
  if (u >= .94) u = .94;
  if (u < 0) u = 0;
  oldsensorB = sensor;
  if (u <= .15 && u != 0) u = .15;
  logMessage("Desired B: " + String(desired) + ", Sensor B: " + String(sensor) + ", uB: " + String(u));
  return u;
}

float encoderFilterA(float v) {
  velocityA_3 = velocityA_2;
  velocityA_2 = velocityA_1;
  velocityA_1 = v;
  return median_of_three(velocityA_1, velocityA_2, velocityA_3);
}

float encoderFilterB(float v) {
  velocityB_3 = velocityB_2;
  velocityB_2 = velocityB_1;
  velocityB_1 = v;
  return median_of_three(velocityB_1, velocityB_2, velocityB_3);
}

void driveMotors(float uA, float uB, char direction) {
  if (direction == 'F') {
    ledcWrite(DIR1A_PIN, uA*1084);
    ledcWrite(DIR2A_PIN, 0);
    ledcWrite(DIR1B_PIN, 0);
    ledcWrite(DIR2B_PIN, uB*1084);
  } else {
    ledcWrite(DIR1A_PIN, 0);
    ledcWrite(DIR2A_PIN, uA*1084);
    ledcWrite(DIR1B_PIN, uB*1084);
    ledcWrite(DIR2B_PIN, 0);
  }
}

void setupMotorControl() {
  // Setup for Motor Driver Pins
  ledcAttach(DIR1A_PIN, 1000, 10);
  ledcAttach(DIR2A_PIN, 1000, 10);
  ledcAttach(DIR1B_PIN, 1000, 10);
  ledcAttach(DIR2B_PIN, 1000, 10);
  pinMode(SLP_PIN, OUTPUT);
  digitalWrite(SLP_PIN, HIGH);

  // Encoder Setup
  pinMode(ENCODERA_A1_PIN, INPUT_PULLUP);
  pinMode(ENCODERB_A2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODERA_A1_PIN), encoder_ISRA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODERB_A2_PIN), encoder_ISRB, CHANGE);
}

void updateMotorControl() {
  // unsigned long current_time = millis();
  // if (current_time - last_time >= interval) {
  //   unsigned long elapsed_time = current_time - last_time;
  //   last_time = current_time;

  //   // Calculate velocity: pulses per second
  //   velocityA = encoderFilterA((pulse_countA * 1000.0) / elapsed_time);
  //   velocityB = encoderFilterB((pulse_countB * 1000.0) / elapsed_time);

  //   // Reset pulse count
  //   portENTER_CRITICAL_ISR(&motorMux);
  //   pulse_countA = 0;
  //   pulse_countB = 0;
  //   portEXIT_CRITICAL_ISR(&motorMux);
  // }

  if (speedControlA != 0) {
    uA = speedControlA;
  } else {
    uA = 0;
  }
  if (speedControlB != 0) {
    uB = speedControlB;
  } else {
    uB = 0;
  }

  driveMotors(uA, uB, direction);
}

float getSpeedA() {
  return (isWallFollowing || isAutoMode || isAutoAttacking) ? WALL_SPEED_A : DEFAULT_SPEED_A;
}

float getSpeedB() {
  return (isWallFollowing || isAutoMode || isAutoAttacking) ? WALL_SPEED_B : DEFAULT_SPEED_B;
}

float getTurningSpeed() {
  return (isWallFollowing || isAutoMode) ? WALL_TURNING_SPEED : (isAutoAttacking ? ATTACK_TURNING_SPEED : DEFAULT_TURNING_SPEED);
}

void setSpeedControlA(float speed) {
  speedControlA = speed;
}

void setSpeedControlB(float speed) {
  speedControlB = speed;
}

void setDirection(char dir) {
  direction = dir;
}

void resetPIDErrors() {
  summederrorA = 0;
  summederrorB = 0;
}

void setSpeedInput(float speed) {
  //speedInput = speed;
}

void moveForward() {
  // logMessage("MOTOR FWD");
  setDirection('F');
  setSpeedControlA(getSpeedA());
  setSpeedControlB(getSpeedB());
  //resetPIDErrors();
}

void moveBackward() {
  setDirection('B');
  setSpeedControlA(getSpeedA());
  setSpeedControlB(getSpeedB());
  // resetPIDErrors();
}

void turnLeft() {
  // logMessage("MOTOR LEFT");
  setDirection('F');
  setSpeedControlA(getTurningSpeed());
  setSpeedControlB(0);
  // resetPIDErrors();
}

void turnRight() {
  // logMessage("MOTOR RIGHT");
  setDirection('F');
  setSpeedControlA(0);
  setSpeedControlB(getTurningSpeed());
  // resetPIDErrors();
}

void stopMotors() {
  // logMessage("STOP MOTORS");
  setSpeedControlA(0);
  setSpeedControlB(0);
  setDirection('F');
  // resetPIDErrors();
} 