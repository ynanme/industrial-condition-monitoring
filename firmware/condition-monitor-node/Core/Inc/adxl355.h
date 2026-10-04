#ifndef INC_ADXL355_H_
#define INC_ADXL355_H_

#include "main.h"

#define ADXL355_REG_PARTID   0x02
#define ADXL355_PARTID_VALUE 0xED


HAL_StatusTypeDef ADXL355_ReadRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t *value);

HAL_StatusTypeDef ADXL355_WriteRegister(
    SPI_HandleTypeDef *hspi,
    uint8_t reg,
    uint8_t value);


#endif
