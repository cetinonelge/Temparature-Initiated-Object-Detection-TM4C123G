#include "printHelper.h"
#include "lm35_control.h"
#include "bmp280.h"
#include "Stepper_Scan.h"
#include "LedSpeaker.h"
#include "PushButtons.h"  // Include push buttons header
#include "DistanceSensor.h"
#include "Nokia5110.h"
#include <stdint.h>
#include <stdio.h>

// Define temperature threshold as a global variable
volatile float TEMP_THRESHOLD = 20.0;  // Threshold temperature in Celsius
volatile uint8_t isDeepSleepPressed = 0;
extern volatile uint32_t distance;

extern void Timer5_Init(void);
extern void Timer5_DelayMs(uint32_t ms);
extern void Timer5_DelayUs(uint32_t us);

// Function prototypes
uint8_t CheckComparatorOutput(void);
void StandbyMode(void);  // Standby mode to update temperature every 2 seconds

int main(void) {
    // Initialization
		Nokia5110_Init();
    Timer5_Init();             // Initialize Timer5 for delays
		TimerWT0_Init();         	// Initialize Timer 1A
		RGB_Init();
    BMP280_Init();             // Initialize BMP280 sensor
    Stepper_Init();            // Initialize Stepper Motor
    LedSpeaker_Init();         // Initialize power LED and speaker
    PushButtons_Init();        // Initialize push buttons with interrupts
		DistanceSensor_Init();  		// Initialize the distance sensor
		AnalogComparator_Init();   // Initialize the Analog Comparator

    // Enter deep sleep mode
    TurnOffPowerLED();
    printString("System Initialized. Entering Deep Sleep...\r\n");

    while (1) {
				EnterDeepSleep();
				isDeepSleepPressed = 0;
				NVIC_DisableIRQ(COMP0_IRQn);
				NVIC->ISER[0] |= (1 << 4);   // Enable interrupt for Port E
        // Check if comparator output indicates temperature increase
					TurnOnPowerLED();
					printString("Temperature threshold exceeded! Waking up...\r\n");

					// Perform one full scan (128 samples)
					float sum = 0;
					for (uint8_t i = 0; i < FILTER_SIZE; i++) {
							float temp = BMP280_ReadTemperature();
							sum += temp;
							Timer5_DelayMs(10);  // Small delay between samples
					}

					float averageTemperature = sum / FILTER_SIZE;
					
					//lcd print
					char temperature[50];
					Nokia5110_Clear();
					sprintf(temperature, "Temp: %.2f", averageTemperature);
					Nokia5110_OutString(temperature);
					
					// Print the average temperature
					printString("Average Temperature: ");
					printFloat(averageTemperature, 2);
					printString(" C\r\n");

					// Check if the average temperature is above the threshold
					if (averageTemperature > TEMP_THRESHOLD) {
							printString("ALERT: Temperature is above the threshold!\r\n");

							// Play square wave for 3 seconds
							printString("Playing alert sound...\r\n");
							PlaySquareWave(500 * (averageTemperature / 10), 3000);

							printString("Starting Stepper Motor Scan...\r\n");
							StepperMotor_Scan();  // Perform stepper motor scan
							Timer5_DelayMs(5000);
					} else {
							printString("Temperature is within safe limits.\r\n");
					}

					// After the scan, enter standby mode
					printString("Scan complete. Entering Standby Mode...\r\n");
					StandbyMode();  // Enter standby mode
    }

    return 0;
}

// Function to check comparator output
uint8_t CheckComparatorOutput(void) {
		printString("error");
    return (COMP_ACSTAT0_R & 0x02);  // Returns 1 if C0+ > C0-
}

// Standby mode: Update and print temperature every 2 seconds
void StandbyMode(void) {
    while (!isDeepSleepPressed) {
        // Perform one full scan (128 samples)
            /*
						float sum = 0;
            for (uint8_t i = 0; i < FILTER_SIZE; i++) {
                float temp = BMP280_ReadTemperature();
                sum += temp;
                Timer5_DelayMs(10);  // Small delay between samples
            }

            float averageTemperature = sum / FILTER_SIZE;
						*/
						
						//lcd print
						char temperature[50];
						Nokia5110_Clear();
						sprintf(temperature, "Temp: %.2f", BMP280_ReadTemperature());
						Nokia5110_OutString(temperature);

            // Print the average temperature
            printString("Average Temperature: ");
            printFloat(BMP280_ReadTemperature(), 2);
            printString(" C\r\n");

        // Delay for 2 seconds
        Timer5_DelayMs(1000);
    }
}
