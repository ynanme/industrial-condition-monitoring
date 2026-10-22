#include "adxl355.h"


#define ADXL355_DMA_TRANSFER_SIZE 10

static uint8_t dma_tx[ADXL355_DMA_TRANSFER_SIZE];
static uint8_t dma_rx[ADXL355_DMA_TRANSFER_SIZE];


HAL_StatusTypeDef ADXL355_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *value)
{
    return ADXL355_ReadRegisters(hspi, reg, value, 1);
}


HAL_StatusTypeDef ADXL355_ReadRegisters(SPI_HandleTypeDef *hspi, uint8_t start_reg, uint8_t *values, uint16_t n)
{
	if (n == 0 || n > ADXL355_MAX_BURST_SIZE) {
	    return HAL_ERROR;
	}

	uint8_t tx[ADXL355_MAX_BURST_SIZE+1];
	uint8_t rx[ADXL355_MAX_BURST_SIZE+1];

	tx[0] = (start_reg << 1) | 0x01;
	for (uint16_t i = 1; i <= n; i++) {
		tx[i] = 0x00;
	}

	HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
					  ADXL355_CS_Pin,
					  GPIO_PIN_RESET);

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hspi, tx, rx, n+1, HAL_MAX_DELAY);
	if (status == HAL_OK) {
		for (uint16_t i = 0; i < n; i++) {
			values[i] = rx[i+1];
		}
	}

	HAL_GPIO_WritePin(ADXL355_CS_GPIO_Port,
					  ADXL355_CS_Pin,
					  GPIO_PIN_SET);

	return status;
}


HAL_StatusTypeDef ADXL355_StartReadDMA(SPI_HandleTypeDef *hspi)
{
    dma_tx[0] = (ADXL355_REG_XDATA3 << 1) | 0x01;

    for (uint8_t i = 1; i < ADXL355_DMA_TRANSFER_SIZE; i++)
    {
        dma_tx[i] = 0x00;
    }

    HAL_GPIO_WritePin(
        ADXL355_CS_GPIO_Port,
        ADXL355_CS_Pin,
        GPIO_PIN_RESET
    );

    return HAL_SPI_TransmitReceive_DMA(
        hspi,
        dma_tx,
        dma_rx,
        ADXL355_DMA_TRANSFER_SIZE
    );
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

	status = ADXL355_WriteRegister(hspi, ADXL355_REG_FILTER, ADXL355_FILTER_2KHZ);
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


HAL_StatusTypeDef ADXL355_ReadX(SPI_HandleTypeDef *hspi, int32_t *x_raw) {

	HAL_StatusTypeDef status;

	uint8_t x1, x2, x3;
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_XDATA3, &x3);
	if (status != HAL_OK) {
		return status;
	}
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_XDATA2, &x2);
	if (status != HAL_OK) {
		return status;
	}
	status = ADXL355_ReadRegister(hspi, ADXL355_REG_XDATA1, &x1);
	if (status != HAL_OK) {
		return status;
	}

	uint32_t raw = ((uint32_t)x3 << 12) | ((uint32_t)x2 << 4)  | ((uint32_t)x1 >> 4);
	if ((raw & 0x80000) != 0) {
	    raw |= 0xFFF00000;
	}
	*x_raw = (int32_t)raw;

	return HAL_OK;
}


static int32_t ADXL355_DecodeAxis(uint8_t data3, uint8_t data2, uint8_t data1)
{
    uint32_t raw = ((uint32_t)data3 << 12) | ((uint32_t)data2 << 4) | ((uint32_t)data1 >> 4);
    if ((raw & 0x80000) != 0) {
        raw |= 0xFFF00000;
    }
    return (int32_t)raw;
}

HAL_StatusTypeDef ADXL355_ReadAcceleration(SPI_HandleTypeDef *hspi, int32_t *x_raw, int32_t *y_raw, int32_t *z_raw)
{
	uint8_t data[9];
	HAL_StatusTypeDef status = ADXL355_ReadRegisters(hspi, ADXL355_REG_XDATA3, data, 9);
	if (status != HAL_OK) {
		return status;
	}

	*x_raw = ADXL355_DecodeAxis(data[0], data[1], data[2]);
	*y_raw = ADXL355_DecodeAxis(data[3], data[4], data[5]);
	*z_raw = ADXL355_DecodeAxis(data[6], data[7], data[8]);

	return HAL_OK;
}
