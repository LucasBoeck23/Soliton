/**
 * @file drv_battery.c
 * @brief Driver stub para leitura de tensão e porcentagem da bateria via ADC.
 *
 * Implementação stub: retorna valores fixos para desenvolvimento e testes.
 * - read_voltage_mv retorna BATTERY_STUB_VOLTAGE_MV (3700mV)
 * - read_percent calcula linearmente: (V - 3000) / (4200 - 3000) * 100, clamped 0-100
 */

#include "drv_battery.h"
#include "hal_adc.h"

/* Estado interno do driver */
static uint8_t s_initialized = 0;
static battery_config_t s_config;

hal_err_t battery_init(const battery_config_t *config)
{
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Configura o canal ADC para leitura da bateria */
    hal_adc_config_t adc_cfg = {
        .channel = config->adc_channel,
        .atten = 3,         /* 11dB — faixa completa */
        .width_bits = 12    /* Resolução 12-bit */
    };

    hal_err_t err = hal_adc_init(&adc_cfg);
    if (err != HAL_OK) {
        return err;
    }

    s_config = *config;
    s_initialized = 1;

    return HAL_OK;
}

hal_err_t battery_read_voltage_mv(uint16_t *voltage_mv)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    if (voltage_mv == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Stub: retorna valor fixo de 3700mV */
    *voltage_mv = BATTERY_STUB_VOLTAGE_MV;

    return HAL_OK;
}

hal_err_t battery_read_percent(uint8_t *percent)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    if (percent == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Lê tensão atual */
    uint16_t voltage_mv = 0;
    hal_err_t err = battery_read_voltage_mv(&voltage_mv);
    if (err != HAL_OK) {
        return err;
    }

    /* Cálculo linear: (V - 3000) / (4200 - 3000) * 100, clamped 0-100 */
    if (voltage_mv <= 3000) {
        *percent = 0;
    } else if (voltage_mv >= 4200) {
        *percent = 100;
    } else {
        *percent = (uint8_t)(((uint32_t)(voltage_mv - 3000) * 100) / 1200);
    }

    return HAL_OK;
}

hal_err_t battery_deinit(void)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    hal_err_t err = hal_adc_deinit(s_config.adc_channel);
    if (err != HAL_OK) {
        return err;
    }

    s_initialized = 0;

    return HAL_OK;
}
