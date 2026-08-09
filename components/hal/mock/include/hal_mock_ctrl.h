#ifndef SOLITON_HAL_MOCK_CTRL_H
#define SOLITON_HAL_MOCK_CTRL_H

#include "hal_error.h"
#include <stdint.h>
#include <stddef.h>

#define HAL_MOCK_MAX_CALLS  64
#define HAL_MOCK_MAX_PARAMS 8

typedef struct {
    const char *func_name;
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    uint8_t param_count;
} hal_mock_call_t;

typedef struct {
    hal_mock_call_t calls[HAL_MOCK_MAX_CALLS];
    size_t call_count;
} hal_mock_history_t;

/* Controle de injeção de valores */
void hal_mock_reset(void);
void hal_mock_i2c_set_read_data(const uint8_t *data, size_t len);
void hal_mock_adc_set_raw_value(uint32_t raw);
void hal_mock_adc_set_voltage_mv(uint32_t mv);
void hal_mock_uart_set_read_data(const uint8_t *data, size_t len);

/* Controle de simulação de erros */
void hal_mock_set_next_error(hal_err_t err);
void hal_mock_i2c_simulate_timeout(uint8_t enable);
void hal_mock_i2c_simulate_nack(uint8_t enable);

/* Consulta de histórico */
const hal_mock_history_t *hal_mock_get_history(void);
size_t hal_mock_get_call_count(const char *func_name);
uint8_t hal_mock_was_called(const char *func_name);

#endif /* SOLITON_HAL_MOCK_CTRL_H */
