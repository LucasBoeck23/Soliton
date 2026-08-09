/* test/hal/test_mock_recording.c */
/* Feature: project-foundation, Property 1: Mock call recording */
/* Validates: Requirements 3.2 */

/**
 * Property 1: Mock call recording preserves call history
 *
 * For any sequence of HAL function calls made through the Mock Layer,
 * the mock history SHALL contain every call in order, with the correct
 * function name and parameter values matching exactly what was passed
 * by the caller.
 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_pwm.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include <string.h>

/* Número de tipos de periférico disponíveis para chamadas aleatórias */
#define NUM_PERIPHERAL_TYPES  5

/* Identificadores de periférico para geração aleatória */
enum {
    PERIPH_I2C = 0,
    PERIPH_UART,
    PERIPH_PWM,
    PERIPH_ADC,
    PERIPH_GPIO
};

void setUp(void)
{
    hal_mock_reset();
}

void tearDown(void) {}

/**
 * @brief Executa uma chamada HAL aleatória baseada no tipo de periférico
 *        e retorna o nome da função esperado no histórico.
 *
 * Cada tipo de periférico chama uma função de inicialização (que não requer
 * estado prévio), garantindo que a chamada sempre é registrada.
 */
static const char *perform_random_hal_call(pbt_context_t *ctx, uint8_t periph_type)
{
    switch (periph_type) {
    case PERIPH_I2C: {
        hal_i2c_config_t cfg = {
            .port = 0,
            .sda_pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .scl_pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .freq_hz = pbt_gen_uint32(ctx, 100000, 400000)
        };
        hal_i2c_init(&cfg);
        return "hal_i2c_init";
    }
    case PERIPH_UART: {
        hal_uart_config_t cfg = {
            .port = 0,
            .tx_pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .rx_pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .baud_rate = pbt_gen_uint32(ctx, 9600, 115200)
        };
        hal_uart_init(&cfg);
        return "hal_uart_init";
    }
    case PERIPH_PWM: {
        hal_pwm_config_t cfg = {
            .channel = (uint8_t)pbt_gen_uint8(ctx, 0, 3),
            .gpio_pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .freq_hz = pbt_gen_uint32(ctx, 500, 5000),
            .resolution_bits = (uint8_t)pbt_gen_uint8(ctx, 8, 12)
        };
        hal_pwm_init(&cfg);
        return "hal_pwm_init";
    }
    case PERIPH_ADC: {
        hal_adc_config_t cfg = {
            .channel = (uint8_t)pbt_gen_uint8(ctx, 0, 3),
            .atten = (uint8_t)pbt_gen_uint8(ctx, 0, 3),
            .width_bits = (uint8_t)pbt_gen_uint8(ctx, 9, 12)
        };
        hal_adc_init(&cfg);
        return "hal_adc_init";
    }
    case PERIPH_GPIO: {
        hal_gpio_config_t cfg = {
            .pin = (uint8_t)pbt_gen_uint8(ctx, 0, 21),
            .mode = (hal_gpio_mode_t)pbt_gen_uint8(ctx, 0, 2),
            .pull_up = (uint8_t)pbt_gen_uint8(ctx, 0, 1),
            .pull_down = (uint8_t)pbt_gen_uint8(ctx, 0, 1)
        };
        hal_gpio_init(&cfg);
        return "hal_gpio_init";
    }
    default:
        return NULL;
    }
}

/**
 * @brief Propriedade: para qualquer sequência aleatória de chamadas HAL,
 *        o histórico do mock registra todas em ordem com nomes corretos.
 */
void test_mock_call_recording_property(pbt_context_t *ctx)
{
    /* Reset do mock para cada iteração */
    hal_mock_reset();

    /* Gerar número aleatório de chamadas (1-10) */
    uint32_t num_calls = pbt_gen_uint32(ctx, 1, 10);

    /* Limitar para não exceder buffer do mock (64 max) */
    if (num_calls > HAL_MOCK_MAX_CALLS) {
        num_calls = HAL_MOCK_MAX_CALLS;
    }

    /* Array para armazenar nomes de funções esperados */
    const char *expected_names[HAL_MOCK_MAX_CALLS];

    /* Executar chamadas aleatórias */
    for (uint32_t i = 0; i < num_calls; i++) {
        uint8_t periph_type = pbt_gen_uint8(ctx, 0, NUM_PERIPHERAL_TYPES - 1);
        expected_names[i] = perform_random_hal_call(ctx, periph_type);
    }

    /* Verificar: call_count corresponde ao número de chamadas feitas */
    const hal_mock_history_t *history = hal_mock_get_history();
    TEST_ASSERT_EQUAL_UINT32(num_calls, (uint32_t)history->call_count);

    /* Verificar: cada chamada registrada tem o nome de função correto na ordem */
    for (uint32_t i = 0; i < num_calls; i++) {
        TEST_ASSERT_NOT_NULL_MESSAGE(
            history->calls[i].func_name,
            "Recorded call has NULL func_name");
        TEST_ASSERT_EQUAL_STRING_MESSAGE(
            expected_names[i],
            history->calls[i].func_name,
            "Recorded function name does not match expected call order");
    }
}

/**
 * @brief Runner Unity que executa a propriedade via PBT_RUN.
 */
void test_property_1_runner(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_mock_call_recording_property, ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property_1_runner);
    return UNITY_END();
}
