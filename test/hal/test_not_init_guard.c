/* test/hal/test_not_init_guard.c */
/* Feature: project-foundation, Property 4: Uninitiated guard */
/* Validates: Requirements 3.6 */
/*
 * Property 4: For any HAL peripheral type (I2C, PWM, UART, ADC, GPIO) and
 * for any operation function called before init(), the HAL SHALL return
 * HAL_ERR_NOT_INIT without accessing hardware or modifying any internal state.
 *
 * Strategy: Use PBT runner to generate random port/channel/pin values within
 * valid range. For each peripheral, call operation functions WITHOUT calling
 * init first. Verify every operation returns HAL_ERR_NOT_INIT.
 * Reset mock history before each iteration to confirm no state leaks.
 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_pwm.h"
#include "hal_adc.h"
#include "hal_gpio.h"

void setUp(void)
{
    hal_mock_reset();
}

void tearDown(void) {}

/* ─────────────────────────────────────────────────────────────────────────
 * I2C: write, read, deinit — all should return HAL_ERR_NOT_INIT
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_i2c_not_init_guard(pbt_context_t *ctx)
{
    hal_mock_reset();

    /* Generate random valid port (0-1) and parameters */
    uint8_t port = pbt_gen_uint8(ctx, 0, 1);
    uint8_t dev_addr = pbt_gen_uint8(ctx, 0x08, 0x77); /* valid I2C range */
    uint8_t data_buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_buf[4] = {0};
    size_t len = pbt_gen_uint32(ctx, 1, 4);

    /* Call operations WITHOUT calling hal_i2c_init() */
    hal_err_t err_write = hal_i2c_write(port, dev_addr, data_buf, len);
    hal_err_t err_read  = hal_i2c_read(port, dev_addr, read_buf, len);
    hal_err_t err_deinit = hal_i2c_deinit(port);

    /* All must return HAL_ERR_NOT_INIT */
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_write,
        "I2C write should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_read,
        "I2C read should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_deinit,
        "I2C deinit should return HAL_ERR_NOT_INIT before init");
}

/* ─────────────────────────────────────────────────────────────────────────
 * UART: write, read, deinit — all should return HAL_ERR_NOT_INIT
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_uart_not_init_guard(pbt_context_t *ctx)
{
    hal_mock_reset();

    /* Generate random valid port (0-1) */
    uint8_t port = pbt_gen_uint8(ctx, 0, 1);
    uint8_t data_buf[4] = {0x7E, 0xFF, 0x06, 0x03};
    uint8_t read_buf[4] = {0};
    size_t len = pbt_gen_uint32(ctx, 1, 4);
    uint32_t timeout_ms = pbt_gen_uint32(ctx, 10, 1000);

    /* Call operations WITHOUT calling hal_uart_init() */
    hal_err_t err_write = hal_uart_write(port, data_buf, len);
    hal_err_t err_read  = hal_uart_read(port, read_buf, len, timeout_ms);
    hal_err_t err_deinit = hal_uart_deinit(port);

    /* All must return HAL_ERR_NOT_INIT */
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_write,
        "UART write should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_read,
        "UART read should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_deinit,
        "UART deinit should return HAL_ERR_NOT_INIT before init");
}

/* ─────────────────────────────────────────────────────────────────────────
 * PWM: set_duty, deinit — all should return HAL_ERR_NOT_INIT
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_pwm_not_init_guard(pbt_context_t *ctx)
{
    hal_mock_reset();

    /* Generate random valid channel (0-3) */
    uint8_t channel = pbt_gen_uint8(ctx, 0, 3);
    uint8_t duty = pbt_gen_uint8(ctx, 0, 100);

    /* Call operations WITHOUT calling hal_pwm_init() */
    hal_err_t err_duty = hal_pwm_set_duty(channel, duty);
    hal_err_t err_deinit = hal_pwm_deinit(channel);

    /* All must return HAL_ERR_NOT_INIT */
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_duty,
        "PWM set_duty should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_deinit,
        "PWM deinit should return HAL_ERR_NOT_INIT before init");
}

/* ─────────────────────────────────────────────────────────────────────────
 * ADC: read_raw, read_mv, deinit — all should return HAL_ERR_NOT_INIT
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_adc_not_init_guard(pbt_context_t *ctx)
{
    hal_mock_reset();

    /* Generate random valid channel (0-3) */
    uint8_t channel = pbt_gen_uint8(ctx, 0, 3);
    uint32_t raw_value = 0;
    uint32_t voltage_mv = 0;

    /* Call operations WITHOUT calling hal_adc_init() */
    hal_err_t err_raw = hal_adc_read_raw(channel, &raw_value);
    hal_err_t err_mv  = hal_adc_read_mv(channel, &voltage_mv);
    hal_err_t err_deinit = hal_adc_deinit(channel);

    /* All must return HAL_ERR_NOT_INIT */
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_raw,
        "ADC read_raw should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_mv,
        "ADC read_mv should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_deinit,
        "ADC deinit should return HAL_ERR_NOT_INIT before init");
}

/* ─────────────────────────────────────────────────────────────────────────
 * GPIO: set_level, get_level, deinit — all should return HAL_ERR_NOT_INIT
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_gpio_not_init_guard(pbt_context_t *ctx)
{
    hal_mock_reset();

    /* Generate random valid pin (0-21 for ESP32-C3) */
    uint8_t pin = pbt_gen_uint8(ctx, 0, 21);
    uint8_t level = pbt_gen_uint8(ctx, 0, 1);
    uint8_t read_level = 0;

    /* Call operations WITHOUT calling hal_gpio_init() */
    hal_err_t err_set = hal_gpio_set_level(pin, level);
    hal_err_t err_get = hal_gpio_get_level(pin, &read_level);
    hal_err_t err_deinit = hal_gpio_deinit(pin);

    /* All must return HAL_ERR_NOT_INIT */
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_set,
        "GPIO set_level should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_get,
        "GPIO get_level should return HAL_ERR_NOT_INIT before init");
    TEST_ASSERT_EQUAL_MESSAGE(HAL_ERR_NOT_INIT, err_deinit,
        "GPIO deinit should return HAL_ERR_NOT_INIT before init");
}

/* ─────────────────────────────────────────────────────────────────────────
 * PBT runners — execute each property with 100 iterations
 * ───────────────────────────────────────────────────────────────────────── */
void test_property4_i2c_not_init_guard(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_i2c_not_init_guard, ctx);
}

void test_property4_uart_not_init_guard(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_uart_not_init_guard, ctx);
}

void test_property4_pwm_not_init_guard(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_pwm_not_init_guard, ctx);
}

void test_property4_adc_not_init_guard(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_adc_not_init_guard, ctx);
}

void test_property4_gpio_not_init_guard(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_gpio_not_init_guard, ctx);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Main — Unity test runner
 * ───────────────────────────────────────────────────────────────────────── */
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property4_i2c_not_init_guard);
    RUN_TEST(test_property4_uart_not_init_guard);
    RUN_TEST(test_property4_pwm_not_init_guard);
    RUN_TEST(test_property4_adc_not_init_guard);
    RUN_TEST(test_property4_gpio_not_init_guard);
    return UNITY_END();
}
