#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define CENTER_X (LCD_WIDTH / 2)
#define MAX_DISTANCE 100 // Max distance in cm for screen scaling

// Map angle (-90 to 90) to LCD x-coordinate (0 to 83)
int mapAngleToX(int angle) {
    return CENTER_X + (angle * LCD_WIDTH / 180);
}

// Map distance (0 to MAX_DISTANCE) to LCD y-coordinate (47 to 0)
int mapDistanceToY(uint16_t distance) {
    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
    return LCD_HEIGHT - 1 - (distance * (LCD_HEIGHT - 1) / MAX_DISTANCE);
}

// Function to dynamically plot angles and distances
void dynamicPlot(uint16_t *distances, int *angles, int count) {
    const uint16_t NO_OBJECT_DISTANCE = MAX_DISTANCE; // Treat MAX_DISTANCE as no object
    int objectDetected = 0;  // Flag to track if an object is currently being detected

    Nokia5110_ClearBuffer(); // Clear the LCD buffer

    for (int i = 0; i < count; i++) {
        int x = mapAngleToX(angles[i]);  // Map angle to x-coordinate
        int y;

        if (distances[i] <= MAX_DISTANCE) {
            // Object detected
            objectDetected = 1;  // Set object detected flag
            y = mapDistanceToY(distances[i]);  // Map distance to y-coordinate
        } else if (objectDetected) {
            // Transition from object to no object
            objectDetected = 0;  // Reset object detected flag
            y = mapDistanceToY(NO_OBJECT_DISTANCE);  // Plot max distance
        } else {
            // No object, plot max distance
            y = mapDistanceToY(NO_OBJECT_DISTANCE);
        }

        // Plot the point
        Nokia5110_SetPxl(y, x);
    }

    // Display the buffer
    Nokia5110_DisplayBuffer();
}