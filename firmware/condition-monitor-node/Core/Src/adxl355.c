#include "adxl355.h"


uint8_t ADXL355_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = (reg << 1) | 0x01;
    tx[1] = 0x00;

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
                      ADXL355_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(hspi, tx, rx, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
                      ADXL355_CS_Pin,
                      GPIO_PIN_SET);

    return rx[1];
}
