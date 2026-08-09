#include "hal_error.h"

const char *hal_err_to_str(hal_err_t err)
{
    switch (err) {
    case HAL_OK:
        return "HAL_OK";
    case HAL_ERR_TIMEOUT:
        return "HAL_ERR_TIMEOUT";
    case HAL_ERR_INVALID_PARAM:
        return "HAL_ERR_INVALID_PARAM";
    case HAL_ERR_NOT_INIT:
        return "HAL_ERR_NOT_INIT";
    case HAL_ERR_COMM_FAIL:
        return "HAL_ERR_COMM_FAIL";
    case HAL_ERR_BUSY:
        return "HAL_ERR_BUSY";
    case HAL_ERR_NO_MEMORY:
        return "HAL_ERR_NO_MEMORY";
    default:
        return "HAL_ERR_UNKNOWN";
    }
}
