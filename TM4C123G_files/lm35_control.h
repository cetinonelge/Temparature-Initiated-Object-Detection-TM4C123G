#ifndef LM35_CONTROL_H
#define LM35_CONTROL_H

#include "tm4c123gh6pm.h"
#include "TM4C123.h"
#include <stdint.h>
#include "printHelper.h"
#include "Nokia5110.h"
#include <stdio.h>

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

// Function Prototypes
void AnalogComparator_Init(void);
void EnterDeepSleep(void);
void COMP0_Handler(void);

extern volatile float TEMP_THRESHOLD;

// Analog Comparator Initialization
void AnalogComparator_Init(void) {
    // Step 1: Enable the analog comparator clock
    SYSCTL_RCGCACMP_R |= 0x01;  // Enable clock for the Analog Comparator
    while ((SYSCTL_PRACMP_R & 0x01) == 0); // Wait for clock stabilization

    // Step 2: Enable clock to GPIOC for PC6 (C0+) and PC7 (C0-)
    SYSCTL_RCGCGPIO_R |= 0x04;  // Enable clock for GPIOC
    while ((SYSCTL_PRGPIO_R & 0x04) == 0); // Wait for GPIOC clock stabilization

    // Step 3: Configure PC6 and PC7 as GPIO inputs for analog signals
    GPIO_PORTC_DIR_R &= ~0xC0;  // Set PC6 and PC7 as inputs
    GPIO_PORTC_AFSEL_R |= 0xC0; // Enable alternate function on PC6 and PC7
    GPIO_PORTC_DEN_R &= ~0xC0;  // Disable digital functionality on PC6 and PC7
    GPIO_PORTC_AMSEL_R |= 0xC0; // Enable analog functionality on PC6 and PC7

    // Step 4: Configure the comparator to compare external inputs
    COMP_ACCTL0_R = 0x0000;        // Reset ACCTL0
    COMP_ACCTL0_R |= (0x1 << 9);   // Use external input PC6 for C0+ (ASRCP = 0x1)
    COMP_ACCTL0_R |= (0x2 << 2);   // Set ISEN to Rising Edge (ISEN = 0x2)

    // Step 5: Enable comparator interrupt
    COMP_ACINTEN_R |= 0x01;       // Enable interrupt for Comparator 0
		
		SYSCTL_DCGCACMP_R |= 0x01;  // Enable deep sleep clock for analog comparator

    // Step 6: Enable comparator interrupt in NVIC
    NVIC_EnableIRQ(COMP0_IRQn);   // Enable IRQ41 for Comparator 0
}

// Enter Deep Sleep Mode
void EnterDeepSleep(void) {
		//lcd print
		char temp_thresh[50];
		Nokia5110_Clear();
		sprintf(temp_thresh, "Limit: %.0f", TEMP_THRESHOLD);
		Nokia5110_OutString(temp_thresh);
		 
		printString("Entering Deep Sleep...\r\n");
		//COMP_ACMIS_R |= 0x01;      // Clear ACMIS flag for Comparator 0
		NVIC->ISER[0] &= ~(1 << 4);   // Disable interrupt for Port E
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   // Set SLEEPDEEP bit in the System Control Block
		NVIC_EnableIRQ(COMP0_IRQn);
    __asm("WFI");                        // Wait For Interrupt instruction
}

void COMP0_Handler(void) {
		
    printString("Comparator Interrupt: Waking up from Deep Sleep...\r\n");
		COMP_ACMIS_R |= 0x01;      // Clear ACMIS flag for Comparator 0
}


#endif // LM35_CONTROL_H
