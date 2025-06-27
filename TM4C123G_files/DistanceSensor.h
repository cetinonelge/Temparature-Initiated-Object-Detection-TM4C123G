#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "TM4C123.h"

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

// Pins configuration for HC-SR04
#define TRIG_PIN 0x04  // PF2 for Trig
#define ECHO_PIN 0x10  // PC4 for Echo

// Global variables for pulse measurement
volatile uint32_t risingEdge = 0;
volatile uint32_t fallingEdge = 0;
volatile uint32_t pulseWidth = 0;
volatile uint32_t distance = 200; // Default distance (in cm)

// Function prototypes
void DistanceSensor_Init(void);
void TimerWT0_Init(void);
void TriggerPulse(void);
void WTIMER0A_Handler(void);

// Initialize the distance sensor (Trig: PF2, Echo: PC4)
void DistanceSensor_Init(void) {
    // Enable clock for Port F and Port C
    SYSCTL->RCGCGPIO |= 0x24;  // Enable clock for Port F and Port C
    while ((SYSCTL->PRGPIO & 0x24) == 0); // Wait for ports to be ready

    // Configure PF2 as output for Trig
    GPIOF->DIR |= TRIG_PIN;     // Set PF2 as output
    GPIOF->DEN |= TRIG_PIN;     // Enable digital function for PF2

    // Configure PC4 as input for Echo
    GPIOC->DIR &= ~ECHO_PIN;    // Set PC4 as input
    GPIOC->DEN |= ECHO_PIN;     // Enable digital function for PC4
    GPIOC->AFSEL |= ECHO_PIN;   // Enable alternate function on PC4
    GPIOC->PCTL = (GPIOC->PCTL & ~0x000F0000) | 0x00070000; // Configure PC4 as WT0CCP0
}

// Wide Timer 0 Initialization for Echo Pin (PC4)
void TimerWT0_Init(void) {
    SYSCTL->RCGCWTIMER |= 0x01;  // Enable clock for Wide Timer 0
    while ((SYSCTL->PRWTIMER & 0x01) == 0); // Wait for Wide Timer 0 to be ready

    WTIMER0->CTL &= ~0x01;       // Disable Timer 0A during configuration
    WTIMER0->CFG = 0x04;         // Configure for 32-bit timer mode
    WTIMER0->TAMR = 0x17;        // Capture mode, edge-time mode
    WTIMER0->CTL |= 0x0C;        // Configure for both edge detection (rising and falling)
    WTIMER0->TAILR = 0xFFFFFFFF; // Load the maximum 32-bit value
    WTIMER0->IMR |= 0x04;        // Enable capture event interrupt
    WTIMER0->ICR |= 0x04;        // Clear capture event flag
    WTIMER0->CTL |= 0x01;        // Enable Timer 0A

    //NVIC->ISER[0] |= (1 << 94);  // Enable IRQ 94 for Wide Timer 0A
		NVIC_EnableIRQ(WTIMER0A_IRQn);
}

// Trigger a pulse for HC-SR04
void TriggerPulse(void) {
    GPIOF->DATA &= ~TRIG_PIN; // Ensure Trig is low
    Timer5_DelayUs(2);        // Wait for 2 microseconds using Timer5
    GPIOF->DATA |= TRIG_PIN;  // Set Trig high
    Timer5_DelayUs(10);       // Send 10 microseconds pulse using Timer5
    GPIOF->DATA &= ~TRIG_PIN; // Set Trig low
}

// Wide Timer 0A ISR (handles Echo pin signal capture)
void WTIMER0A_Handler(void) {
    static int is_rising_edge = 1;

    if (is_rising_edge) {
        risingEdge = WTIMER0->TAR;  // Capture the rising edge
        is_rising_edge = 0;
    } else {
        fallingEdge = WTIMER0->TAR;  // Capture the falling edge

        if (fallingEdge >= risingEdge) {
            pulseWidth = fallingEdge - risingEdge;
        } else {
            pulseWidth = (0xFFFFFFFF - risingEdge) + fallingEdge;  // Handle Timer overflow
        }

        distance = pulseWidth * 0.001071875;  // Convert pulse width to distance in cm
        is_rising_edge = 1;
    }

    WTIMER0->ICR |= 0x04; // Clear capture event flag
}

#endif // DISTANCESENSOR_H
