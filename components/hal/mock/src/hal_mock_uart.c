/**
 * @file hal_mock_uart.c
 * @brief Mock implementation of HAL UART interface for host testing.
 *
 * Records all calls via hal_mock_record_call(), tracks init state per port,
 * returns injected read data, and stores written data for DFPlayer validation.
 */

#include "hal_uart.h"
#include "hal_mock_internal.h"

#include <string.h>

/* Número máximo de portas UART suportadas pelo mock */
#define MOCK_UART_MAX_PORTS  2

/* Buffer para armazenar dados escritos (para validação de comandos DFPlayer) */
#define MOCK_UART_WRITE_BUF_SIZE  256

/* Estado de inicialização por porta */
static uint8_t s_uart_initialized[MOCK_UART_MAX_PORTS] = {0};

/* Buffer de dados escritos por porta (para verificação em testes) */
static uint8_t s_uart_write_buf[MOCK_UART_MAX_PORTS][MOCK_UART_WRITE_BUF_SIZE];
static size_t s_uart_write_len[MOCK_UART_MAX_PORTS] = {0};

hal_err_t hal_uart_init(const hal_uart_config_t *config)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];

    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    params[0] = (uint32_t)config->port;
    params[1] = (uint32_t)config->tx_pin;
    params[2] = (uint32_t)config->rx_pin;
    params[3] = config->baud_rate;
    hal_mock_record_call("hal_uart_init", params, 4);

    /* Consumir erro injetado, se houver */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    if (config->port >= MOCK_UART_MAX_PORTS) {
        return HAL_ERR_INVALID_PARAM;
    }

    s_uart_initialized[config->port] = 1;
    /* Limpar buffer de escrita ao inicializar */
    s_uart_write_len[config->port] = 0;
    memset(s_uart_write_buf[config->port], 0, MOCK_UART_WRITE_BUF_SIZE);

    return HAL_OK;
}

hal_err_t hal_uart_write(uint8_t port, const uint8_t *data, size_t len)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)port;
    params[1] = (uint32_t)(uintptr_t)data;
    params[2] = (uint32_t)len;
    hal_mock_record_call("hal_uart_write", params, 3);

    /* Verificar inicialização */
    if (port >= MOCK_UART_MAX_PORTS || !s_uart_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado, se houver */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar parâmetros */
    if (data == NULL || len == 0) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Armazenar dados escritos para verificação posterior (comandos DFPlayer) */
    size_t space = MOCK_UART_WRITE_BUF_SIZE - s_uart_write_len[port];
    size_t to_copy = (len <= space) ? len : space;
    if (to_copy > 0) {
        memcpy(&s_uart_write_buf[port][s_uart_write_len[port]], data, to_copy);
        s_uart_write_len[port] += to_copy;
    }

    return HAL_OK;
}

hal_err_t hal_uart_read(uint8_t port, uint8_t *buffer, size_t len,
                         uint32_t timeout_ms)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)port;
    params[1] = (uint32_t)(uintptr_t)buffer;
    params[2] = (uint32_t)len;
    params[3] = timeout_ms;
    hal_mock_record_call("hal_uart_read", params, 4);

    /* Verificar inicialização */
    if (port >= MOCK_UART_MAX_PORTS || !s_uart_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado, se houver */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar parâmetros */
    if (buffer == NULL || len == 0) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Copiar dados injetados ou zeros */
    size_t injected_len = 0;
    const uint8_t *injected_data = hal_mock_get_uart_read_data(&injected_len);

    if (injected_data != NULL && injected_len > 0) {
        size_t to_copy = (len <= injected_len) ? len : injected_len;
        memcpy(buffer, injected_data, to_copy);
        /* Preencher restante com zeros se buffer maior que dados injetados */
        if (len > to_copy) {
            memset(buffer + to_copy, 0, len - to_copy);
        }
    } else {
        /* Sem dados injetados: retornar zeros */
        memset(buffer, 0, len);
    }

    return HAL_OK;
}

hal_err_t hal_uart_deinit(uint8_t port)
{
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (uint32_t)port;
    hal_mock_record_call("hal_uart_deinit", params, 1);

    /* Verificar inicialização */
    if (port >= MOCK_UART_MAX_PORTS || !s_uart_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado, se houver */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    s_uart_initialized[port] = 0;
    s_uart_write_len[port] = 0;

    return HAL_OK;
}
