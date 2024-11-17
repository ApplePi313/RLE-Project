#include "FruitNinjaControl.h"

// Variables
float baselineX = 0, baselineY = 0, baselineZ = 0;
bool autoClick = false;
float xScale = SCREEN_WIDTH / GAME_WIDTH;
float yScale = SCREEN_HEIGHT / GAME_HEIGHT;

// Setup function
void setupFruitNinjaControl() {
    CircuitPlayground.begin();
    Mouse.begin();
    pinMode(CPLAY_SLIDESWITCHPIN, INPUT);
    Serial.begin(9600);
}

// Main loop function
void loopFruitNinjaControl() {
    while (!(CircuitPlayground.leftButton() && CircuitPlayground.rightButton())) {
        // Toggle auto-click behavior based on slide switch
        autoClick = digitalRead(CPLAY_SLIDESWITCHPIN) == AUTO_CLICK_ON;

        // Get acceleration and adjust for tilt
        float accelX = CircuitPlayground.motionX();
        float accelY = CircuitPlayground.motionY();
        float accelZ = CircuitPlayground.motionZ();

        // Remove baseline offsets
        accelX -= baselineX;
        accelY -= baselineY;
        accelZ -= baselineZ;

        // Calculate plane-corrected movement
        float horizontalVelocity = accelX / accelZ;
        float verticalVelocity = accelY / accelZ;

        // Map physical motion to screen movement
        int mouseX = horizontalVelocity * xScale;
        int mouseY = verticalVelocity * yScale;

        // Move the mouse
        Mouse.move(mouseX, mouseY);

        // Handle click behavior
        if (CircuitPlayground.leftButton()) {
            if (autoClick) {
                Mouse.release(MOUSE_LEFT);
            } else {
                Mouse.press(MOUSE_LEFT);
            }
        } else if (autoClick) {
            Mouse.press(MOUSE_LEFT);
        } else {
            Mouse.release(MOUSE_LEFT);
        }

        delay(10);
    }

    // Light up circle during countdown
    for (int i = 0; i < 10; i++) {
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(25 * i));
        delay(LIGHT_DELAY);
    }

    if (!(CircuitPlayground.leftButton() && CircuitPlayground.rightButton())) {
        calibrateSensor();
    }
}

// Calibration Function
void calibrateSensor() {
    unsigned long startTime = millis();

    // Turn LEDs red during calibration
    CircuitPlayground.clearPixels();
    for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(0)); // Red
    }

    // Collect baseline readings
    float sumX = 0, sumY = 0, sumZ = 0;
    int readings = 0;

    while (millis() - startTime < CALIBRATION_TIME) {
        sumX += CircuitPlayground.motionX();
        sumY += CircuitPlayground.motionY();
        sumZ += CircuitPlayground.motionZ();
        readings++;
        delay(10);
    }

    baselineX = sumX / readings;
    baselineY = sumY / readings;
    baselineZ = sumZ / readings;

    // Indicate success with green light
    CircuitPlayground.clearPixels();
    for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(85)); // Green
    }
    delay(GREEN_DISPLAY);
    CircuitPlayground.clearPixels();
}