/**
 * @file hal_mock_i2c.c
 * @brief Mock implementation of HAL I2C interface for host testing.
 *
 * Records all calls in mock history, tracks initialization state per port,
 * returns injected read data or zeros, and simulates timeout/NACK errors.
 */

#include "hal_i2c.h"
#include "hal_mock_internal.h"
#include <string.h>

/* Estado de inicialização por porta I2C (max 2 portas: 0 e 1) */
static uint8_t s_i2c_initialized[2] = {0, 0};

hal_err_t hal_i2c_init(const hal_i2c_config_t *config)
{
    uint32_t params[4] = {0};

    if (config != NULL) {
        params[0] = config->port;
        params[1] = config->sda_pin;
        params[2] = config->scl_pin;
        params[3] = config->freq_hz;
    }
    hal_mock_record_call("hal_i2c_init", params, 4);

    /* Consume injected error (if any) */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validate parameters */
    if (config == NULL || config->port > 1) {
        return HAL_ERR_INVALID_PARAM;
    }

    s_i2c_initialized[config->port] = 1;
    return HAL_OK;
}

hal_err_t hal_i2c_write(uint8_t port, uint8_t dev_addr,
                         const uint8_t *data, size_t len)
{
    uint32_t params[4];
    params[0] = port;
    params[1] = dev_addr;
    params[2] = (uint32_t)(uintptr_t)data;
    params[3] = (uint32_t)len;
    hal_mock_record_call("hal_i2c_write", params, 4);

    /* Check initialization */
    if (port > 1 || !s_i2c_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consume injected error */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Check simulated errors */
    if (hal_mock_is_i2c_timeout_enabled()) {
        return HAL_ERR_TIMEOUT;
    }
    if (hal_mock_is_i2c_nack_enabled()) {
        return HAL_ERR_COMM_FAIL;
    }

    /* Validate parameters */
    if (data == NULL || len == 0) {
        return HAL_ERR_INVALID_PARAM;
    }

    return HAL_OK;
}

hal_err_t hal_i2c_read(uint8_t port, uint8_t dev_addr,
                        uint8_t *buffer, size_t len)
{
    uint32_t params[4];
    params[0] = port;
    params[1] = dev_addr;
    params[2] = (uint32_t)(uintptr_t)buffer;
    params[3] = (uint32_t)len;
    hal_mock_record_call("hal_i2c_read", params, 4);

    /* Check initialization */
    if (port > 1 || !s_i2c_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consume injected error */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Check simulated errors */
    if (hal_mock_is_i2c_timeout_enabled()) {
        return HAL_ERR_TIMEOUT;
    }
    if (hal_mock_is_i2c_nack_enabled()) {
        return HAL_ERR_COMM_FAIL;
    }

    /* Validate parameters */
    if (buffer == NULL || len == 0) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Copy injected read data or zeros */
    size_t injected_len = 0;
    const uint8_t *injected_data = hal_mock_get_i2c_read_data(&injected_len);

    if (injected_data != NULL && injected_len > 0) {
        size_t copy_len = (len < injected_len) ? len : injected_len;
        memcpy(buffer, injected_data, copy_len);
        /* Zero remaining buffer if injected data is shorter */
        if (copy_len < len) {
            memset(buffer + copy_len, 0, len - copy_len);
        }
    } else {
        /* No injected data — return zeros */
        memset(buffer, 0, len);
    }

    return HAL_OK;
}

hal_err_t hal_i2c_deinit(uint8_t port)
{
    uint32_t params[1];
    params[0] = port;
    hal_mock_record_call("hal_i2c_deinit", params, 1);

    /* Check initialization */
    if (port > 1 || !s_i2c_initialized[port]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consume injected error */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    s_i2c_initialized[port] = 0;
    return HAL_OK;
}
