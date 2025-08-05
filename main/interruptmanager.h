#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H

#include <Arduino.h>
#include <esp_timer.h>

// Wall following states
enum class WallFollowingState {
    IDLE,
    START,
    TRANSITIONING,
    TURNING,
    TURNING_ATTACK,
    TURNING_ATTACK_2,
    TURNING_45,
    CORRECTING_RIGHT,
    CORRECTING_LEFT,
    BACKING_UP,
    BACKING_UP_ATTACK,
    FORWARD_AFTER_TURN,
    FORWARD_ATTACK_1,
    FORWARD_ATTACK_2,
    TURNING_180,
};

// General interrupt timer
extern hw_timer_t* interrupt_timer;
extern volatile bool interrupt_fired;

// Wall following timer
extern hw_timer_t* wall_following_timer;
extern volatile bool wall_following_interrupt_fired;
extern volatile WallFollowingState current_state;

// Timer frequencies
#define WALL_FOLLOWING_TIMER_FREQ 10000  // 10kHz for wall following
#define GENERAL_TIMER_FREQ 100000        // 100kHz for general interrupt

// Function declarations
void initInterrupt();
void startWallFollowingTimer(WallFollowingState state);
void stopWallFollowingTimer();
void resetWallFollowingTimer();

#endif 