#ifndef SOLITON_HAL_ERROR_H
#define SOLITON_HAL_ERROR_H

typedef enum {
    HAL_OK                  = 0,   /* Operação bem-sucedida */
    HAL_ERR_TIMEOUT         = -1,  /* Timeout na comunicação */
    HAL_ERR_INVALID_PARAM   = -2,  /* Parâmetro inválido */
    HAL_ERR_NOT_INIT        = -3,  /* Periférico não inicializado */
    HAL_ERR_COMM_FAIL       = -4,  /* Falha de comunicação (NACK, bus error) */
    HAL_ERR_BUSY            = -5,  /* Periférico ocupado */
    HAL_ERR_NO_MEMORY       = -6,  /* Memória insuficiente */
} hal_err_t;

/**
 * @brief Retorna string descritiva para um código de erro HAL.
 * @param err Código de erro HAL.
 * @return String constante com o nome do erro.
 */
const char *hal_err_to_str(hal_err_t err);

#endif /* SOLITON_HAL_ERROR_H */
