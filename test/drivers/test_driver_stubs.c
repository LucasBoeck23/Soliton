/* test/drivers/test_driver_stubs.c */
/* Unit tests for driver stubs — example-based tests */
/* Validates: Requirements 4.7, 4.6 */
/*
 * Tests:
 * 1. VL53L0X stub returns 500mm distance after init
 * 2. Battery stub returns 3700mV voltage after init
 * 3. VL53L0X returns HAL_ERR_NOT_INIT before init
 * 4. DFPlayer returns HAL_ERR_NOT_INIT before init
 * 5. Motor ERM returns HAL_ERR_NOT_INIT before init
 * 6. Battery returns HAL_ERR_NOT_INIT before init
 * 7. VL53L0X init records hal_i2c_init call in mock history
 * 8. DFPlayer init records hal_uart_init call in mock history
 */

#include "unity.h"
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

/* ─────────────────────────────────────────────────────────────────────────
 * Test 1: VL53L0X stub returns 500mm distance
 * ───────────────────────────────────────────────────────────────────────── */
void test_vl53l0x_returns_stub_distance(void)
{
    vl53l0x_config_t cfg = {
        .i2c_port = 0,
        .i2c_addr = VL53L0X_DEFAULT_ADDR
    };

    hal_err_t err = vl53l0x_init(&cfg);
    TEST_ASSERT_EQUAL(HAL_OK, err);

    uint16_t distance_mm = 0;
    err = vl53l0x_read_distance_mm(&distance_mm);
    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT16(500, distance_mm);

    vl53l0x_deinit();
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 2: Battery stub returns 3700mV voltage
 * ───────────────────────────────────────────────────────────────────────── */
void test_battery_returns_stub_voltage(void)
{
    battery_config_t cfg = {
        .adc_channel = 0,
        .divider_r1_kohm = 100,
        .divider_r2_kohm = 100
    };

    hal_err_t err = battery_init(&cfg);
    TEST_ASSERT_EQUAL(HAL_OK, err);

    uint16_t voltage_mv = 0;
    err = battery_read_voltage_mv(&voltage_mv);
    TEST_ASSERT_EQUAL(HAL_OK, err);
    TEST_ASSERT_EQUAL_UINT16(3700, voltage_mv);

    battery_deinit();
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 3: VL53L0X not-init guard
 * ───────────────────────────────────────────────────────────────────────── */
void test_vl53l0x_not_init_guard(void)
{
    uint16_t distance_mm = 0;
    hal_err_t err = vl53l0x_read_distance_mm(&distance_mm);
    TEST_ASSERT_EQUAL(HAL_ERR_NOT_INIT, err);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 4: DFPlayer not-init guard
 * ───────────────────────────────────────────────────────────────────────── */
void test_dfplayer_not_init_guard(void)
{
    hal_err_t err = dfplayer_play_track(1);
    TEST_ASSERT_EQUAL(HAL_ERR_NOT_INIT, err);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 5: Motor ERM not-init guard
 * ───────────────────────────────────────────────────────────────────────── */
void test_motor_erm_not_init_guard(void)
{
    hal_err_t err = motor_erm_set_duty(50);
    TEST_ASSERT_EQUAL(HAL_ERR_NOT_INIT, err);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 6: Battery not-init guard
 * ───────────────────────────────────────────────────────────────────────── */
void test_battery_not_init_guard(void)
{
    uint16_t voltage_mv = 0;
    hal_err_t err = battery_read_voltage_mv(&voltage_mv);
    TEST_ASSERT_EQUAL(HAL_ERR_NOT_INIT, err);
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 7: VL53L0X init records hal_i2c_init call
 * ───────────────────────────────────────────────────────────────────────── */
void test_vl53l0x_init_records_hal_call(void)
{
    vl53l0x_config_t cfg = {
        .i2c_port = 0,
        .i2c_addr = VL53L0X_DEFAULT_ADDR
    };

    hal_err_t err = vl53l0x_init(&cfg);
    TEST_ASSERT_EQUAL(HAL_OK, err);

    /* Verify that hal_i2c_init was called in the mock history */
    TEST_ASSERT_TRUE(hal_mock_was_called("hal_i2c_init"));

    vl53l0x_deinit();
}

/* ─────────────────────────────────────────────────────────────────────────
 * Test 8: DFPlayer init records hal_uart_init call
 * ───────────────────────────────────────────────────────────────────────── */
void test_dfplayer_init_records_hal_call(void)
{
    dfplayer_config_t cfg = {
        .uart_port = 1,
        .volume = 15
    };

    hal_err_t err = dfplayer_init(&cfg);
    TEST_ASSERT_EQUAL(HAL_OK, err);

    /* Verify that hal_uart_init was called in the mock history */
    TEST_ASSERT_TRUE(hal_mock_was_called("hal_uart_init"));

    dfplayer_deinit();
}

/* ─────────────────────────────────────────────────────────────────────────
 * Main — Unity test runner
 * ───────────────────────────────────────────────────────────────────────── */
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_vl53l0x_returns_stub_distance);
    RUN_TEST(test_battery_returns_stub_voltage);
    RUN_TEST(test_vl53l0x_not_init_guard);
    RUN_TEST(test_dfplayer_not_init_guard);
    RUN_TEST(test_motor_erm_not_init_guard);
    RUN_TEST(test_battery_not_init_guard);
    RUN_TEST(test_vl53l0x_init_records_hal_call);
    RUN_TEST(test_dfplayer_init_records_hal_call);
    return UNITY_END();
}
