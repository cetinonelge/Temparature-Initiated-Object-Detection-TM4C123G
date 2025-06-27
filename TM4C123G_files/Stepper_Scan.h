#ifndef STEPPER_SCAN_H
#define STEPPER_SCAN_H

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "DistanceSensor.h"
#include "Nokia5110.h"
#include <stdio.h>
#include "printHelper.h"
#include "plot.h"

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

extern volatile uint32_t distance;

#define MAX_STEPS 180

// Arrays to store distances and angles
uint16_t distanceArray[MAX_STEPS];
int angleArray[MAX_STEPS];
char buffer[50];

void RGB_Init(void);
void Set_RGB_Color(uint8_t red, uint8_t green, uint8_t blue);

// Initialize PD0-PD3 for stepper motor control
void Stepper_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x08;  // Enable clock for Port D
    while ((SYSCTL_PRGPIO_R & 0x08) == 0); // Wait for Port D to be ready

    GPIO_PORTD_DIR_R |= 0x0F;  // Set PD0-PD3 as outputs
    GPIO_PORTD_DEN_R |= 0x0F;  // Enable digital function for PD0-PD3
    GPIO_PORTD_DATA_R &= ~0x0F; // Clear PD0-PD3 (initial state)
}

// Full-step sequence for stepper motor (PD0-PD3 control IN1, IN2, IN3, IN4)
const uint8_t stepSequence[4] = {0x01, 0x02, 0x04, 0x08}; // Step sequence for motor control

// Function to scan from -90 to 90 degrees once
void StepperMotor_Scan(void) {
    int stepsPerDegree = 4096 / 360;  // Steps per degree for a full 360-degree rotation
    int totalSteps = ((180 * stepsPerDegree) * 8 / 14);  // Total steps for 180 degrees (-90 to 90)
    int currentStep = 0;

    // Reset object detection variables
    uint16_t initialDistance = 0xFFFF;  // Initial value to filter
    int objectStartAngle = -1;
    int objectEndAngle = -1;
    uint16_t objectDistance = 0xFFFF;  // Reset to large value
    int objectDetected = 0;

    // Clear previous data
    for (int i = 0; i < MAX_STEPS; i++) {
        distanceArray[i] = 0xFFFF;  // Initialize with a large value
        angleArray[i] = -90 + (i * 180 / MAX_STEPS);  // Map angles from -90 to 90
    }

    // Perform the scan
    for (int step = 0; step < totalSteps; step++) {
        // Rotate motor one step
        uint8_t currentStepValue = stepSequence[currentStep % 4];
        GPIOD->DATA = (GPIOD->DATA & 0xF0) | currentStepValue;
        currentStep++;
        Timer5_DelayUs(2000); // Adjust delay for smooth operation

        // Trigger distance measurement
        TriggerPulse();
        Timer5_DelayMs(15);  // Allow time for echo response

        // Store distance at corresponding angle
        int index = step * MAX_STEPS / totalSteps;  // Map step to array index
        if (index < MAX_STEPS) {
            distanceArray[index] = distance;
        }

        // Initialize initialDistance during the first few steps
        if (step < 10) {
            initialDistance = distance;
            continue;
        }

        // Detect start of object
        if (!objectDetected && distance < (initialDistance - 50)) {  // Threshold for detecting an object
            objectStartAngle = angleArray[index];
            objectDistance = distance;
            objectDetected = 1;  // Object detection starts
        }

        // Detect end of object (continue scanning even after detecting an object)
        if (objectDetected && distance > (initialDistance - 30)) {  // Threshold for losing object
            objectEndAngle = angleArray[index];
        }
    }

    // Plot the data
    dynamicPlot(distanceArray, angleArray, MAX_STEPS);
    Timer5_DelayMs(5000);

    // If an object was detected, calculate its average angle
    int averageAngle = (objectDetected) ? (objectStartAngle + objectEndAngle) / 2 : -1;

    // Display results and control LEDs
    if (objectDetected) {
        if (objectDistance >= 75 && objectDistance < 100) {
            Set_RGB_Color(0, 1, 0); // Green LED ON
            sprintf(buffer, "Avg Angle: %d\nDist: %u cm", averageAngle, objectDistance);
        } else if (objectDistance >= 50 && objectDistance < 75) {
            Set_RGB_Color(0, 0, 1); // Blue LED ON
            sprintf(buffer, "Avg Angle: %d\nDist: %u cm", averageAngle, objectDistance);
        } else if (objectDistance < 50) {
            Set_RGB_Color(1, 0, 0); // Red LED ON
            sprintf(buffer, "Avg Angle: %d\nDist: %u cm", averageAngle, objectDistance);
        }	else {
						Set_RGB_Color(0, 0, 0); // Turn off all LEDs
            sprintf(buffer, "NO OBJECT");
				}
    } else {
        Set_RGB_Color(0, 0, 0); // Turn off all LEDs
        sprintf(buffer, "NO OBJECT");
    }

    // Display on LCD
    Nokia5110_Clear();
    Nokia5110_OutString(buffer);
}




// Initialize RGB LEDs
void RGB_Init(void) {
    SYSCTL->RCGCGPIO |= 0x20;  // Enable clock for Port F
    while ((SYSCTL->PRGPIO & 0x20) == 0); // Wait for Port F to be ready

    GPIOF->LOCK = 0x4C4F434B;  // Unlock GPIO Port F
    GPIOF->CR |= 0x0E;         // Allow changes to PF1, PF2, PF3
    GPIOF->LOCK = 0;           // Re-lock GPIO Port F

    GPIOF->DIR |= 0x0E;        // Set PF1, PF2, PF3 as outputs (RGB LEDs)
    GPIOF->DEN |= 0x0E;        // Enable digital function for PF1, PF2, PF3
}

// Set RGB LED color
void Set_RGB_Color(uint8_t red, uint8_t green, uint8_t blue) {
    GPIOF->DATA &= ~0x0E; // Turn off all RGB LEDs
    if (red) GPIOF->DATA |= 0x02;   // Turn on Red LED (PF1)
    if (green) GPIOF->DATA |= 0x08; // Turn on Green LED (PF3)
    if (blue) GPIOF->DATA |= 0x04;  // Turn on Blue LED (PF2)
}

#endif // STEPPER_SCAN_H
