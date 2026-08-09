#ifndef SOLITON_HAL_UART_H
#define SOLITON_HAL_UART_H

#include "hal_error.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t port;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint32_t baud_rate;
} hal_uart_config_t;

/**
 * @brief Inicializa a porta UART com a configuração fornecida.
 * @param config Ponteiro para struct de configuração UART.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_uart_init(const hal_uart_config_t *config);

/**
 * @brief Escreve dados na porta UART especificada.
 * @param port Número da porta UART.
 * @param data Ponteiro para buffer de dados a enviar.
 * @param len Número de bytes a enviar.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_uart_write(uint8_t port, const uint8_t *data, size_t len);

/**
 * @brief Lê dados da porta UART especificada com timeout.
 * @param port Número da porta UART.
 * @param buffer Ponteiro para buffer de destino.
 * @param len Número de bytes a ler.
 * @param timeout_ms Timeout em milissegundos para a operação.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_uart_read(uint8_t port, uint8_t *buffer, size_t len,
                         uint32_t timeout_ms);

/**
 * @brief De-inicializa a porta UART e libera recursos.
 * @param port Número da porta UART a de-inicializar.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_uart_deinit(uint8_t port);

#endif /* SOLITON_HAL_UART_H */
