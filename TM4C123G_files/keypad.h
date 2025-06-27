#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "timer_init.h"  // For Timer0_DelayMs

/*
 * Keypad_Read:
 * Reads the state of the 4x4 keypad buttons by setting each row (B4-B7) low
 * and checking the columns (B0-B3) for a pressed button.
 * Returns the button number (1-16) or 0 if no button is pressed.
 */
uint32_t Keypad_Read(void) {
    // Iterate over each row (B4-B7)
    for (int row = 0; row < 4; row++) {
        // Set all rows high
        GPIO_PORTB_DATA_R |= 0xF0;  // Ensure all rows (B4-B7) are high

        // Set the current row low
        GPIO_PORTB_DATA_R &= ~(1 << (4 + row));  // Set the selected row low (B4+B-row)

        // Wait for a short delay to debounce
        Timer5_DelayMs(5);

        // Read the columns (B0-B3)
        uint32_t colState = GPIO_PORTB_DATA_R & 0x0F;  // Mask to read columns only

        // Check if any column is low (button pressed)
        if (colState != 0x0F) {  // If any column is not high, a button is pressed
            for (int col = 0; col < 4; col++) {
                if ((colState & (1 << col)) == 0) {  // Check which column is low
                    // Calculate and return the button number (1-16)
                    return (row * 4) + (col + 1);
                }
            }
        }
    }

    return 0;  // No button pressed
}

/*
 * PushButtons_Read:
 * Reads the state of standalone push buttons connected to C4-C7.
 * Returns a value corresponding to the button number (17-20) or 0 if no button is pressed.
 */
uint32_t PushButtons_Read(void) {
    uint32_t buttonState = GPIO_PORTC_DATA_R & 0xF0;  // Read C4-C7
    switch (buttonState) {
        case 0xE0: return 17;  // Button 1
        case 0xD0: return 18;  // Button 2
        case 0xB0: return 19;  // Button 3
        case 0x70: return 20;  // Button 4
        default: return 0;     // No button pressed
    }
}

/*
 * Keypad_GetPressedButton:
 * Combines the Keypad_Read and PushButtons_Read functions to check both keypad and push buttons.
 * Returns the button number (1-20) if a valid button press is detected, or 0 otherwise.
 */
uint32_t Keypad_GetPressedButton(void) {
    uint32_t keypadButton = Keypad_Read();
    uint32_t pushButton = PushButtons_Read();

    // Check for a valid keypad button press
    if (keypadButton != 0) {
        // Debounce the keypad button
        Timer5_DelayMs(10);  // Additional debounce delay
        if (Keypad_Read() == keypadButton) {
            return keypadButton;
        }
    }

    // Check for a valid push button press
    if (pushButton != 0) {
        // Debounce the push button
        Timer5_DelayMs(10);  // Additional debounce delay
        if (PushButtons_Read() == pushButton) {
            return pushButton;
        }
    }

    return 0;  // No valid button press detected
}

#endif  // KEYPAD_H
