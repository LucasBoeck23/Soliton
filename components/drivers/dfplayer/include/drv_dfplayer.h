#ifndef SOLITON_DRV_DFPLAYER_H
#define SOLITON_DRV_DFPLAYER_H

#include "hal_error.h"
#include <stdint.h>

/**
 * @brief Estrutura de configuração do driver DFPlayer Mini.
 */
typedef struct {
    uint8_t uart_port;  /* Porta UART utilizada para comunicação */
    uint8_t volume;     /* Volume inicial (0-30) */
} dfplayer_config_t;

/**
 * @brief Inicializa o driver DFPlayer Mini.
 * @param config Ponteiro para struct de configuração.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t dfplayer_init(const dfplayer_config_t *config);

/**
 * @brief Reproduz uma faixa de áudio pelo número.
 * @param track_number Número da faixa (1-65535).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t dfplayer_play_track(uint16_t track_number);

/**
 * @brief Ajusta o volume do DFPlayer.
 * @param volume Volume desejado (0-30).
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t dfplayer_set_volume(uint8_t volume);

/**
 * @brief Para a reprodução atual.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t dfplayer_stop(void);

/**
 * @brief De-inicializa o driver DFPlayer e libera recursos.
 * @return HAL_OK em sucesso, código de erro caso contrário.
 */
hal_err_t dfplayer_deinit(void);

#endif /* SOLITON_DRV_DFPLAYER_H */
