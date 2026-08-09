/* test/hal/test_mock_injection.c */
/* Feature: project-foundation, Property 6: Mock injection round-trip */
/* Validates: Requirements 5.3, 5.4, 5.5, 5.6, 5.7 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "hal_adc.h"
#include "hal_i2c.h"
#include "hal_uart.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ─────────────────────────────────────────────────────────────────────────
 * Property 6.1: ADC voltage injection round-trip
 * Inject random value 3000-4200mV via hal_mock_adc_set_voltage_mv(),
 * init ADC, read via hal_adc_read_mv(), verify exact match.
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_adc_voltage_injection_roundtrip(pbt_context_t *ctx)
{
    uint32_t expected_mv = pbt_gen_uint32(ctx, 3000, 4200);

    hal_mock_reset();
    hal_adc_config_t cfg = { .channel = 0, .atten = 3, .width_bits = 12 };
    hal_adc_init(&cfg);
    hal_mock_adc_set_voltage_mv(expected_mv);

    uint32_t actual_mv = 0;
    hal_err_t err = hal_adc_read_mv(0, &actual_mv);

    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT32(expected_mv, actual_mv);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Property 6.2: ADC raw injection round-trip
 * Inject random raw value via hal_mock_adc_set_raw_value(),
 * read via hal_adc_read_raw(), verify exact match.
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_adc_raw_injection_roundtrip(pbt_context_t *ctx)
{
    /* 12-bit ADC: raw values 1 to 4095 (avoid 0 since mock uses 0 as "no injection") */
    uint32_t expected_raw = pbt_gen_uint32(ctx, 1, 4095);

    hal_mock_reset();
    hal_adc_config_t cfg = { .channel = 0, .atten = 3, .width_bits = 12 };
    hal_adc_init(&cfg);
    hal_mock_adc_set_raw_value(expected_raw);

    uint32_t actual_raw = 0;
    hal_err_t err = hal_adc_read_raw(0, &actual_raw);

    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT32(expected_raw, actual_raw);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Property 6.3: I2C read data injection round-trip
 * Inject random data via hal_mock_i2c_set_read_data(),
 * init I2C, read via hal_i2c_read(), verify exact match.
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_i2c_read_injection_roundtrip(pbt_context_t *ctx)
{
    /* Generate random data length (1-16 bytes) and content */
    uint8_t len = (uint8_t)pbt_gen_uint32(ctx, 1, 16);
    uint8_t expected_data[16];
    for (uint8_t i = 0; i < len; i++) {
        expected_data[i] = pbt_gen_uint8(ctx, 0, 255);
    }

    hal_mock_reset();
    hal_i2c_config_t cfg = { .port = 0, .sda_pin = 4, .scl_pin = 5, .freq_hz = 400000 };
    hal_i2c_init(&cfg);
    hal_mock_i2c_set_read_data(expected_data, len);

    uint8_t actual_data[16];
    memset(actual_data, 0xFF, sizeof(actual_data));
    hal_err_t err = hal_i2c_read(0, 0x29, actual_data, len);

    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_data, actual_data, len);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Property 6.4: UART read data injection round-trip
 * Inject random data via hal_mock_uart_set_read_data(),
 * init UART, read via hal_uart_read(), verify exact match.
 * ───────────────────────────────────────────────────────────────────────── */
static void prop_uart_read_injection_roundtrip(pbt_context_t *ctx)
{
    /* Generate random data length (1-16 bytes) and content */
    uint8_t len = (uint8_t)pbt_gen_uint32(ctx, 1, 16);
    uint8_t expected_data[16];
    for (uint8_t i = 0; i < len; i++) {
        expected_data[i] = pbt_gen_uint8(ctx, 0, 255);
    }

    hal_mock_reset();
    hal_uart_config_t cfg = { .port = 0, .tx_pin = 7, .rx_pin = 8, .baud_rate = 9600 };
    hal_uart_init(&cfg);
    hal_mock_uart_set_read_data(expected_data, len);

    uint8_t actual_data[16];
    memset(actual_data, 0xFF, sizeof(actual_data));
    hal_err_t err = hal_uart_read(0, actual_data, len, 1000);

    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_data, actual_data, len);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test runners using PBT_RUN macro (100 iterations each)
 * ───────────────────────────────────────────────────────────────────────── */

void test_property6_adc_voltage_injection_roundtrip(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_adc_voltage_injection_roundtrip, ctx);
}

void test_property6_adc_raw_injection_roundtrip(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_adc_raw_injection_roundtrip, ctx);
}

void test_property6_i2c_read_injection_roundtrip(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_i2c_read_injection_roundtrip, ctx);
}

void test_property6_uart_read_injection_roundtrip(void)
{
    pbt_context_t ctx;
    PBT_RUN(prop_uart_read_injection_roundtrip, ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property6_adc_voltage_injection_roundtrip);
    RUN_TEST(test_property6_adc_raw_injection_roundtrip);
    RUN_TEST(test_property6_i2c_read_injection_roundtrip);
    RUN_TEST(test_property6_uart_read_injection_roundtrip);
    return UNITY_END();
}
