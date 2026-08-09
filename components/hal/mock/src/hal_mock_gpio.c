/**
 * @file hal_mock_gpio.c
 * @brief Mock implementation of HAL GPIO interface for host testing.
 *
 * Registra cada chamada via hal_mock_record_call(), rastreia estado de
 * inicialização por pino, armazena nível lógico para validação em testes.
 * Nível padrão: 0 (baixo).
 */

#include "hal_gpio.h"
#include "hal_mock_internal.h"

#include <string.h>

/* Número máximo de pinos GPIO suportados pelo mock (cobre ESP32-C3) */
#define HAL_MOCK_GPIO_MAX_PINS 22

/* Estado interno por pino */
static uint8_t s_gpio_initialized[HAL_MOCK_GPIO_MAX_PINS];
static uint8_t s_gpio_level[HAL_MOCK_GPIO_MAX_PINS];

/**
 * @brief Inicializa o pino GPIO (mock).
 *
 * Registra a chamada, consome erro injetado se presente,
 * marca o pino como inicializado e define nível padrão 0.
 */
hal_err_t hal_gpio_init(const hal_gpio_config_t *config)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = (config != NULL) ? config->pin : 0;
    params[1] = (config != NULL) ? (uint32_t)config->mode : 0;
    params[2] = (config != NULL) ? config->pull_up : 0;
    params[3] = (config != NULL) ? config->pull_down : 0;
    hal_mock_record_call("hal_gpio_init", params, 4);

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar parâmetro */
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (config->pin >= HAL_MOCK_GPIO_MAX_PINS) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Marcar pino como inicializado com nível padrão 0 */
    s_gpio_initialized[config->pin] = 1;
    s_gpio_level[config->pin] = 0;

    return HAL_OK;
}

/**
 * @brief Define o nível lógico de um pino GPIO (mock).
 *
 * Registra a chamada, verifica inicialização do pino,
 * consome erro injetado e armazena o valor de nível.
 */
hal_err_t hal_gpio_set_level(uint8_t pin, uint8_t level)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = pin;
    params[1] = level;
    hal_mock_record_call("hal_gpio_set_level", params, 2);

    /* Verificar inicialização */
    if (pin >= HAL_MOCK_GPIO_MAX_PINS || !s_gpio_initialized[pin]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar nível (0 ou 1) */
    if (level > 1) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Armazenar nível */
    s_gpio_level[pin] = level;

    return HAL_OK;
}

/**
 * @brief Lê o nível lógico de um pino GPIO (mock).
 *
 * Registra a chamada, verifica inicialização do pino,
 * consome erro injetado e retorna nível armazenado (padrão 0).
 */
hal_err_t hal_gpio_get_level(uint8_t pin, uint8_t *level)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = pin;
    hal_mock_record_call("hal_gpio_get_level", params, 1);

    /* Verificar inicialização */
    if (pin >= HAL_MOCK_GPIO_MAX_PINS || !s_gpio_initialized[pin]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Validar parâmetro */
    if (level == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Retornar nível armazenado */
    *level = s_gpio_level[pin];

    return HAL_OK;
}

/**
 * @brief De-inicializa o pino GPIO (mock).
 *
 * Registra a chamada, consome erro injetado, marca pino como
 * não-inicializado e reseta nível para 0.
 */
hal_err_t hal_gpio_deinit(uint8_t pin)
{
    /* Registrar chamada */
    uint32_t params[HAL_MOCK_MAX_PARAMS];
    params[0] = pin;
    hal_mock_record_call("hal_gpio_deinit", params, 1);

    /* Verificar inicialização */
    if (pin >= HAL_MOCK_GPIO_MAX_PINS || !s_gpio_initialized[pin]) {
        return HAL_ERR_NOT_INIT;
    }

    /* Consumir erro injetado */
    hal_err_t err = hal_mock_consume_next_error();
    if (err != HAL_OK) {
        return err;
    }

    /* Marcar como não-inicializado e resetar nível */
    s_gpio_initialized[pin] = 0;
    s_gpio_level[pin] = 0;

    return HAL_OK;
}
