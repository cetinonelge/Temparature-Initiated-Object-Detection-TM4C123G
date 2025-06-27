#ifndef PUSHBUTTONS_H
#define PUSHBUTTONS_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "printHelper.h"
#include "lm35_control.h"
#include "LedSpeaker.h"
#include "Nokia5110.h"
#include <stdio.h>

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

// Declare TEMP_THRESHOLD as an external variable
extern volatile float TEMP_THRESHOLD;
extern volatile uint8_t isDeepSleepPressed;
// Function prototypes
void PushButtons_Init(void);
void GPIOE_Handler(void);  // ISR for push buttons

// Initialize Port E for push buttons (E1-E4) with interrupts
void PushButtons_Init(void) {
    SYSCTL->RCGCGPIO |= 0x10;         // Enable clock for Port E
    while ((SYSCTL->PRGPIO & 0x10) == 0);  // Wait for Port E to be ready

    GPIOE->DIR &= ~0x1E;         // Set E1-E4 as inputs (0b00011110)
    GPIOE->DEN |= 0x1E;          // Enable digital function for E1-E4

    GPIOE->IS &= ~0x1E;          // Edge-sensitive interrupts
    GPIOE->IBE &= ~0x1E;         // Interrupt controlled by IEV
    GPIOE->IEV &= ~0x1E;         // Falling edge triggers interrupt
    GPIOE->IM |= 0x1E;           // Unmask interrupts for E1-E4

		NVIC->ISER[0] |= (1 << 4);   // Enable interrupt for Port E
}

// GPIO Port E Interrupt Service Routine (ISR)
void GPIOE_Handler(void) {
    uint32_t status = GPIOE->MIS & 0x1E;  // Get masked interrupt status
    if (status & 0x02) {  // E1: Increase TEMP_THRESHOLD
        TEMP_THRESHOLD++;
				//lcd print
				char temp_thresh[50];
				Nokia5110_Clear();
				sprintf(temp_thresh, "Limit: %.0f", TEMP_THRESHOLD);
				Nokia5110_OutString(temp_thresh);
			
        printString("TEMP_THRESHOLD increased to: ");
        printFloat(TEMP_THRESHOLD, 1);
        printString(" C\r\n");
    }

    if (status & 0x04) {  // E2: Decrease TEMP_THRESHOLD
        if (TEMP_THRESHOLD > 0) TEMP_THRESHOLD--;
				//lcd print
				char temp_thresh[50];
				Nokia5110_Clear();
				sprintf(temp_thresh, "Limit: %.0f", TEMP_THRESHOLD);
				Nokia5110_OutString(temp_thresh);
			
        printString("TEMP_THRESHOLD decreased to: ");
        printFloat(TEMP_THRESHOLD, 1);
        printString(" C\r\n");
    }

    if (status & 0x10) {  // E4: Enter deep sleep
        TurnOffPowerLED();
				//EnterDeepSleep();
				isDeepSleepPressed = 1;
    }
		GPIOE->ICR |= 0x1E;  // Clear the interrupt
}

#endif  // PUSHBUTTONS_H
