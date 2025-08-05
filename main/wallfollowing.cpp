#include "wallfollowing.h"
#include "logger.h"
#include "motorcontrol.h"
#include "psd.h"
#include "interruptmanager.h"
#include "ultrasonicsensors.h"

#include <Arduino.h>

#define FRONT_STOP_DISTANCE 10.0
// #define FRONT_MIN_DISTANCE 10.0
#define LEFT_MIN_DISTANCE 7.0
#define LEFT_MAX_DISTANCE 12.0
#define LEFT_NO_WALL_DISTANCE 50.0

// Define the wall following state
bool isWallFollowing = false;
bool isTransitioning = false;
bool isTurningRight = false;
bool isTurningLeft = false;
bool isCorrectingRight = false;
bool isBigCorrectingRight = false;
bool isBigCorrectingLeft = false;
bool isCorrectingLeft = false;
bool justTurnedLeftDriveForwardFlag = false;
bool justTurnedRightDriveForwardFlag = false;
bool justCorrectedFlag = false;
bool isBackingUp = false;
bool initialDriveStraight = true;
bool isStarting = false;
int numRightTurns = 0;
// bool justTurnedDrive = false;

void coolDown() {
  stopMotors();
  isTransitioning = true;
  resetWallFollowingTimer();
  startWallFollowingTimer(WallFollowingState::TRANSITIONING);
}

void performWallFollowing() {
  if (!isWallFollowing && !isAutoAttacking) {
    logMessage("Wall following not active");
    return;
  }

  if (initialDriveStraight) {
    initialDriveStraight = false;
    isStarting = true;
    logMessage("Start by driving forward");
    startWallFollowingTimer(WallFollowingState::START);
    moveForward();
  }
  else if (isStarting) {
    logMessage("is starting");
    if (wall_following_interrupt_fired) {
      logMessage("Start complete, resume normal operation");
      isStarting = false;
      coolDown();
    }
  }
  else if (isTransitioning) {
    if (wall_following_interrupt_fired) {
      if (isCorrectingRight) {
        logMessage("Starting right correction");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::CORRECTING_RIGHT);
        turnRight();
        logMessage("Correcting right");
      } else if (isBigCorrectingRight) {
        logMessage("Transition complete, starting big right correction");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::TURNING_45);
        // if (left_sensor_distance <= LEFT_MAX_DISTANCE) {
          logMessage("BIG Correcting right");
          turnRight();
        // }
      } else if (isBigCorrectingLeft) {
        logMessage("Transition complete, starting big left correction");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::TURNING_45);
        if (left_sensor_distance > LEFT_MIN_DISTANCE) {
          turnLeft();
        }
      } else if (isCorrectingLeft) {
        logMessage("Transition complete, starting left correction");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::CORRECTING_LEFT);
        if (left_sensor_distance > LEFT_MIN_DISTANCE) {
          turnLeft();
        }
      } else if (isTurningRight) {
        logMessage("Transition complete, starting right turn");
        isTransitioning = false;
        startWallFollowingTimer(isAutoAttacking ? WallFollowingState::TURNING_45 : WallFollowingState::TURNING);
        logMessage("turning right");
        turnRight();
      } else if (isTurningLeft) {
        logMessage("Transition complete, starting left turn");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::TURNING);
        turnLeft();
      } else if (justTurnedLeftDriveForwardFlag) {
        logMessage("Transition complete, starting drive forward");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::FORWARD_AFTER_TURN);
        if (!(left_sensor_distance < LEFT_MIN_DISTANCE || psd_distance < (FRONT_STOP_DISTANCE * 1.1))) {
          moveForward();
        }
      } else if (justTurnedRightDriveForwardFlag) {
        logMessage("Transition complete, starting drive forward");
        isTransitioning = false;
        startWallFollowingTimer(WallFollowingState::FORWARD_AFTER_TURN);
        if (isAutoAttacking || !(psd_distance < (FRONT_STOP_DISTANCE * 1.1))) {
          moveForward();
        }
      }
      else if (isBackingUp) {
        logMessage("Transition complete, starting backup");
        isTransitioning = false;
        if (isAutoAttacking) {
          startWallFollowingTimer(WallFollowingState::BACKING_UP_ATTACK);
        } else {
          startWallFollowingTimer(WallFollowingState::BACKING_UP);
        }
        
        // if (!isAutoAttacking) {
          moveBackward();
        // }
      } else {
        stopMotors();
        isTransitioning = false;
        resetWallFollowingTimer();  // Keep this one as it's the final cleanup
      }
    }
  } else if (isTurningRight) {
    if (wall_following_interrupt_fired) {
      logMessage("Turning complete");
      isTurningRight = false;
      justTurnedRightDriveForwardFlag = true;
      coolDown();
    } else {
      // logMessage("turning right");
      // turnRight();
    }
  } else if (isBigCorrectingRight) {
    if (wall_following_interrupt_fired) {
      logMessage("Big right correction complete");
      isBigCorrectingRight = false;
      justTurnedRightDriveForwardFlag = true;
      coolDown();
    } else {
      logMessage("Big right correction...");
      // if (left_sensor_distance > LEFT_MAX_DISTANCE) {
        // stopMotors();
      // }
    }
  } else if (isBigCorrectingLeft) {
    if (wall_following_interrupt_fired) {
      logMessage("Big left correction complete");
      isBigCorrectingLeft = false;
      justTurnedLeftDriveForwardFlag = true;
      coolDown();
    } else {
      // logMessage("Big right correction...");
    //  if (left_sensor_distance > LEFT_MIN_DISTANCE) {
    //     turnLeft();
    //   } else {
    //     stopMotors();
    //   }
      if (left_sensor_distance < LEFT_MIN_DISTANCE) {
        stopMotors();
      }
    }
  } else if (isTurningLeft) {
    if (wall_following_interrupt_fired) {
      logMessage("Turning complete");
      isTurningLeft = false;
      justTurnedLeftDriveForwardFlag = true;
      coolDown();
    } else {
      // logMessage("Turning left...");
      // turnLeft();
    }
  } else if (isBackingUp) {
    if (wall_following_interrupt_fired) {
      logMessage("Backup complete");
      isBackingUp = false;
      if (numRightTurns == 2) {
        isBigCorrectingRight = true;
      } else {
        isTurningRight = true;
      }
      coolDown();
    } else {
      // moveBackward();
    }
  } else if (isCorrectingRight) {
    if (wall_following_interrupt_fired) {
      // logMessage("Right correction complete at " + String(millis()) + "ms");
      isCorrectingRight = false;
      justTurnedRightDriveForwardFlag = true;
      // justCorrectedFlag = true;
      coolDown();
    } else {
      // logMessage("Correcting right...");
      logMessage("correcting right");
      // turnRight();
    }
  } else if (isCorrectingLeft) {
    if (wall_following_interrupt_fired) {
      logMessage("Left correction complete");
      isCorrectingLeft = false;
      justTurnedLeftDriveForwardFlag = true;
      coolDown();
    } else {
      // logMessage("Correcting left...");
      // if (left_sensor_distance > LEFT_MIN_DISTANCE) {
      //   turnLeft();
      // } else {
      //   stopMotors();
      // }
      if (left_sensor_distance < LEFT_MIN_DISTANCE) {
        stopMotors();
      }
    }
  } else if (justTurnedLeftDriveForwardFlag) {
    if (wall_following_interrupt_fired) {
      justTurnedLeftDriveForwardFlag = false;
      coolDown();
    } else if (left_sensor_distance < LEFT_MIN_DISTANCE || psd_distance < (FRONT_STOP_DISTANCE * 1.1)) {
      stopMotors();
    }
  } else if (justTurnedRightDriveForwardFlag) {
    if (wall_following_interrupt_fired) {
      justTurnedRightDriveForwardFlag = false;
      coolDown();
    } else if (!isAutoAttacking && psd_distance < (FRONT_STOP_DISTANCE * 1.1)) {
      stopMotors();
    }
  } else if (left_sensor_distance > 200.0) {
    logMessage("Ultrasonic sensor seems too close to the wall, big turn right - Left: " + String(left_sensor_distance) + " Front: " + String(psd_distance));
    isBigCorrectingRight = true;
    coolDown();
  }  
  else if (left_sensor_distance < LEFT_MIN_DISTANCE && psd_distance > FRONT_STOP_DISTANCE) {
    logMessage("Wall too close - Left: " + String(left_sensor_distance) + " Front: " + String(psd_distance));
    isCorrectingRight = true;
    coolDown();
  } 
    // else if (left_sensor_distance > 18.0) {
    //   logMessage("No wall detected, turn left");
    //   stopMotors();
    //   isTurningLeft = true;
    //   isTransitioning = true;
    //   resetWfTimers();
    //   startWfTransitionTimer();
    // } 
  
  //  else if (left_sensor_distance >= LEFT_NO_WALL_DISTANCE) {
  //   logMessage("No wall detected, turn left");
  //   isTurningLeft = true;
  //   coolDown();
  // } 
  else if (psd_distance <= 20.0 && left_sensor_distance > 25.0) {
    logMessage("Big right baby");
    isBigCorrectingRight = true;
    coolDown();
  }
  else if (left_sensor_distance > 35.0 && psd_distance > FRONT_STOP_DISTANCE) {
    logMessage("Get closer to wall (big correction) - Left: " + String(left_sensor_distance) + " Front: " + String(psd_distance));
    isBigCorrectingLeft = true;
    coolDown();
  } 
  else if (left_sensor_distance > LEFT_MAX_DISTANCE && psd_distance > FRONT_STOP_DISTANCE) {
    logMessage("Get closer to wall (small correction) - Left: " + String(left_sensor_distance) + " Front: " + String(psd_distance));
    isCorrectingLeft = true;
    coolDown();
  } 
  
  else if (psd_distance <= FRONT_STOP_DISTANCE) {
    logMessage("Obstacle detected, backing up");
    isBackingUp = true;
    numRightTurns++;
    coolDown();
  } 
  else {
    moveForward();
  }
}