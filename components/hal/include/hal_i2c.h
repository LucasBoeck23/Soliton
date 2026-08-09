#ifndef SOLITON_HAL_I2C_H
#define SOLITON_HAL_I2C_H

#include "hal_error.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t port;       /* I2C port number (0 or 1) */
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t freq_hz;   /* Clock frequency in Hz */
} hal_i2c_config_t;

/**
 * @brief Inicializa o barramento I2C com a configuração fornecida.
 * @param config Ponteiro para a estrutura de configuração I2C.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_i2c_init(const hal_i2c_config_t *config);

/**
 * @brief Escreve dados no dispositivo I2C no endereço especificado.
 * @param port Número da porta I2C.
 * @param dev_addr Endereço do dispositivo I2C (7-bit).
 * @param data Ponteiro para os dados a serem escritos.
 * @param len Tamanho dos dados em bytes.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_i2c_write(uint8_t port, uint8_t dev_addr, const uint8_t *data, size_t len);

/**
 * @brief Lê dados do dispositivo I2C no endereço especificado.
 * @param port Número da porta I2C.
 * @param dev_addr Endereço do dispositivo I2C (7-bit).
 * @param buffer Ponteiro para o buffer de recepção.
 * @param len Número de bytes a serem lidos.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_i2c_read(uint8_t port, uint8_t dev_addr, uint8_t *buffer, size_t len);

/**
 * @brief De-inicializa o barramento I2C e libera recursos.
 * @param port Número da porta I2C a ser de-inicializada.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t hal_i2c_deinit(uint8_t port);

#endif /* SOLITON_HAL_I2C_H */
