#ifndef SOLITON_HAL_MOCK_INTERNAL_H
#define SOLITON_HAL_MOCK_INTERNAL_H

#include "hal_mock_ctrl.h"

/* Internal function used by mock implementations to record calls */
void hal_mock_record_call(const char *func_name, const uint32_t *params, uint8_t param_count);

/* Internal: check and consume next error */
hal_err_t hal_mock_consume_next_error(void);

/* Internal: get injected I2C read data */
const uint8_t *hal_mock_get_i2c_read_data(size_t *len);

/* Internal: get injected ADC values */
uint32_t hal_mock_get_adc_raw_value(void);
uint32_t hal_mock_get_adc_voltage_mv(void);

/* Internal: get injected UART read data */
const uint8_t *hal_mock_get_uart_read_data(size_t *len);

/* Internal: check I2C error simulation */
uint8_t hal_mock_is_i2c_timeout_enabled(void);
uint8_t hal_mock_is_i2c_nack_enabled(void);

#endif /* SOLITON_HAL_MOCK_INTERNAL_H */
