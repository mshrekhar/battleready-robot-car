#include <Arduino.h>
#include "motorcontrol.h"
#include "webserver.h"
#include "PsdSensor.h"
#include "interruptmanager.h"
#include "vivesensors.h"
#include "autodrive.h"
#include "tophat.h"
#include "psd.h"
#include "wallfollowing.h"
#include "ultrasonicsensors.h"
#include "logger.h"
#include "autoattack.h"

void setup() {
  Serial.begin(115200);
  setupMotorControl();
  initializeTophat();
  initViveSensors();
  initUltrasonicSensors();
  setupWebServer();
  initInterrupt();
  
  // Set initial destination
  // addWaypoint(4500, 3000);
  // addWaypoint(2500, 3000);
  // addWaypoint(2500, 3500);
  // addWaypoint(3500, 3500);
}

void loop() {
  if (interrupt_fired) {
    interrupt_fired = false;
    syncWithTophat();
  }

  updateVive();
  updatePsdSensor();
    
  // Get the latest distance (will process any received echo)
  getLeftSensorDistance();
  // Start a new ultrasonic reading
  startUltrasonicReading();

  if (current_health > 0) {
    if (isAutoMode) {
      drive();
    } else if (isWallFollowing) {
      performWallFollowing();
    } else if (isAutoAttacking) {
      performAutoAttack();
    }
    
    updateMotorControl();
  } else {
    stopMotors();
    // logMessage("Health depleted!");
  } 

  webServer.serve();
  // delay(10);  // Small delay is fine now since we're using interrupts
} 