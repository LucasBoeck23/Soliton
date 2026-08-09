#ifndef SOLITON_HAL_GPIO_H
#define SOLITON_HAL_GPIO_H

#include "hal_error.h"
#include <stdint.h>

typedef enum {
    HAL_GPIO_MODE_INPUT,
    HAL_GPIO_MODE_OUTPUT,
    HAL_GPIO_MODE_INPUT_OUTPUT
} hal_gpio_mode_t;

typedef struct {
    uint8_t pin;
    hal_gpio_mode_t mode;
    uint8_t pull_up;    /* 1 = enable, 0 = disable */
    uint8_t pull_down;  /* 1 = enable, 0 = disable */
} hal_gpio_config_t;

/**
 * @brief Inicializa o pino GPIO com a configuração fornecida.
 * @param config Ponteiro para a estrutura de configuração GPIO.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_gpio_init(const hal_gpio_config_t *config);

/**
 * @brief Define o nível lógico de um pino GPIO de saída.
 * @param pin Número do pino GPIO.
 * @param level Nível lógico (0 = baixo, 1 = alto).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_gpio_set_level(uint8_t pin, uint8_t level);

/**
 * @brief Lê o nível lógico atual de um pino GPIO.
 * @param pin Número do pino GPIO.
 * @param level Ponteiro para armazenar o nível lido (0 ou 1).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_gpio_get_level(uint8_t pin, uint8_t *level);

/**
 * @brief De-inicializa o pino GPIO e libera recursos.
 * @param pin Número do pino GPIO a ser de-inicializado.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_gpio_deinit(uint8_t pin);

#endif /* SOLITON_HAL_GPIO_H */
