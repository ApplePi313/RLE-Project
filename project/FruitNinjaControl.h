#ifndef FRUIT_NINJA_CONTROL_H
#define FRUIT_NINJA_CONTROL_H

#include <Mouse.h>
#include <Adafruit_CircuitPlayground.h>

// Constants
const float SENSITIVITY = 10.0;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const float GAME_WIDTH = 4.0;
const float GAME_HEIGHT = 3.0;
const int AUTO_CLICK_ON = LOW;
const unsigned long CALIBRATION_TIME = 5000;
const int LIGHT_DELAY = 500;
const int GREEN_DISPLAY = 2000;

// Function prototypes
void setupFruitNinjaControl();
void loopFruitNinjaControl();
void calibrateSensor();

#endif // FRUIT_NINJA_CONTROL_H
