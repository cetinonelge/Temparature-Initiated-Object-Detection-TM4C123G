#ifndef PRINT_HELPER_H
#define PRINT_HELPER_H

#include <stdint.h>

// Function to print a character via UART
void OutChar(char c);

// Function to print a string via UART
void printString(const char *str) {
    while (*str) {
        OutChar(*str);
        str++;
    }
}

// Function to print an integer (decimal) via UART
void printInt(uint32_t num) {
    if (num == 0) {
        OutChar('0');
        return;
    }

    char buffer[10];
    int i = 0;

    // Convert the integer to string in reverse order
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';  // Get the last digit
        num /= 10;  // Remove the last digit
    }

    // Print the string in correct order
    for (int j = i - 1; j >= 0; j--) {
        OutChar(buffer[j]);
    }
}

// Function to print a number (currently prints decimal) via UART
void printNumber(uint32_t num) {
    printInt(num);  // Reuse printInt to print the number in decimal
}

// Function to print a float via UART
void printFloat(float value, int decimalPlaces) {
    if (value < 0) {
        OutChar('-');    // Print negative sign for negative values
        value = -value;  // Work with the absolute value
    }

    // Print the integer part
    uint32_t intPart = (uint32_t)value;
    printInt(intPart);

    // Print the decimal point
    OutChar('.');

    // Calculate the fractional part
    float fractionalPart = value - (float)intPart;
    for (int i = 0; i < decimalPlaces; i++) {
        fractionalPart *= 10;
    }

    // Print the fractional part as an integer
    printInt((uint32_t)fractionalPart);
}

#endif  // PRINT_HELPER_H
