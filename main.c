/* USER CODE BEGIN Header */
/**
  **************************
  * @file           : main.c
  * @brief          : Main program body
  **************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>   // For sprintf
#include <math.h>    // For sqrt
#include <string.h>  // For strlen
#include <stdbool.h> // For bool, true, false

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FALL_THRESHOLD_G 2.5  // Acceleration threshold in G units
#define FALL_DURATION_MS 200 // Minimum duration in milliseconds
#define G_CONVERSION 9.8    // Conversion factor for G to m/s^2
#define ADXL345_I2C_ADDR 0x53 // I2C Address
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t fallStartTime = 0;  // Tracks duration of fall
bool fallInProgress = false; // Flag for ongoing fall
char debugBuffer[100];       // UART debug message buffer
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
void initADXL345(void);
void readAccelerometer(float *x, float *y, float *z);
bool detectFall(float x, float y, float z);
void outputFallSignal(bool detected);
void printToConsole(const char *message);
void Error_Handler(void); // Define the missing Error_Handler function

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  initADXL345(); // Initialize accelerometer

  while (1)
  {
    float x = 0, y = 0, z = 0;

    // Read accelerometer data
    readAccelerometer(&x, &y, &z);

    // Detect falls
    bool fallDetected = detectFall(x, y, z);

    // Output result
    outputFallSignal(fallDetected);

    // Print accelerometer data to UART
    sprintf(debugBuffer, "Accel Data - X: %.2f m/s^2, Y: %.2f m/s^2, Z: %.2f m/s^2\r\n", x, y, z);
    printToConsole(debugBuffer);

    // Print fall detection status
    if (fallDetected) {
        printToConsole("Fall detected! Threshold exceeded.\r\n");
    }

    HAL_Delay(500); // Delay for stability
  }
}

/* USER CODE BEGIN 4 */

// Initialize the ADXL345 accelerometer
void initADXL345(void) {
    uint8_t data[2];

    // Set power control to measurement mode
    data[0] = ADXL345_POWER_CTL;
    data[1] = 0x08; // Measurement mode
    HAL_I2C_Master_Transmit(&hi2c1, ADXL345_I2C_ADDR << 1, data, 2, HAL_MAX_DELAY);

    // Set data format to full resolution, +/- 16G
    data[0] = ADXL345_DATA_FORMAT;
    data[1] = 0x0B; // Full resolution, +/- 16G
    HAL_I2C_Master_Transmit(&hi2c1, ADXL345_I2C_ADDR << 1, data, 2, HAL_MAX_DELAY);
}

// Read accelerometer data (X, Y, Z)
void readAccelerometer(float *x, float *y, float *z) {
    uint8_t buffer[6];
    int16_t rawX, rawY, rawZ;

    // Read 6 bytes starting at DATAX0
    HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_ADDR << 1, ADXL345_DATAX0, 1, buffer, 6, HAL_MAX_DELAY);

    // Combine bytes to get signed 16-bit values
    rawX = (int16_t)((buffer[1] << 8) | buffer[0]);
    rawY = (int16_t)((buffer[3] << 8) | buffer[2]);
    rawZ = (int16_t)((buffer[5] << 8) | buffer[4]);

    // Convert to G-units (assuming 4 mg/LSB)
    *x = rawX * 0.004 * G_CONVERSION;
    *y = rawY * 0.004 * G_CONVERSION;
    *z = rawZ * 0.004 * G_CONVERSION;
}

// Detect if a fall occurred based on acceleration data
bool detectFall(float x, float y, float z) {
    float magnitude = sqrt(x * x + y * y + z * z);

    if (magnitude > FALL_THRESHOLD_G * G_CONVERSION) {
        if (!fallInProgress) {
            fallInProgress = true;
            fallStartTime = HAL_GetTick(); // Start timing
        } else if ((HAL_GetTick() - fallStartTime) > FALL_DURATION_MS) {
            fallInProgress = false;
            return true; // Fall detected
        }
    } else {
        fallInProgress = false; // Reset if threshold not maintained
    }

    return false; // No fall detected
}

// Set GPIO output to signal a fall
void outputFallSignal(bool detected) {
    if (detected) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); // Set PC7 HIGH
    } else {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // Set PC7 LOW
    }
}

// Print message to UART console
void printToConsole(const char *message) {
    HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

// Error handler implementation
void Error_Handler(void) {
    // Infinite loop to indicate an error
    while (1) {
    }
}

/* USER CODE END 4 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    // System clock configuration code (auto-generated)
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC7 */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}