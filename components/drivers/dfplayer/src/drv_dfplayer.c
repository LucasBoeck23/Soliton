/**
 * @file drv_dfplayer.c
 * @brief Driver stub para o módulo DFPlayer Mini.
 *
 * Comunicação via UART utilizando o protocolo padrão de 10 bytes:
 * [0x7E, 0xFF, 0x06, CMD, FEEDBACK, PARAM_H, PARAM_L, CHECKSUM_H, CHECKSUM_L, 0xEF]
 */

#include "drv_dfplayer.h"
#include "hal_uart.h"
#include <string.h>

/* ─── Comandos DFPlayer ─── */
#define DFPLAYER_CMD_PLAY_TRACK   0x03
#define DFPLAYER_CMD_SET_VOLUME   0x06
#define DFPLAYER_CMD_STOP         0x16

/* ─── Constantes do protocolo ─── */
#define DFPLAYER_START_BYTE       0x7E
#define DFPLAYER_VERSION          0xFF
#define DFPLAYER_DATA_LEN         0x06
#define DFPLAYER_FEEDBACK_OFF     0x00
#define DFPLAYER_END_BYTE         0xEF
#define DFPLAYER_FRAME_SIZE       10

/* ─── Estado interno do driver ─── */
static uint8_t s_initialized = 0;
static uint8_t s_uart_port   = 0;

/* ─── Funções internas ─── */

/**
 * @brief Calcula o checksum do frame DFPlayer.
 *
 * Checksum = -(Version + Length + Cmd + Feedback + Param1 + Param2)
 * Resultado é o complemento de dois dos 6 bytes centrais.
 */
static uint16_t dfplayer_calc_checksum(uint8_t cmd, uint8_t feedback,
                                       uint8_t param_h, uint8_t param_l)
{
    uint16_t sum = (uint16_t)DFPLAYER_VERSION + (uint16_t)DFPLAYER_DATA_LEN +
                   (uint16_t)cmd + (uint16_t)feedback +
                   (uint16_t)param_h + (uint16_t)param_l;
    return (uint16_t)(-(int16_t)sum);
}

/**
 * @brief Monta e envia um frame de comando para o DFPlayer via UART.
 */
static hal_err_t dfplayer_send_cmd(uint8_t cmd, uint8_t param_h, uint8_t param_l)
{
    uint16_t checksum = dfplayer_calc_checksum(cmd, DFPLAYER_FEEDBACK_OFF,
                                               param_h, param_l);

    uint8_t frame[DFPLAYER_FRAME_SIZE] = {
        DFPLAYER_START_BYTE,       /* Byte 0: Start (0x7E) */
        DFPLAYER_VERSION,          /* Byte 1: Version (0xFF) */
        DFPLAYER_DATA_LEN,         /* Byte 2: Data length (0x06) */
        cmd,                       /* Byte 3: Command */
        DFPLAYER_FEEDBACK_OFF,     /* Byte 4: Feedback (0 = off) */
        param_h,                   /* Byte 5: Parameter high byte */
        param_l,                   /* Byte 6: Parameter low byte */
        (uint8_t)(checksum >> 8),  /* Byte 7: Checksum high byte */
        (uint8_t)(checksum & 0xFF),/* Byte 8: Checksum low byte */
        DFPLAYER_END_BYTE          /* Byte 9: End (0xEF) */
    };

    return hal_uart_write(s_uart_port, frame, DFPLAYER_FRAME_SIZE);
}

/* ─── API pública ─── */

hal_err_t dfplayer_init(const dfplayer_config_t *config)
{
    if (config == NULL) {
        return HAL_ERR_INVALID_PARAM;
    }

    if (config->volume > 30) {
        return HAL_ERR_INVALID_PARAM;
    }

    /* Configura UART para comunicação com DFPlayer (9600 baud padrão) */
    hal_uart_config_t uart_cfg = {
        .port      = config->uart_port,
        .tx_pin    = 0,  /* Pinos definidos pela camada de configuração */
        .rx_pin    = 0,
        .baud_rate = 9600
    };

    hal_err_t err = hal_uart_init(&uart_cfg);
    if (err != HAL_OK) {
        return err;
    }

    s_uart_port   = config->uart_port;
    s_initialized = 1;

    /* Configura volume inicial */
    err = dfplayer_set_volume(config->volume);
    if (err != HAL_OK) {
        /* Se falhou ao configurar volume, desfaz inicialização */
        s_initialized = 0;
        hal_uart_deinit(s_uart_port);
        return err;
    }

    return HAL_OK;
}

hal_err_t dfplayer_play_track(uint16_t track_number)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    if (track_number == 0) {
        return HAL_ERR_INVALID_PARAM;
    }

    uint8_t param_h = (uint8_t)(track_number >> 8);
    uint8_t param_l = (uint8_t)(track_number & 0xFF);

    return dfplayer_send_cmd(DFPLAYER_CMD_PLAY_TRACK, param_h, param_l);
}

hal_err_t dfplayer_set_volume(uint8_t volume)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    if (volume > 30) {
        return HAL_ERR_INVALID_PARAM;
    }

    return dfplayer_send_cmd(DFPLAYER_CMD_SET_VOLUME, 0x00, volume);
}

hal_err_t dfplayer_stop(void)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    return dfplayer_send_cmd(DFPLAYER_CMD_STOP, 0x00, 0x00);
}

hal_err_t dfplayer_deinit(void)
{
    if (!s_initialized) {
        return HAL_ERR_NOT_INIT;
    }

    hal_err_t err = hal_uart_deinit(s_uart_port);

    s_initialized = 0;
    s_uart_port   = 0;

    return err;
}
