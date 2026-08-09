#ifndef SOLITON_DRV_VL53L0X_H
#define SOLITON_DRV_VL53L0X_H

#include "hal_error.h"
#include <stdint.h>

/* Endereço I2C padrão do VL53L0X */
#define VL53L0X_DEFAULT_ADDR      0x29

/* Valor stub retornado em modo de teste (distância em mm) */
#define VL53L0X_STUB_DISTANCE_MM  500

/**
 * @brief Estrutura de configuração do driver VL53L0X.
 */
typedef struct {
    uint8_t i2c_port;   /* Porta I2C a ser utilizada (0 ou 1) */
    uint8_t i2c_addr;   /* Endereço I2C do sensor (padrão: 0x29) */
} vl53l0x_config_t;

/**
 * @brief Inicializa o driver VL53L0X com a configuração fornecida.
 * @param config Ponteiro para a estrutura de configuração.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t vl53l0x_init(const vl53l0x_config_t *config);

/**
 * @brief Lê a distância medida pelo sensor em milímetros.
 * @param distance_mm Ponteiro onde o valor da distância será armazenado.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 * @note No modo stub, retorna VL53L0X_STUB_DISTANCE_MM (500mm).
 */
hal_err_t vl53l0x_read_distance_mm(uint16_t *distance_mm);

/**
 * @brief De-inicializa o driver VL53L0X e libera recursos.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t vl53l0x_deinit(void);

#endif /* SOLITON_DRV_VL53L0X_H */
