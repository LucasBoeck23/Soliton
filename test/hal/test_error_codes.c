/* test/hal/test_error_codes.c */
/* Feature: project-foundation, Property 2: Error code uniqueness */
/* Validates: Requirements 3.4 */

#include "unity.h"
#include "hal_error.h"
#include <string.h>

static const hal_err_t all_error_codes[] = {
    HAL_OK,
    HAL_ERR_TIMEOUT,
    HAL_ERR_INVALID_PARAM,
    HAL_ERR_NOT_INIT,
    HAL_ERR_COMM_FAIL,
    HAL_ERR_BUSY,
    HAL_ERR_NO_MEMORY
};

#define NUM_ERROR_CODES (sizeof(all_error_codes) / sizeof(all_error_codes[0]))

void setUp(void) {}
void tearDown(void) {}

void test_all_error_codes_are_unique(void)
{
    for (size_t i = 0; i < NUM_ERROR_CODES; i++) {
        for (size_t j = i + 1; j < NUM_ERROR_CODES; j++) {
            TEST_ASSERT_NOT_EQUAL_MESSAGE(
                all_error_codes[i], all_error_codes[j],
                "Found duplicate HAL error code values");
        }
    }
}

void test_hal_err_to_str_returns_valid_strings(void)
{
    for (size_t i = 0; i < NUM_ERROR_CODES; i++) {
        const char *str = hal_err_to_str(all_error_codes[i]);
        TEST_ASSERT_NOT_NULL(str);
        TEST_ASSERT_GREATER_THAN(0, strlen(str));
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_all_error_codes_are_unique);
    RUN_TEST(test_hal_err_to_str_returns_valid_strings);
    return UNITY_END();
}
