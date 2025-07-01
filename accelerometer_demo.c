#include "main.h"
#include <math.h>
#include <string.h>

#define ACCEL_I2C_ADDR (0x53 << 1)
#define REG_DATAX0 0x32
#define REG_POWER_CTL 0x2D

#define SCALE_FACTOR 0.004f
#define FALL_THRESHOLD 2.5f

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

void Accelerometer_Init(void) {
    uint8_t data = 0x08;
    HAL_I2C_Mem_Write(&hi2c1, ACCEL_I2C_ADDR, REG_POWER_CTL, 1, &data, 1, HAL_MAX_DELAY);
}

void Accelerometer_ReadRawData(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t buffer[6];
    HAL_I2C_Mem_Read(&hi2c1, ACCEL_I2C_ADDR, REG_DATAX0, 1, buffer, 6, HAL_MAX_DELAY);
    *x = (int16_t)((buffer[1] << 8) | buffer[0]);
    *y = (int16_t)((buffer[3] << 8) | buffer[2]);
    *z = (int16_t)((buffer[5] << 8) | buffer[4]);
}

float Calculate_Magnitude(int16_t x, int16_t y, int16_t z) {
    float xf = x * SCALE_FACTOR;
    float yf = y * SCALE_FACTOR;
    float zf = z * SCALE_FACTOR;
    return sqrtf(xf * xf + yf * yf + zf * zf);
}

void UART_Print(const char *msg) {
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    Accelerometer_Init();

    int16_t x, y, z;
    float magnitude;
    char msg[64];

    while (1) {
        Accelerometer_ReadRawData(&x, &y, &z);
        magnitude = Calculate_Magnitude(x, y, z);

        sprintf(msg, "Accel Magnitude: %.2f g\r\n", magnitude);
        UART_Print(msg);

        if (magnitude > FALL_THRESHOLD) {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
            UART_Print("Fall Detected!\r\n");
        } else {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        }

        HAL_Delay(200);
    }
}
