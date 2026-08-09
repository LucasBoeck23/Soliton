/* Configuração centralizada de pinout — config/include/soliton_pinout.h */
#ifndef SOLITON_CONFIG_PINOUT_H
#define SOLITON_CONFIG_PINOUT_H

/* I2C — VL53L0X */
#define SOLITON_I2C_PORT      0
#define SOLITON_I2C_SDA_PIN   4
#define SOLITON_I2C_SCL_PIN   5
#define SOLITON_I2C_FREQ_HZ   400000

/* PWM — Motor ERM */
#define SOLITON_PWM_CHANNEL   0
#define SOLITON_PWM_GPIO      6
#define SOLITON_PWM_FREQ_HZ   1000
#define SOLITON_PWM_RES_BITS  8

/* UART — DFPlayer */
#define SOLITON_UART_PORT     1
#define SOLITON_UART_TX_PIN   7
#define SOLITON_UART_RX_PIN   8
#define SOLITON_UART_BAUD     9600

/* ADC — Bateria */
#define SOLITON_ADC_CHANNEL   0
#define SOLITON_ADC_ATTEN     3  /* 11dB */
#define SOLITON_ADC_WIDTH     12 /* 12-bit */

/* Divisor resistivo bateria */
#define SOLITON_BAT_R1_KOHM   100
#define SOLITON_BAT_R2_KOHM   100

/* GPIO — Botão do usuário */
#define SOLITON_BTN_PIN       9

#endif /* SOLITON_CONFIG_PINOUT_H */
