/**
 * @file hal_mock_pwm.c
 * @brief Mock implementation of HAL PWM interface for host testing.
 *
 * Registra cada chamada via hal_mock_record_call(), rastreia estado de
 * inicialização por canal, armazena duty-cycle para validação em testes.
 * Duty padrão: 0%.
 */

#include "hal_pwm.h"
#include "hal_mock_internal.h"

#include <string.h>

/* Número máximo de canais PWM suportados pelo mock */
#define HAL_MOCK_PWM_MAX_CHANNELS 4

/* Estado interno por canal */
static uint8_t s_pwm_initialized[HAL_MOCK_PWM_MAX_CHANNELS];
static uint8_t s_pwm_duty[HAL_MOCK_PWM_MAX_CHANNELS];

/**
 * @brief Inicializa o canal PWM (mock).
 *
 * Registra a chamada, consome erro injetado se presente,
 * marca o canal como inicializado e define duty padrão 0%.
 */
hal_err_t hal_pwm_init(const hal_pwm_config_t *config)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (config != NULL) ? config->channel : 0;
    params[1] = (config != NULL) ? config->gpio_pin : 0;
    params[2] = (config != NULL) ? config->freq_hz : 0;
    params[3] = (config != NULL) ? config->resolution_bits : 0;
    hal_mock_record_call("hal_pwm_init", params, 4);

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar parâmetro */
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (config->channel >= HAL_MOCK_PWM_MAX_CHANNELS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Marcar canal como inicializado com duty padrão 0% */
    s_pwm_initialized[config->channel] = 1;
    s_pwm_duty[config->channel] = 0;

    return HAL_OK;
}

/**
 * @brief Define duty-cycle do canal PWM (mock).
 *
 * Registra a chamada, verifica inicialização do canal,
 * consome erro injetado e armazena o valor de duty.
 */
hal_err_t hal_pwm_set_duty(uint8_t channel, uint8_t duty_percent)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = channel;
    params[1] = duty_percent;
    hal_mock_record_call("hal_pwm_set_duty", params, 2);

    /* Verificar inicialização */
    if (channel >= HAL_MOCK_PWM_MAX_CHANNELS || !s_pwm_initialized[channel]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar duty-cycle (0-100%) */
    if (duty_percent > 100) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Armazenar valor de duty */
    s_pwm_duty[channel] = duty_percent;

    return HAL_OK;
}

/**
 * @brief De-inicializa o canal PWM (mock).
 *
 * Registra a chamada, consome erro injetado, marca canal como
 * não-inicializado e reseta duty para 0%.
 */
hal_err_t hal_pwm_deinit(uint8_t channel)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = channel;
    hal_mock_record_call("hal_pwm_deinit", params, 1);

    /* Verificar inicialização */
    if (channel >= HAL_MOCK_PWM_MAX_CHANNELS || !s_pwm_initialized[channel]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Marcar como não-inicializado e resetar duty */
    s_pwm_initialized[channel] = 0;
    s_pwm_duty[channel] = 0;

    return HAL_OK;
}
