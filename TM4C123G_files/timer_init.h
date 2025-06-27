#ifndef TIMER_INIT_H
#define TIMER_INIT_H

#include "tm4c123gh6pm.h"
#include "TM4C123.h"
#include <stdint.h>

extern volatile uint8_t isDeepSleepPressed;

// ==================== Timer Initialization and Configuration Template ====================

// General-purpose timer initialization function templates are provided for all timers (Timer0-Timer5).
// Uncomment and customize the corresponding function for your specific project needs.

// ==================== General Notes ====================
// 1. **Timer Modes**:
//    The mode of operation is configured using GPTMTAMR and GPTMTBMR registers for Timer A and B.
//    - One-shot Mode: `GPTMTAMR = 0x01` - Timer counts down once and stops.
//    - Periodic Mode: `GPTMTAMR = 0x02` - Timer counts down repeatedly, reloading after each timeout.
//    - Capture Mode: `GPTMTAMR = 0x03` - Used for edge event detection on external input.
//    - PWM Mode: Requires special configuration to operate as a PWM generator.
//
// 2. **Configuration Registers**:
//    - GPTMCFG: Sets 32-bit, 16-bit, or RTC mode.
//    - GPTMTAILR/GPTMTBILR: Sets the initial load value for the timer.
//    - GPTMCTL: Configures advanced settings like edge detection or enabling timers.

// ==================== Register Details ====================
// 1. **GPTMCFG (General Purpose Timer Configuration Register)**:
//    - `0x0`: 32-bit mode (concatenated).
//    - `0x1`: 16-bit mode (independent A/B timers).
//    - `0x4`: RTC mode.
//
// 2. **GPTMTAMR/GPTMTBMR (Timer A/B Mode Registers)**:
//    - Bits 1:0 (TAMR/TBMR):
//      * `0x01` - One-shot mode.
//      * `0x02` - Periodic mode.
//      * `0x03` - Capture mode.
//    - Bit 3 (TACDIR/TBCDIR): Count direction (0 = Down, 1 = Up).
//    - Bit 4 (TAMIE/TBMIE): Match interrupt enable (1 = Enable).
//
// 3. **GPTMCTL (Timer Control Register)**:
//    - Configures timers, PWM, and edge detection.
//    - Bit 0 (TAEN): Enables Timer A.
//    - Bit 8 (TBEN): Enables Timer B.
//
// 4. **GPTMTAILR/GPTMTBILR (Timer A/B Interval Load Registers)**:
//    - Sets the start value for countdowns.
//    - For a system clock of 16 MHz, 1 ms delay = `16000` counts.

// ==================== SysTick ====================
// 1. **SysTick**:
//    - SysTick is a 24-bit system timer available on Cortex-M processors, used for periodic interrupts.
//    - Can be configured to generate periodic interrupts or as a simple timer.
//    - Registers:
//        * SYST_CSR: Control and status register.
//        * SYST_RVR: Reload value register (sets the countdown start value).
//        * SYST_CVR: Current value register (shows the current timer value).
//
// ==================== Timer Function Templates ====================

// ==================== Timer0 ====================
/*
void Timer0_Init(void) {
    SYSCTL_RCGCTIMER_R |= 1;          // Enable Timer0 clock
    TIMER0_CTL_R &= ~0x01;            // Disable Timer0
    TIMER0_CFG_R = 0x00000000;        // 32-bit mode
    TIMER0_TAMR_R = 0x02;             // Periodic mode
    TIMER0_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER0_CTL_R |= 0x01;             // Enable Timer0
}
*/

// ==================== Timer1 (Disabled by Default) ====================
/*
void Timer1_Init(void) {
    SYSCTL_RCGCTIMER_R |= 2;          // Enable Timer1 clock
    TIMER1_CTL_R &= ~0x01;            // Disable Timer1
    TIMER1_CFG_R = 0x00000000;        // 32-bit mode
    TIMER1_TAMR_R = 0x02;             // Periodic mode
    TIMER1_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER1_CTL_R |= 0x01;             // Enable Timer1
}
*/

// ==================== Timer2 (Disabled by Default) ====================
/*
void Timer2_Init(void) {
    SYSCTL_RCGCTIMER_R |= 4;          // Enable Timer2 clock
    TIMER2_CTL_R &= ~0x01;            // Disable Timer2
    TIMER2_CFG_R = 0x00000000;        // 32-bit mode
    TIMER2_TAMR_R = 0x02;             // Periodic mode
    TIMER2_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER2_CTL_R |= 0x01;             // Enable Timer2
}
*/

// ==================== Timer3 (Disabled by Default) ====================
/*
void Timer3_Init(void) {
    SYSCTL_RCGCTIMER_R |= 8;          // Enable Timer3 clock
    TIMER3_CTL_R &= ~0x01;            // Disable Timer3
    TIMER3_CFG_R = 0x00000000;        // 32-bit mode
    TIMER3_TAMR_R = 0x02;             // Periodic mode
    TIMER3_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER3_CTL_R |= 0x01;             // Enable Timer3
}
*/

// ==================== Timer4 (Disabled by Default) ====================
/*
void Timer4_Init(void) {
    SYSCTL_RCGCTIMER_R |= 16;         // Enable Timer4 clock
    TIMER4_CTL_R &= ~0x01;            // Disable Timer4
    TIMER4_CFG_R = 0x00000000;        // 32-bit mode
    TIMER4_TAMR_R = 0x02;             // Periodic mode
    TIMER4_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER4_CTL_R |= 0x01;             // Enable Timer4
}
*/

/*
// Initialize Timer4 for 1-second interrupts
void Timer4_Init(void) {
    SYSCTL->RCGCTIMER |= (1 << 4);      // Enable Timer4 clock
    while ((SYSCTL->PRTIMER & (1 << 4)) == 0);  // Wait for Timer4 to be ready

    TIMER4->CTL &= ~0x01;               // Disable Timer4 during configuration
    TIMER4->CFG = 0x00000000;           // Configure as 32-bit timer
    TIMER4->TAMR = 0x02;                // Periodic mode
    TIMER4->TAILR = 16000000 - 1;       // Load value for 1-second interval (assuming 16 MHz clock)
    TIMER4->IMR = 0x01;                 // Enable timeout interrupt
    NVIC_EnableIRQ(TIMER4A_IRQn);       // Enable Timer4A interrupt in NVIC
    TIMER4->CTL |= 0x01;                // Enable Timer4
}
*/
// ==================== Timer5 (Disabled by Default) ====================

void Timer5_Init(void) {
    SYSCTL_RCGCTIMER_R |= 32;         // Enable Timer5 clock
    TIMER5_CTL_R &= ~0x01;            // Disable Timer5
    TIMER5_CFG_R = 0x00000000;        // 32-bit mode
    TIMER5_TAMR_R = 0x02;             // Periodic mode
    TIMER5_TAILR_R = 0xFFFFFFFF;      // Set initial load value
    TIMER5_CTL_R |= 0x01;             // Enable Timer5
}


// Function to create a delay in milliseconds using Timer0
// Example delay function for creating a delay in milliseconds using Timer0.
// Assumes a 16 MHz system clock and uses a periodic timer for delay.
static void Timer5_DelayMs(uint32_t delay) {
    uint32_t ticks = delay * 16000; // Assuming 16 MHz system clock
    TIMER5_TAILR_R = ticks - 1;    // Load the calculated ticks into Timer0
    TIMER5_ICR_R = 0x01;           // Clear timeout flag
    TIMER5_CTL_R |= 0x01;          // Start Timer0

    // Poll the timeout flag to check if the timer has finished
    while ((TIMER5_RIS_R & 0x01) == 0){
			if(isDeepSleepPressed){
				break;
			}
		} // Wait for timeout flag to set

    TIMER5_ICR_R = 0x01; // Clear timeout flag again
}

static void Timer5_DelayUs(uint32_t delay) {
    uint32_t ticks = delay * 16; // Assuming 16 MHz system clock
    TIMER5_TAILR_R = ticks - 1;    // Load the calculated ticks into Timer0
    TIMER5_ICR_R = 0x01;           // Clear timeout flag
    TIMER5_CTL_R |= 0x01;          // Start Timer0

    // Poll the timeout flag to check if the timer has finished
    while ((TIMER5_RIS_R & 0x01) == 0){
			if(isDeepSleepPressed){
				break;
			}
		} // Wait for timeout flag to set

    TIMER5_ICR_R = 0x01; // Clear timeout flag again
}

// ==================== SysTick (Disabled by Default) ====================
/*
void SysTick_Init(uint32_t reloadValue) {
    // ==================== Configuration ====================
    // SysTick is a 24-bit timer used for periodic interrupts or delays.
    // Registers used:
    // 1. STCTRL (0xE000E010): Control and Status Register
    //    - Bit 0 (ENABLE): Enable the SysTick timer.
    //    - Bit 1 (INTEN): Enable SysTick interrupt.
    //    - Bit 2 (CLK_SRC): Clock source (0 = External, 1 = System clock).
    // 2. STRELOAD (0xE000E014): Reload Value Register
    //    - Sets the starting value for the countdown.
    // 3. STCURRENT (0xE000E018): Current Value Register
    //    - Shows the current countdown value (write clears this register).
    //
    // ==================== Parameters ====================
    // reloadValue: Sets the starting countdown value. 
    // Example: For a 1ms period with a 16 MHz system clock, use `16000 - 1`.
    //
    // ==================== Steps ====================
    NVIC_ST_CTRL_R = 0;               // Disable SysTick during setup
    NVIC_ST_RELOAD_R = reloadValue - 1;   // Set reload value
    NVIC_ST_CURRENT_R = 0;            // Clear current value register
    NVIC_ST_CTRL_R = 0x07;            // Enable SysTick with system clock and interrupt
}
*/
// SysTick Interrupt Handler (Template)
// Add your custom functionality here. This function is called on every SysTick interrupt.
/*
void SysTick_Handler(void) {
    // Custom periodic task code
}
*/

#endif // TIMER_INIT_H
