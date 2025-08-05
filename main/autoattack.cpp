#include "autoAttack.h"
#include "wallfollowing.h"
#include "vivesensors.h"
#include "motorcontrol.h"
#include "logger.h"
#include "interruptmanager.h"
#include "autodrive.h"

bool isAutoAttacking = false;
int numPointsHit = 0;
bool isTurning = false;
bool hasTurned = false;
bool isDrivingStraight = false;
bool isPaused = false;
bool goingBack = false;
bool goneBack = false; 
bool addedWaypoint = false;
bool shouldAddWaypoint = false;

void performAutoAttack() {

  if (!isAutoAttacking) {
    return;
  }

  if (numPointsHit == 0) {
    if (abs(front_x - 2800) <= 150) {
      stopMotors();
      logMessage ("Made it to point 1");
      numPointsHit++;
      resetVive = true;
      checkForFlipping = true;
    } else {
      performWallFollowing();
    }
  } else if (numPointsHit < 3) {
    if (hasTurned && !isDrivingStraight) {
      if (isPaused) {
        if (wall_following_interrupt_fired) {
          isPaused = false;
          isDrivingStraight = true;
          if (numPointsHit == 1) {
            startWallFollowingTimer(WallFollowingState::FORWARD_ATTACK_1);
          } else {
            startWallFollowingTimer(WallFollowingState::FORWARD_ATTACK_2);
          }
          
          moveForward();
        }
      } else {
        isPaused = true;
        stopMotors();
        startWallFollowingTimer(WallFollowingState::TRANSITIONING);
      }
    } else if (hasTurned && isDrivingStraight) {
      if (wall_following_interrupt_fired) {
        isDrivingStraight = false;
        stopMotors();
        numPointsHit++;
        logMessage ("Made it to point " + String(numPointsHit));
        hasTurned = false;
        isTurning = false;
      }
    } else if (isTurning) {
      if (wall_following_interrupt_fired) {
        isTurning = false;
        hasTurned = true;
      }
    } else {
      if (isPaused) {
        if (wall_following_interrupt_fired) {
          isPaused = false;
          isTurning = true;
          startWallFollowingTimer(WallFollowingState::TURNING_ATTACK);
          turnRight();
        }
      } else {
        startWallFollowingTimer(WallFollowingState::TRANSITIONING);
        // isTurning = true; 
        isPaused = true;
        stopMotors();
      }
    }
  } else if (numPointsHit == 3) {
    if (hasTurned) {
      performWallFollowing();
      // if (addedWaypoint) {
      //   if (reachedTarget) {
      //     numPointsHit++;
      //     logMessage("Made it to point " + String(numPointsHit));
      //     goneBack = false;
      //     addedWaypoint = false;
      //     hasTurned = false;
      //   } else {
      //     drive();
      //   }
      // } else if (shouldAddWaypoint) {
      //   addWaypoint(2060, 3480);
      //   setNextWaypoint();
      //   logMessage("Added waypoint");
      //   shouldAddWaypoint = false;
      //   addedWaypoint = true;
      // } else if (isPaused) {
      //   if (wall_following_interrupt_fired) {
      //     logMessage("unpausing");
      //     isPaused = false;
      //     shouldAddWaypoint = true;
      //   }
      // } else {
      //   logMessage("pause before adding waypoint");
      //   isPaused = true;
      //   stopMotors();
      //   startWallFollowingTimer(WallFollowingState::TRANSITIONING);
      // }
    } else if (isTurning) {
      if (wall_following_interrupt_fired) {
        isTurning = false;
        hasTurned = true;
        stopMotors();
        logMessage("Turned 180");
      }
    } else if (isPaused && !hasTurned && goneBack) {
      if (wall_following_interrupt_fired) {
        isPaused = false;
        isTurning = true;
        logMessage("turning");
        startWallFollowingTimer(WallFollowingState::TURNING_ATTACK_2);
        turnRight();
      }
    } else if (goneBack) {
      logMessage("pause before turning");
      startWallFollowingTimer(WallFollowingState::TRANSITIONING);
        isPaused = true;
        stopMotors();
    } else if (goingBack) {
      if (wall_following_interrupt_fired) {
        goingBack = false;
        goneBack = true;
        stopMotors();
        logMessage("Backed up");
      }
    } else {
      if (isPaused) {
        if (wall_following_interrupt_fired) {
          isPaused = false;
          goingBack = true;
          logMessage("backing up");
          startWallFollowingTimer(WallFollowingState::BACKING_UP_ATTACK);
          moveBackward();
        }
      } else {
        logMessage("pause before backing up");
        startWallFollowingTimer(WallFollowingState::TRANSITIONING);
        isPaused = true;
        stopMotors();
      }
    }
  } 
  else {
    logMessage("Auto attacking complete!");
    stopMotors();
    isAutoAttacking = false;
    isWallFollowing = false;
    checkForFlipping = false;
  }

  

}
