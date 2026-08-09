#ifndef SOLITON_HAL_PWM_H
#define SOLITON_HAL_PWM_H

#include "hal_error.h"
#include <stdint.h>

typedef struct {
    uint8_t channel;
    uint8_t gpio_pin;
    uint32_t freq_hz;
    uint8_t resolution_bits;
} hal_pwm_config_t;

/**
 * @brief Inicializa o canal PWM com a configuração fornecida.
 * @param config Ponteiro para struct de configuração do PWM.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_pwm_init(const hal_pwm_config_t *config);

/**
 * @brief Define o duty-cycle do canal PWM especificado.
 * @param channel Número do canal PWM.
 * @param duty_percent Duty-cycle em porcentagem (0-100).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_pwm_set_duty(uint8_t channel, uint8_t duty_percent);

/**
 * @brief De-inicializa o canal PWM e libera recursos.
 * @param channel Número do canal PWM a ser liberado.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_pwm_deinit(uint8_t channel);

#endif /* SOLITON_HAL_PWM_H */
