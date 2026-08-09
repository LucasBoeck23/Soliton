#ifndef SOLITON_HAL_ADC_H
#define SOLITON_HAL_ADC_H

#include "hal_error.h"
#include <stdint.h>

typedef struct {
    uint8_t channel;
    uint8_t atten;      /* Atenuação ADC */
    uint8_t width_bits; /* Resolução em bits */
} hal_adc_config_t;

/**
 * @brief Inicializa o canal ADC com a configuração fornecida.
 * @param config Ponteiro para estrutura de configuração do ADC.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_adc_init(const hal_adc_config_t *config);

/**
 * @brief Lê o valor bruto (raw) do canal ADC especificado.
 * @param channel Canal ADC a ser lido.
 * @param raw_value Ponteiro para armazenar o valor bruto lido.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_adc_read_raw(uint8_t channel, uint32_t *raw_value);

/**
 * @brief Lê a tensão em milivolts do canal ADC especificado.
 * @param channel Canal ADC a ser lido.
 * @param voltage_mv Ponteiro para armazenar a tensão em mV.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_adc_read_mv(uint8_t channel, uint32_t *voltage_mv);

/**
 * @brief De-inicializa o canal ADC e libera recursos.
 * @param channel Canal ADC a ser de-inicializado.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_adc_deinit(uint8_t channel);

#endif /* SOLITON_HAL_ADC_H */
