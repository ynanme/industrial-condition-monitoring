#include "adxl355.h"


HAL_StatusTypeDef ADXL355_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *value)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = (reg << 1) | 0x01;
    tx[1] = 0x00;

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
                      ADXL355_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hspi, tx, rx, 2, HAL_MAX_DELAY);
    if (status == HAL_OK) {
    	*value = rx[1];
    }

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
                      ADXL355_CS_Pin,
                      GPIO_PIN_SET);

    return status;
}


HAL_StatusTypeDef ADXL355_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value)
{
    uint8_t tx[2];

    tx[0] = reg << 1;
    tx[1] = value;

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port, ADXL355_CS_Pin, GPIO_PIN_RESET);

    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, tx, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port, ADXL355_CS_Pin, GPIO_PIN_SET);

    return status;
}


HAL_StatusTypeDef ADXL355_Init(SPI_HandleTypeDef *hspi) {

	HAL_StatusTypeDef status;

	uint8_t partid;
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_PARTID, &partid);
	if (status != HAL_OK) {
		return status;
	}
	if (partid != ADXL355_PARTID_VALUE) {
		return HAL_ERROR;
	}

	uint8_t filter;
	status = ADXL355_WriteRegister(hspi, ADXL355_REG_FILTER, 0x01);
	if (status != HAL_OK) {
	    return status;
	}

	uint8_t range;
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_RANGE, &range);
	if (status != HAL_OK) {
		return status;
	}
	range &= 0xFC;
	range |= 0x01;
	status = ADXL355_WriteRegister(hspi, ADXL355_REG_RANGE, range);
	if (status != HAL_OK) {
		return status;
	}

	uint8_t power_ctrl;
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_POWER_CTL, &power_ctrl);
	if (status != HAL_OK) {
		return status;
	}
	power_ctrl &= 0xFE;
	status = ADXL355_WriteRegister(hspi, ADXL355_REG_POWER_CTL, power_ctrl);
	if (status != HAL_OK) {
		return status;
	}

	return status;
}
