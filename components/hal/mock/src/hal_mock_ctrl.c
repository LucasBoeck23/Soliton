/**
 * @file hal_mock_ctrl.c
 * @brief Implementação do núcleo da Mock Layer: gravação de chamadas,
 *        buffer de histórico, reset, injeção de valores e simulação de erros.
 *
 * Este arquivo implementa todas as funções públicas de hal_mock_ctrl.h e
 * as funções internas expostas via hal_mock_internal.h para uso pelas
 * implementações mock individuais (I2C, UART, PWM, ADC, GPIO).
 */

#include "hal_mock_ctrl.h"
#include "hal_mock_internal.h"
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Estado global do mock
 * ───────────────────────────────────────────────────────────────────────── */

/* Histórico de chamadas */
static hal_mock_history_t s_history;

/* Próximo erro a ser retornado (consumido após uso) */
static hal_err_t s_next_error = HAL_OK;

/* Dados injetados para I2C read */
#define HAL_MOCK_MAX_I2C_DATA  256
static uint8_t s_i2c_read_data[HAL_MOCK_MAX_I2C_DATA];
static size_t  s_i2c_read_data_len = 0;

/* Valores injetados para ADC */
static uint32_t s_adc_raw_value   = 0;
static uint32_t s_adc_voltage_mv  = 3700; /* Default seguro conforme requisito 5.9 */

/* Dados injetados para UART read */
#define HAL_MOCK_MAX_UART_DATA  256
static uint8_t s_uart_read_data[HAL_MOCK_MAX_UART_DATA];
static size_t  s_uart_read_data_len = 0;

/* Simulação de erros I2C */
static uint8_t s_i2c_timeout_enabled = 0;
static uint8_t s_i2c_nack_enabled    = 0;

/* ─────────────────────────────────────────────────────────────────────────
 * Funções públicas — Controle (hal_mock_ctrl.h)
 * ───────────────────────────────────────────────────────────────────────── */

void hal_mock_reset(void)
{
    /* Zera histórico */
    memset(&s_history, 0, sizeof(s_history));

    /* Limpa erro pendente */
    s_next_error = HAL_OK;

    /* Limpa dados I2C injetados */
    memset(s_i2c_read_data, 0, sizeof(s_i2c_read_data));
    s_i2c_read_data_len = 0;

    /* Restaura valores ADC para defaults seguros */
    s_adc_raw_value  = 0;
    s_adc_voltage_mv = 3700;

    /* Limpa dados UART injetados */
    memset(s_uart_read_data, 0, sizeof(s_uart_read_data));
    s_uart_read_data_len = 0;

    /* Desabilita simulação de erros I2C */
    s_i2c_timeout_enabled = 0;
    s_i2c_nack_enabled    = 0;
}

void hal_mock_i2c_set_read_data(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0) {
        s_i2c_read_data_len = 0;
        return;
    }

    size_t copy_len = (len > HAL_MOCK_MAX_I2C_DATA) ? HAL_MOCK_MAX_I2C_DATA : len;
    memcpy(s_i2c_read_data, data, copy_len);
    s_i2c_read_data_len = copy_len;
}

void hal_mock_adc_set_raw_value(uint32_t raw)
{
    s_adc_raw_value = raw;
}

void hal_mock_adc_set_voltage_mv(uint32_t mv)
{
    s_adc_voltage_mv = mv;
}

void hal_mock_uart_set_read_data(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0) {
        s_uart_read_data_len = 0;
        return;
    }

    size_t copy_len = (len > HAL_MOCK_MAX_UART_DATA) ? HAL_MOCK_MAX_UART_DATA : len;
    memcpy(s_uart_read_data, data, copy_len);
    s_uart_read_data_len = copy_len;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Funções públicas — Simulação de erros
 * ───────────────────────────────────────────────────────────────────────── */

void hal_mock_set_next_error(hal_err_t err)
{
    s_next_error = err;
}

void hal_mock_i2c_simulate_timeout(uint8_t enable)
{
    s_i2c_timeout_enabled = enable;
}

void hal_mock_i2c_simulate_nack(uint8_t enable)
{
    s_i2c_nack_enabled = enable;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Funções públicas — Consulta de histórico
 * ───────────────────────────────────────────────────────────────────────── */

const hal_mock_history_t *hal_mock_get_history(void)
{
    return &s_history;
}

size_t hal_mock_get_call_count(const char *func_name)
{
    if (func_name == NULL) {
        return 0;
    }

    size_t count = 0;
    size_t limit = (s_history.call_count < HAL_MOCK_MAX_CALLS)
                       ? s_history.call_count
                       : HAL_MOCK_MAX_CALLS;

    for (size_t i = 0; i < limit; i++) {
        if (s_history.calls[i].func_name != NULL &&
            strcmp(s_history.calls[i].func_name, func_name) == 0) {
            count++;
        }
    }

    return count;
}

uint8_t hal_mock_was_called(const char *func_name)
{
    return (hal_mock_get_call_count(func_name) > 0) ? 1 : 0;
}

/* ─────────────────────────────────────────────────────────────────────────
 * Funções internas (hal_mock_internal.h) — usadas pelas implementações mock
 * ───────────────────────────────────────────────────────────────────────── */

void hal_mock_record_call(const char *func_name, const uint32_t *params, uint8_t param_count)
{
    /* Se o buffer está cheio, para de gravar (não sobrescreve) */
    if (s_history.call_count >= HAL_MOCK_MAX_CALLS) {
        return;
    }

    hal_mock_call_t *call = &s_history.calls[s_history.call_count];
    call->func_name = func_name;
    call->param_count = (param_count > HAL_MOCK_MAX_PARAMS)
                            ? HAL_MOCK_MAX_PARAMS
                            : param_count;

    if (params != NULL && call->param_count > 0) {
        memcpy(call->params, params, call->param_count * sizeof(uint32_t));
    } else {
        memset(call->params, 0, sizeof(call->params));
    }

    s_history.call_count++;
}

hal_err_t hal_mock_consume_next_error(void)
{
    hal_err_t err = s_next_error;
    /* Após consumo, reseta para HAL_OK (erro é single-shot) */
    s_next_error = HAL_OK;
    return err;
}

const uint8_t *hal_mock_get_i2c_read_data(size_t *len)
{
    if (len != NULL) {
        *len = s_i2c_read_data_len;
    }
    return s_i2c_read_data;
}

uint32_t hal_mock_get_adc_raw_value(void)
{
    return s_adc_raw_value;
}

uint32_t hal_mock_get_adc_voltage_mv(void)
{
    return s_adc_voltage_mv;
}

const uint8_t *hal_mock_get_uart_read_data(size_t *len)
{
    if (len != NULL) {
        *len = s_uart_read_data_len;
    }
    return s_uart_read_data;
}

uint8_t hal_mock_is_i2c_timeout_enabled(void)
{
    return s_i2c_timeout_enabled;
}

uint8_t hal_mock_is_i2c_nack_enabled(void)
{
    return s_i2c_nack_enabled;
}
