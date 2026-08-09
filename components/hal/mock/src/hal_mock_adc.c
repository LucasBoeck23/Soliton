/**
 * @file hal_mock_adc.c
 * @brief Mock implementation of HAL ADC interface for host testing.
 *
 * Registra cada chamada via hal_mock_record_call(), rastreia estado de
 * inicialização por canal, consome next_error se configurado, e retorna
 * valores injetados ou defaults (raw=0, voltage=3700mV).
 */

#include "hal_adc.h"
#include "hal_mock_internal.h"

#include <string.h>

/* Número máximo de canais ADC suportados pelo mock */
#define MOCK_ADC_MAX_CHANNELS 4

/* Estado de inicialização por canal */
static uint8_t s_adc_initialized[MOCK_ADC_MAX_CHANNELS] = {0};

/* Valor padrão de tensão em mV quando nenhum valor é injetado */
#define MOCK_ADC_DEFAULT_VOLTAGE_MV 3700

/* Valor padrão de raw quando nenhum valor é injetado */
#define MOCK_ADC_DEFAULT_RAW_VALUE  0

/**
 * @brief Inicializa o canal ADC mock.
 */
hal_err_t hal_adc_init(const hal_adc_config_t *config)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];

    /* Registra a chamada */
    if (config != NULL) {
        params[0] = (uint32_t)config->channel;
        params[1] = (uint32_t)config->atten;
        params[2] = (uint32_t)config->width_bits;
        hal_mock_record_call("hal_adc_init", params, 3);
    } else {
        hal_mock_record_call("hal_adc_init", NULL, 0);
    }

    /* Consome next_error se configurado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Valida parâmetro */
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (config->channel >= MOCK_ADC_MAX_CHANNELS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Marca canal como inicializado */
    s_adc_initialized[config->channel] = 1;

    return HAL_OK;
}

/**
 * @brief Lê valor bruto (raw) do canal ADC mock.
 */
hal_err_t hal_adc_read_raw(uint8_t channel, uint32_t *raw_value)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)channel;
    hal_mock_record_call("hal_adc_read_raw", params, 1);

    /* Consome next_error se configurado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Valida parâmetros */
    if (raw_value == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (channel >= MOCK_ADC_MAX_CHANNELS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Verifica inicialização */
    if (!s_adc_initialized[channel]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Retorna valor injetado ou default */
    uint32_t injected = hal_mock_get_adc_raw_value();
    if (injected != 0) {
        *raw_value = injected;
    } else {
        *raw_value = MOCK_ADC_DEFAULT_RAW_VALUE;
    }

    return HAL_OK;
}

/**
 * @brief Lê tensão em milivolts do canal ADC mock.
 */
hal_err_t hal_adc_read_mv(uint8_t channel, uint32_t *voltage_mv)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)channel;
    hal_mock_record_call("hal_adc_read_mv", params, 1);

    /* Consome next_error se configurado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Valida parâmetros */
    if (voltage_mv == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (channel >= MOCK_ADC_MAX_CHANNELS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Verifica inicialização */
    if (!s_adc_initialized[channel]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Retorna valor injetado ou default (3700mV) */
    uint32_t injected = hal_mock_get_adc_voltage_mv();
    if (injected != 0) {
        *voltage_mv = injected;
    } else {
        *voltage_mv = MOCK_ADC_DEFAULT_VOLTAGE_MV;
    }

    return HAL_OK;
}

/**
 * @brief De-inicializa o canal ADC mock.
 */
hal_err_t hal_adc_deinit(uint8_t channel)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)channel;
    hal_mock_record_call("hal_adc_deinit", params, 1);

    /* Consome next_error se configurado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    if (channel >= MOCK_ADC_MAX_CHANNELS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Verifica inicialização */
    if (!s_adc_initialized[channel]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Marca canal como de-inicializado */
    s_adc_initialized[channel] = 0;

    return HAL_OK;
}
