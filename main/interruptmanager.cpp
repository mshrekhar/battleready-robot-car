#include "interruptmanager.h"
#include "logger.h"

// General interrupt timer
hw_timer_t* interrupt_timer = nullptr;
volatile bool interrupt_fired = false;

// Wall following timer
hw_timer_t* wall_following_timer = nullptr;
volatile bool wall_following_interrupt_fired = false;
volatile WallFollowingState current_state = WallFollowingState::IDLE;

void IRAM_ATTR onInterrupt() {
    interrupt_fired = true;
}

void IRAM_ATTR onWallFollowingInterrupt() {
    wall_following_interrupt_fired = true;
    // logMessage("Wall following interrupt fired at " + String(millis()) + "ms");
}

void initInterrupt() {
    // Initialize the general interrupt timer (fires 2 times per second)
    interrupt_timer = timerBegin(GENERAL_TIMER_FREQ);
    timerAttachInterrupt(interrupt_timer, &onInterrupt);
    timerAlarm(interrupt_timer, 50000, true, 0);
}

void startWallFollowingTimer(WallFollowingState state) {
    // Clean up existing timer if it exists
    if (wall_following_timer != nullptr) {
        timerStop(wall_following_timer);
        timerDetachInterrupt(wall_following_timer);
        timerEnd(wall_following_timer);
        wall_following_timer = nullptr;
    }

    wall_following_interrupt_fired = false;

    wall_following_timer = timerBegin(WALL_FOLLOWING_TIMER_FREQ);
    timerAttachInterrupt(wall_following_timer, &onWallFollowingInterrupt);
    
    // Set new state and reset interrupt flag
    current_state = state;
    
    // Map state to duration
    uint32_t duration_ms = 0;
    switch (state) {
        case WallFollowingState::TRANSITIONING:
            duration_ms = 200;  // 300ms for transition
            break;
        case WallFollowingState::TURNING:
            duration_ms = 1600; // 1000ms (1s) for full turn
            break;
        case WallFollowingState::TURNING_ATTACK:
            duration_ms = 1600; // 1000ms (1s) for full turn
            break;
        case WallFollowingState::TURNING_ATTACK_2:
            duration_ms = 3000; // 1000ms (1s) for full turn
            break;
        case WallFollowingState::TURNING_45:
            duration_ms = 800;  // 500ms for 45-degree turn
            break;
        case WallFollowingState::CORRECTING_RIGHT:
            duration_ms = 270;  // 250ms for right correction
            break;
        case WallFollowingState::CORRECTING_LEFT:
            duration_ms = 250;  // 210ms for left correction
            break;
        case WallFollowingState::BACKING_UP:
            duration_ms = 650;  // 300ms for backup
            break;
        case WallFollowingState::BACKING_UP_ATTACK:
            duration_ms = 200;  // 300ms for backup
            break;
        case WallFollowingState::FORWARD_AFTER_TURN:
            duration_ms = 400;  // 300ms for forward after turn
            break;
        case WallFollowingState::START:
            duration_ms = 2000;
            break;
        case WallFollowingState::FORWARD_ATTACK_1:
            duration_ms = 500;
            break;
        case WallFollowingState::FORWARD_ATTACK_2:
            duration_ms = 1000;
            break;
         case WallFollowingState::TURNING_180:
            duration_ms = 3200; // 1000ms (1s) for full turn
            break;
        default:
            duration_ms = 0;   // No duration for IDLE state
            break;
    }
    
    // Calculate timer value
    // WALL_FOLLOWING_TIMER_FREQ is 10kHz (10,000 Hz)
    // So each tick is 0.1ms (100 microseconds)
    // To get the number of ticks for a given duration in ms, multiply by 10
    uint64_t timer_value = duration_ms * 10;
    // logMessage("Timer value: " + String(timer_value));
    
    // Start timer if duration is non-zero
    if (timer_value > 0) {
        // logMessage("Starting timer at " + String(millis()) + "ms");
        timerAlarm(wall_following_timer, timer_value, false, 0);
    }
}

void stopWallFollowingTimer() {
    if (wall_following_timer != nullptr) {
        timerStop(wall_following_timer);
        timerDetachInterrupt(wall_following_timer);
        timerEnd(wall_following_timer);
        wall_following_timer = nullptr;
    }
    current_state = WallFollowingState::IDLE;
    wall_following_interrupt_fired = false;
}

void resetWallFollowingTimer() {
    stopWallFollowingTimer();
}