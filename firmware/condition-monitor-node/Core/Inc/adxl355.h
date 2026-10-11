#ifndef INC_ADXL355_H_
#define INC_ADXL355_H_

#include "main.h"

#define ADXL355_REG_PARTID   0x02
#define ADXL355_PARTID_VALUE 0xED

#define ADXL355_REG_FILTER     0x28
#define ADXL355_REG_RANGE      0x2C
#define ADXL355_REG_POWER_CTL  0x2D

#define ADXL355_FILTER_2KHZ    0x01

#define ADXL355_REG_XDATA3  0x08
#define ADXL355_REG_XDATA2  0x09
#define ADXL355_REG_XDATA1  0x0A
#define ADXL355_REG_YDATA3  0x0B
#define ADXL355_REG_YDATA2  0x0C
#define ADXL355_REG_YDATA1  0x0D
#define ADXL355_REG_ZDATA3  0x0E
#define ADXL355_REG_ZDATA2  0x0F
#define ADXL355_REG_ZDATA1  0x10

#define ADXL355_MAX_BURST_SIZE 9


HAL_StatusTypeDef ADXL355_ReadRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t *value);

HAL_StatusTypeDef ADXL355_ReadRegisters(
    SPI_HandleTypeDef *hspi,
    uint8_t start_reg,
    uint8_t *values,
    uint16_t n);

HAL_StatusTypeDef ADXL355_WriteRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t value);

HAL_StatusTypeDef ADXL355_Init(SPI_HandleTypeDef *hspi);

HAL_StatusTypeDef ADXL355_ReadX(SPI_HandleTypeDef *hspi, int32_t *x_raw);

HAL_StatusTypeDef ADXL355_ReadAcceleration(
    SPI_HandleTypeDef *hspi,
    int32_t *x_raw,
    int32_t *y_raw,
    int32_t *z_raw);


#endif
