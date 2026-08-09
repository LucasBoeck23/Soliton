#ifndef SOLITON_DRV_BATTERY_H
#define SOLITON_DRV_BATTERY_H

#include "hal_error.h"
#include <stdint.h>

#define BATTERY_STUB_VOLTAGE_MV  3700  /* Valor stub para testes */

typedef struct {
    uint8_t adc_channel;
    uint16_t divider_r1_kohm;  /* Resistor superior do divisor */
    uint16_t divider_r2_kohm;  /* Resistor inferior do divisor */
} battery_config_t;

/**
 * @brief Inicializa o driver de bateria com a configuração fornecida.
 * @param config Ponteiro para estrutura de configuração da bateria.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t battery_init(const battery_config_t *config);

/**
 * @brief Lê a tensão da bateria em milivolts.
 * @param voltage_mv Ponteiro para armazenar a tensão lida em mV.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t battery_read_voltage_mv(uint16_t *voltage_mv);

/**
 * @brief Lê a porcentagem de carga da bateria (0-100%).
 * @param percent Ponteiro para armazenar a porcentagem de carga.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t battery_read_percent(uint8_t *percent);

/**
 * @brief De-inicializa o driver de bateria e libera recursos.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t battery_deinit(void);

#endif /* SOLITON_DRV_BATTERY_H */
