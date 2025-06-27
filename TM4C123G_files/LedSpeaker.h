#ifndef LEDSPEAKER_H
#define LEDSPEAKER_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "TM4C123.h"

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

// Define GPIO pins for speaker and power LED
#define SPEAKER_PIN 0x01  // PB0 (bit 0)
#define POWER_LED_PIN 0x02  // PB1 (bit 1)

// Function prototypes
void LedSpeaker_Init(void);
void TurnOnPowerLED(void);
void TurnOffPowerLED(void);
void PlaySquareWave(uint32_t frequency, uint32_t duration);

// Initialize GPIO for power LED and speaker
void LedSpeaker_Init(void) {
    SYSCTL->RCGCGPIO |= 0x02;               // Enable clock for GPIOB
    while ((SYSCTL->PRGPIO & 0x02) == 0);   // Wait for Port B to be ready

    GPIOB->DIR |= (SPEAKER_PIN | POWER_LED_PIN);  // Set PB0 and PB1 as outputs
    GPIOB->DEN |= (SPEAKER_PIN | POWER_LED_PIN);  // Enable digital function for PB0 and PB1

    GPIOB->DATA &= ~(SPEAKER_PIN | POWER_LED_PIN);  // Turn off speaker and LED initially
}

// Turn ON power LED
void TurnOnPowerLED(void) {
    GPIOB->DATA |= POWER_LED_PIN;
}

// Turn OFF power LED
void TurnOffPowerLED(void) {
    GPIOB->DATA &= ~POWER_LED_PIN;
}

// Play a square wave on the speaker
void PlaySquareWave(uint32_t frequency, uint32_t duration) {
    uint32_t halfPeriod = 1000000 / (2 * frequency);  // Half period in microseconds

    for (uint32_t i = 0; i < (duration * 1000) / (2 * halfPeriod); i++) {
        GPIOB->DATA ^= SPEAKER_PIN;  // Toggle PB0
        Timer5_DelayUs(halfPeriod);  // Delay for half period
    }

    // Ensure the speaker is off after playing the sound
    GPIOB->DATA &= ~SPEAKER_PIN;
}

#endif  // LEDSPEAKER_H
