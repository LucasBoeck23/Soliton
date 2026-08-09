#ifndef SOLITON_DRV_MOTOR_ERM_H
#define SOLITON_DRV_MOTOR_ERM_H

#include "hal_error.h"
#include <stdint.h>

typedef struct {
    uint8_t pwm_channel;
} motor_erm_config_t;

/**
 * @brief Inicializa o motor ERM com a configuração fornecida.
 * @param config Ponteiro para struct de configuração do motor.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t motor_erm_init(const motor_erm_config_t *config);

/**
 * @brief Define o duty-cycle do motor ERM.
 * @param duty_percent Duty-cycle em porcentagem (0-100).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t motor_erm_set_duty(uint8_t duty_percent);

/**
 * @brief Para o motor ERM (duty-cycle = 0).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t motor_erm_stop(void);

/**
 * @brief De-inicializa o motor ERM e libera recursos PWM.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t motor_erm_deinit(void);

#endif /* SOLITON_DRV_MOTOR_ERM_H */
