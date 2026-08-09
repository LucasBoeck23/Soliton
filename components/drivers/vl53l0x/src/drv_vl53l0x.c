/**
 * @file drv_vl53l0x.c
 * @brief Driver stub para o sensor de distância VL53L0X (Time-of-Flight).
 *
 * Implementação stub que retorna valor fixo de 500mm para validação
 * da arquitetura e testes em host. Depende exclusivamente da camada HAL.
 */

#include "drv_vl53l0x.h"
#include "hal_i2c.h"
#include <stddef.h>

/* Estado interno do driver */
static uint8_t s_initialized = 0;
static uint8_t s_i2c_port = 0;

hal_err_t vl53l0x_init(const vl53l0x_config_t *config)
{
    /* Validar parâmetros */
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Configurar e inicializar o barramento I2C */
    hal_i2c_config_t i2c_cfg = {
        .port = config->i2c_port,
        .sda_pin = 0,   /* Pinos configurados via pinout global */
        .scl_pin = 0,
        .freq_hz = 400000  /* 400kHz padrão para VL53L0X */
    };

    hal_err_t err = hal_i2c_init(&i2c_cfg);
    if (err != HAL_OK) {
        return err;
    }

    /* Armazenar estado */
    s_i2c_port = config->i2c_port;
    s_initialized = 1;

    return HAL_OK;
}

hal_err_t vl53l0x_read_distance_mm(uint16_t *distance_mm)
{
    /* Guard: verificar se o driver foi inicializado */
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    /* Validar parâmetro de saída */
    if (distance_mm == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Stub: retorna valor fixo de 500mm */
    *distance_mm = VL53L0X_STUB_DISTANCE_MM;

    return HAL_OK;
}

hal_err_t vl53l0x_deinit(void)
{
    /* Guard: verificar se o driver foi inicializado */
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    /* De-inicializar o barramento I2C */
    hal_err_t err = hal_i2c_deinit(s_i2c_port);
    if (err != HAL_OK) {
        return err;
    }

    s_initialized = 0;

    return HAL_OK;
}
