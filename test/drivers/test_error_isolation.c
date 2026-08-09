/* test/drivers/test_error_isolation.c */
/* Feature: project-foundation, Property 3: Error state isolation */
/* Validates: Requirements 3.5, 4.6 */

/**
 * Property 3: Failed operations preserve peer peripheral state
 *
 * For any HAL operation that fails (returns an error code), all other
 * initialized peripherals SHALL remain in their previously initialized
 * state and continue to respond to subsequent operations as if the
 * failure never occurred.
 *
 * Strategy:
 * - Init all 4 peripheral types (I2C port 0, ADC channel 0, PWM channel 0,
 *   GPIO pin 5)
 * - Randomly select one peripheral to fail
 * - Set hal_mock_set_next_error() before operating on that peripheral
 * - Verify the failure returns the expected error
 * - Operate on all other peripherals and verify they return HAL_OK
 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "hal_i2c.h"
#include "hal_adc.h"
#include "hal_pwm.h"
#include "hal_gpio.h"

#include <stdio.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Peripheral types used in this test
 * ───────────────────────────────────────────────────────────────────────── */
typedef enum {
    PERIPH_I2C = 0,
    PERIPH_ADC,
    PERIPH_PWM,
    PERIPH_GPIO,
    PERIPH_COUNT
} periph_type_t;

/* Error codes to inject (excluding HAL_OK) */
static const hal_err_t error_codes[] = {
    HAL_ERR_TIMEOUT,       /* -1 */
    HAL_ERR_INVALID_PARAM, /* -2 */
    HAL_ERR_COMM_FAIL,     /* -4 */
    HAL_ERR_BUSY,          /* -5 */
    HAL_ERR_NO_MEMORY      /* -6 */
};

#define NUM_ERROR_CODES (sizeof(error_codes) / sizeof(error_codes[0]))

void setUp(void) {}
void tearDown(void) {}

/* ─────────────────────────────────────────────────────────────────────────
 * Helper: Initialize all peripherals
 * ───────────────────────────────────────────────────────────────────────── */
static void init_all_peripherals(void)
{
    hal_i2c_config_t i2c_cfg = {
        .port = 0, .sda_pin = 4, .scl_pin = 5, .freq_hz = 400000
    };
    TEST_ASSERT_EQUAL(HAL_OK, hal_i2c_init(&i2c_cfg));

    hal_adc_config_t adc_cfg = {
        .channel = 0, .atten = 3, .width_bits = 12
    };
    TEST_ASSERT_EQUAL(HAL_OK, hal_adc_init(&adc_cfg));

    hal_pwm_config_t pwm_cfg = {
        .channel = 0, .gpio_pin = 6, .freq_hz = 1000, .resolution_bits = 8
    };
    TEST_ASSERT_EQUAL(HAL_OK, hal_pwm_init(&pwm_cfg));

    hal_gpio_config_t gpio_cfg = {
        .pin = 5, .mode = HAL_GPIO_MODE_OUTPUT, .pull_up = 0, .pull_down = 0
    };
    TEST_ASSERT_EQUAL(HAL_OK, hal_gpio_init(&gpio_cfg));
}

/* ─────────────────────────────────────────────────────────────────────────
 * Helper: Perform an operation on a specific peripheral.
 * Returns the hal_err_t result.
 * ───────────────────────────────────────────────────────────────────────── */
static hal_err_t perform_operation(periph_type_t type)
{
    switch (type) {
    case PERIPH_I2C: {
        uint8_t data[] = {0x01, 0x02};
        return hal_i2c_write(0, 0x29, data, sizeof(data));
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
 * Helper: Get peripheral name string for diagnostics
 * ───────────────────────────────────────────────────────────────────────── */
static const char *periph_name(periph_type_t type)
{
    switch (type) {
    case PERIPH_I2C:  return "I2C";
    case PERIPH_ADC:  return "ADC";
    case PERIPH_PWM:  return "PWM";
    case PERIPH_GPIO: return "GPIO";
    default:          return "UNKNOWN";
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * Property test: Failed operation on one peripheral preserves others
 * ───────────────────────────────────────────────────────────────────────── */
static void test_error_isolation_property(pbt_context_t *ctx)
{
    /* Reset mock state */
    hal_mock_reset();

    /* Initialize all 4 peripheral types */
    init_all_peripherals();

    /* Randomly select which peripheral will fail */
    uint32_t fail_periph_idx = pbt_gen_uint32(ctx, 0, PERIPH_COUNT - 1);
    periph_type_t fail_periph = (periph_type_t)fail_periph_idx;

    /* Randomly select which error code to inject */
    uint32_t err_idx = pbt_gen_uint32(ctx, 0, NUM_ERROR_CODES - 1);
    hal_err_t injected_error = error_codes[err_idx];

    /* Inject the failure for the next HAL call */
    hal_mock_set_next_error(injected_error);

    /* Perform operation on the failing peripheral — should return error */
    hal_err_t fail_result = perform_operation(fail_periph);

    if (fail_result != injected_error) {
        printf("[PBT FAIL] iteration=%u, seed=%u, "
               "fail_periph=%s, injected_error=%d, got=%d\n",
               ctx->iteration, ctx->seed,
               periph_name(fail_periph),
               (int)injected_error, (int)fail_result);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        injected_error, fail_result,
        "Failed peripheral should return the injected error code");

    /* Now verify ALL OTHER peripherals still respond correctly (HAL_OK) */
    for (uint32_t i = 0; i < PERIPH_COUNT; i++) {
        if (i == fail_periph_idx) {
            continue; /* Skip the failed peripheral */
        }

        periph_type_t peer = (periph_type_t)i;
        hal_err_t peer_result = perform_operation(peer);

        if (peer_result != HAL_OK) {
            printf("[PBT FAIL] iteration=%u, seed=%u, "
                   "fail_periph=%s, peer_periph=%s, "
                   "expected HAL_OK but got=%d\n",
                   ctx->iteration, ctx->seed,
                   periph_name(fail_periph),
                   periph_name(peer),
                   (int)peer_result);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            HAL_OK, peer_result,
            "Peer peripheral must still operate correctly after another "
            "peripheral's failure");
    }
}

/* ─────────────────────────────────────────────────────────────────────────
 * Runner
 * ───────────────────────────────────────────────────────────────────────── */
void test_property_3_error_state_isolation(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_error_isolation_property, ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property_3_error_state_isolation);
    return UNITY_END();
}
