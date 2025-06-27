#ifndef BMP280_H
#define BMP280_H

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "printHelper.h"

// BMP280 I2C Address
#define BMP280_I2C_ADDRESS 0x76

// BMP280 Registers
#define CTRL_MEAS 0xF4
#define CONFIG 0xF5
#define TEMP_MSB 0xFA
#define TEMP_LSB 0xFB
#define TEMP_XLSB 0xFC
#define CALIB_START 0x88

// Moving Average Filter Size
#define FILTER_SIZE 128

// Static variables for filtering
static float temperatureBuffer[FILTER_SIZE];
static uint8_t bufferIndex = 0;
static uint8_t readingsCount = 0;

// Calibration parameters
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;

// Function Prototypes
void I2C0_Init(void);
void BMP280_Init(void);
float BMP280_ReadTemperature(void);
float BMP280_FilteredTemperature(void);
void I2C_WriteRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data);
uint8_t I2C_ReadRegister(uint8_t deviceAddr, uint8_t regAddr);
void I2C0_Handler(void);

// I2C Initialization
void I2C0_Init(void) {
    SYSCTL_RCGCI2C_R |= 0x01;    // Enable I2C0 clock
    SYSCTL_RCGCGPIO_R |= 0x02;   // Enable clock for Port B
    while ((SYSCTL_PRGPIO_R & 0x02) == 0); // Wait for Port B to be ready

    GPIO_PORTB_AFSEL_R |= 0x0C;  // Enable alternate function for PB2 (SCL) and PB3 (SDA)
    GPIO_PORTB_ODR_R |= 0x08;    // Enable open-drain for PB3 (SDA)
    GPIO_PORTB_DEN_R |= 0x0C;    // Enable digital functionality for PB2 and PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300; // Configure PB2 and PB3 as I2C

    I2C0_MCR_R = 0x10;           // Enable I2C Master mode
    I2C0_MTPR_R = 24;            // Configure for 100 Kbps (assuming 16 MHz clock)

    NVIC_EN0_R |= (1 << 8);      // Enable I2C0 interrupt
}

// BMP280 Initialization
void BMP280_Init(void) {
    I2C0_Init();

    // Read calibration data
    dig_T1 = (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START) | (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START + 1) << 8));
    dig_T2 = (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START + 2) | (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START + 3) << 8));
    dig_T3 = (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START + 4) | (I2C_ReadRegister(BMP280_I2C_ADDRESS, CALIB_START + 5) << 8));

    // Configure CTRL_MEAS register
    I2C_WriteRegister(BMP280_I2C_ADDRESS, CTRL_MEAS, 0x27);  // Temperature oversampling x1, normal mode

    // Configure CONFIG register
    I2C_WriteRegister(BMP280_I2C_ADDRESS, CONFIG, 0xA0);     // Standby time 1000ms, filter coefficient 4
}

// BMP280 Temperature Reading
float BMP280_ReadTemperature(void) {
    uint8_t msb, lsb, xlsb;
    int32_t rawTemp;

    msb = I2C_ReadRegister(BMP280_I2C_ADDRESS, TEMP_MSB);
    lsb = I2C_ReadRegister(BMP280_I2C_ADDRESS, TEMP_LSB);
    xlsb = I2C_ReadRegister(BMP280_I2C_ADDRESS, TEMP_XLSB);

    rawTemp = ((msb << 12) | (lsb << 4) | (xlsb >> 4));

    // Temperature compensation formula from BMP280 datasheet
    int32_t var1, var2;
    var1 = ((((rawTemp >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((rawTemp >> 4) - ((int32_t)dig_T1)) * ((rawTemp >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    float temp = (t_fine * 5 + 128) >> 8;
    return temp / 100.0; // Temperature in Celsius
}

// Update Filtered Temperature using Interrupt
float BMP280_FilteredTemperature(void) {
    float sum = 0;
    float newTemp = BMP280_ReadTemperature();

    // Update the buffer with the new reading
    temperatureBuffer[bufferIndex] = newTemp;
    bufferIndex = (bufferIndex + 1) % FILTER_SIZE;

    // Ensure the buffer is filled before calculating the average
    if (readingsCount < FILTER_SIZE) {
        readingsCount++;
    }

    // Calculate the average
    for (uint8_t i = 0; i < readingsCount; i++) {
        sum += temperatureBuffer[i];
    }

    return sum / readingsCount;
}

// Write to BMP280 Register
void I2C_WriteRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
    I2C0_MSA_R = (deviceAddr << 1);  // Set slave address and write bit
    I2C0_MDR_R = regAddr;           // Set register address
    I2C0_MCS_R = 0x03;              // Start and send
    while (I2C0_MCS_R & 0x01);      // Wait for completion
    I2C0_MDR_R = data;              // Set data
    I2C0_MCS_R = 0x05;              // Stop and send
    while (I2C0_MCS_R & 0x01);      // Wait for completion
}

// Read from BMP280 Register
uint8_t I2C_ReadRegister(uint8_t deviceAddr, uint8_t regAddr) {
    I2C0_MSA_R = (deviceAddr << 1);  // Set slave address and write bit
    I2C0_MDR_R = regAddr;           // Set register address
    I2C0_MCS_R = 0x03;              // Start and send
    while (I2C0_MCS_R & 0x01);      // Wait for completion
    I2C0_MSA_R = (deviceAddr << 1) | 0x01; // Set slave address and read bit
    I2C0_MCS_R = 0x07;              // Restart and read
    while (I2C0_MCS_R & 0x01);      // Wait for completion
    return I2C0_MDR_R;
}

// I2C Interrupt Handler
void I2C0_Handler(void) {
    BMP280_FilteredTemperature();
    I2C0_MICR_R = 0x01; // Clear the interrupt
}

#endif // BMP280_H
