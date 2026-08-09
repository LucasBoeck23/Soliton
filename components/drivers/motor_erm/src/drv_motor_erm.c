#include "drv_motor_erm.h"
#include "hal_pwm.h"
#include <stdbool.h>

/* Estado interno do driver */
static bool s_initialized = false;
static uint8_t s_pwm_channel = 0;

hal_err_t motor_erm_init(const motor_erm_config_t *config)
{
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Configura o canal PWM para o motor ERM */
    hal_pwm_config_t pwm_cfg = {
        .channel = config->pwm_channel,
        .gpio_pin = 0,          /* GPIO definido pela camada de aplicação */
        .freq_hz = 1000,        /* Frequência padrão para motor ERM */
        .resolution_bits = 8    /* Resolução de 8 bits */
    };

    hal_err_t err = hal_pwm_init(&pwm_cfg);
    if (err != HAL_OK) {
        return err;
    }

    s_pwm_channel = config->pwm_channel;
    s_initialized = true;
    return HAL_OK;
}

hal_err_t motor_erm_set_duty(uint8_t duty_percent)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    if (duty_percent > 100) {
        return HAL_ERR_INVALID_PARAM;
    }

    return hal_pwm_set_duty(s_pwm_channel, duty_percent);
}

hal_err_t motor_erm_stop(void)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    return hal_pwm_set_duty(s_pwm_channel, 0);
}

hal_err_t motor_erm_deinit(void)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    hal_err_t err = hal_pwm_deinit(s_pwm_channel);
    if (err != HAL_OK) {
        return err;
    }

    s_initialized = false;
    return HAL_OK;
}
