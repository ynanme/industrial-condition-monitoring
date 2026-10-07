#ifndef INC_ADXL355_H_
#define INC_ADXL355_H_

#include "main.h"

#define ADXL355_REG_PARTID   0x02
#define ADXL355_PARTID_VALUE 0xED

#define ADXL355_REG_FILTER     0x28
#define ADXL355_REG_RANGE      0x2C
#define ADXL355_REG_POWER_CTL  0x2D

#define ADXL355_FILTER_2KHZ    0x01


HAL_StatusTypeDef ADXL355_ReadRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t *value);

HAL_StatusTypeDef ADXL355_WriteRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t value);

HAL_StatusTypeDef ADXL355_Init(SPI_HandleTypeDef *hspi);


#endif
