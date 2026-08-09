/* test/hal/test_mock_error_sim.c */
/* Feature: project-foundation, Property 7: Mock error simulation */
/* Validates: Requirements 5.8 */

/**
 * Property 7: Mock error simulation fidelity
 *
 * For any HAL error code configured via hal_mock_set_next_error(),
 * the next HAL operation call SHALL return that exact error code,
 * and subsequent calls (without new error configuration) SHALL return HAL_OK.
 *
 * Tests with multiple peripheral types (I2C, UART, ADC, PWM, GPIO).
 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_adc.h"
#include "hal_pwm.h"
#include "hal_gpio.h"

#include <stdio.h>

/* Error codes to test (excluding HAL_OK) */
static const hal_err_t error_codes[] = {
    HAL_ERR_TIMEOUT,       /* -1 */
    HAL_ERR_INVALID_PARAM, /* -2 */
    HAL_ERR_NOT_INIT,      /* -3 */
    HAL_ERR_COMM_FAIL,     /* -4 */
    HAL_ERR_BUSY,          /* -5 */
    HAL_ERR_NO_MEMORY      /* -6 */
};

#define NUM_ERROR_CODES (sizeof(error_codes) / sizeof(error_codes[0]))

/* Peripheral types for testing */
typedef enum {
    PERIPH_I2C = 0,
    PERIPH_UART,
    PERIPH_ADC,
    PERIPH_PWM,
    PERIPH_GPIO,
    PERIPH_COUNT
} periph_type_t;

void setUp(void) {}
void tearDown(void) {}

/* ─────────────────────────────────────────────────────────────────────────
 * Helper: Initialize a peripheral using mock (called before error testing)
 * ───────────────────────────────────────────────────────────────────────── */
static void init_peripheral(periph_type_t type)
{
    switch (type) {
    case PERIPH_I2C: {
        hal_i2c_config_t cfg = {
            .port = 0, .sda_pin = 4, .scl_pin = 5, .freq_hz = 400000
        };
        hal_i2c_init(&cfg);
        break;
    }
    case PERIPH_UART: {
        hal_uart_config_t cfg = {
            .port = 0, .tx_pin = 7, .rx_pin = 8, .baud_rate = 9600
        };
        hal_uart_init(&cfg);
        break;
    }
    case PERIPH_ADC: {
        hal_adc_config_t cfg = {
            .channel = 0, .atten = 3, .width_bits = 12
        };
        hal_adc_init(&cfg);
        break;
    }
    case PERIPH_PWM: {
        hal_pwm_config_t cfg = {
            .channel = 0, .gpio_pin = 6, .freq_hz = 1000, .resolution_bits = 8
        };
        hal_pwm_init(&cfg);
        break;
    }
    case PERIPH_GPIO: {
        hal_gpio_config_t cfg = {
            .pin = 5, .mode = HAL_GPIO_MODE_OUTPUT, .pull_up = 0, .pull_down = 0
        };
        hal_gpio_init(&cfg);
        break;
    }
    default:
        break;
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * Helper: Perform an operation on a peripheral that will consume the error.
 * Returns the hal_err_t result from the operation.
 * ───────────────────────────────────────────────────────────────────────── */
static hal_err_t perform_operation(periph_type_t type)
{
    switch (type) {
    case PERIPH_I2C: {
        uint8_t data[] = {0x01, 0x02};
        return hal_i2c_write(0, 0x29, data, sizeof(data));
    }
    case PERIPH_UART: {
        uint8_t data[] = {0x7E, 0xFF};
        return hal_uart_write(0, data, sizeof(data));
    }
    case PERIPH_ADC: {
        uint32_t voltage = 0;
        return hal_adc_read_mv(0, &voltage);
    }
    case PERIPH_PWM:
        return hal_pwm_set_duty(0, 50);
    case PERIPH_GPIO:
        return hal_gpio_set_level(5, 1);
    default:
        return HAL_OK;
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * Property test: error injection is single-shot and returns exact code
 * ───────────────────────────────────────────────────────────────────────── */
static void test_error_sim_fidelity(pbt_context_t *ctx)
{
    /* Select random error code */
    uint32_t err_idx = pbt_gen_uint32(ctx, 0, NUM_ERROR_CODES - 1);
    hal_err_t expected_error = error_codes[err_idx];

    /* Select random peripheral type */
    uint32_t periph_idx = pbt_gen_uint32(ctx, 0, PERIPH_COUNT - 1);
    periph_type_t periph = (periph_type_t)periph_idx;

    /* Reset mock state and init the peripheral */
    hal_mock_reset();
    init_peripheral(periph);

    /* Configure the error to be returned on next call */
    hal_mock_set_next_error(expected_error);

    /* Perform operation — should return the configured error */
    hal_err_t result = perform_operation(periph);

    if (result != expected_error) {
        printf("[PBT FAIL] iteration=%u, seed=%u, periph=%u, "
               "expected_error=%d, got=%d\n",
               ctx->iteration, ctx->seed, periph_idx,
               (int)expected_error, (int)result);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        expected_error, result,
        "First call after set_next_error should return the configured error");

    /* Perform the same operation again — should return HAL_OK (error consumed) */
    hal_err_t result2 = perform_operation(periph);

    if (result2 != HAL_OK) {
        printf("[PBT FAIL] iteration=%u, seed=%u, periph=%u, "
               "expected HAL_OK on 2nd call, got=%d\n",
               ctx->iteration, ctx->seed, periph_idx, (int)result2);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        HAL_OK, result2,
        "Subsequent call (no new error config) should return HAL_OK");
}

/* ─────────────────────────────────────────────────────────────────────────
 * Runner
 * ───────────────────────────────────────────────────────────────────────── */
void test_property_7_mock_error_sim_fidelity(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_error_sim_fidelity, ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property_7_mock_error_sim_fidelity);
    return UNITY_END();
}
