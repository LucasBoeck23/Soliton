/* test/drivers/test_driver_init.c */
/* Feature: project-foundation, Property 5: Valid config init */
/* Validates: Requirements 4.5 */

/**
 * Property 5: Valid configuration initialization succeeds
 *
 * For any driver (VL53L0X, DFPlayer, Motor ERM, Battery) and for any
 * valid configuration struct (valid port/channel/address within hardware range),
 * calling <prefix>_init() SHALL return HAL_OK.
 */

#include "unity.h"
#include "pbt_runner.h"
#include "hal_mock_ctrl.h"
#include "drv_vl53l0x.h"
#include "drv_dfplayer.h"
#include "drv_motor_erm.h"
#include "drv_battery.h"

void setUp(void)
{
    hal_mock_reset();
}

void tearDown(void) {}

/* ─── VL53L0X: i2c_port 0-1, i2c_addr 0x08-0x77 (valid I2C range) ─── */
void test_vl53l0x_valid_config_init(pbt_context_t *ctx)
{
    hal_mock_reset();

    vl53l0x_config_t cfg = {
        .i2c_port = pbt_gen_uint8(ctx, 0, 1),
        .i2c_addr = pbt_gen_uint8(ctx, 0x08, 0x77)
    };

    hal_err_t err = vl53l0x_init(&cfg);
    TEST_ASSERT_EQUAL_MESSAGE(HAL_OK, err,
        "vl53l0x_init() should return HAL_OK for valid config");

    /* Deinit para limpar estado estático para próxima iteração */
    vl53l0x_deinit();
}

/* ─── DFPlayer: uart_port 0-1, volume 0-30 ─── */
void test_dfplayer_valid_config_init(pbt_context_t *ctx)
{
    hal_mock_reset();

    dfplayer_config_t cfg = {
        .uart_port = pbt_gen_uint8(ctx, 0, 1),
        .volume    = pbt_gen_uint8(ctx, 0, 30)
    };

    hal_err_t err = dfplayer_init(&cfg);
    TEST_ASSERT_EQUAL_MESSAGE(HAL_OK, err,
        "dfplayer_init() should return HAL_OK for valid config");

    /* Deinit para limpar estado estático para próxima iteração */
    dfplayer_deinit();
}

/* ─── Motor ERM: pwm_channel 0-3 ─── */
void test_motor_erm_valid_config_init(pbt_context_t *ctx)
{
    hal_mock_reset();

    motor_erm_config_t cfg = {
        .pwm_channel = pbt_gen_uint8(ctx, 0, 3)
    };

    hal_err_t err = motor_erm_init(&cfg);
    TEST_ASSERT_EQUAL_MESSAGE(HAL_OK, err,
        "motor_erm_init() should return HAL_OK for valid config");

    /* Deinit para limpar estado estático para próxima iteração */
    motor_erm_deinit();
}

/* ─── Battery: adc_channel 0-3, divider resistors 1-1000 kΩ ─── */
void test_battery_valid_config_init(pbt_context_t *ctx)
{
    hal_mock_reset();

    battery_config_t cfg = {
        .adc_channel     = pbt_gen_uint8(ctx, 0, 3),
        .divider_r1_kohm = (uint16_t)pbt_gen_uint32(ctx, 1, 1000),
        .divider_r2_kohm = (uint16_t)pbt_gen_uint32(ctx, 1, 1000)
    };

    hal_err_t err = battery_init(&cfg);
    TEST_ASSERT_EQUAL_MESSAGE(HAL_OK, err,
        "battery_init() should return HAL_OK for valid config");

    /* Deinit para limpar estado estático para próxima iteração */
    battery_deinit();
}

/* ─── PBT Runners ─── */

void test_property_5_vl53l0x(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_vl53l0x_valid_config_init, ctx);
}

void test_property_5_dfplayer(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_dfplayer_valid_config_init, ctx);
}

void test_property_5_motor_erm(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_motor_erm_valid_config_init, ctx);
}

void test_property_5_battery(void)
{
    pbt_context_t ctx;
    PBT_RUN(test_battery_valid_config_init, ctx);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_property_5_vl53l0x);
    RUN_TEST(test_property_5_dfplayer);
    RUN_TEST(test_property_5_motor_erm);
    RUN_TEST(test_property_5_battery);
    return UNITY_END();
}
