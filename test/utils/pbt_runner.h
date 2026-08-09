/* test/utils/pbt_runner.h — Property-Based Testing runner para o Sóliton */
#ifndef SOLITON_PBT_RUNNER_H
#define SOLITON_PBT_RUNNER_H

#include <stdint.h>

#define PBT_NUM_ITERATIONS  100

typedef struct {
    uint32_t seed;
    uint32_t iteration;
    uint32_t failures;
} pbt_context_t;

/**
 * @brief Inicializa o contexto PBT com seed baseada em time(NULL).
 *        Imprime a seed usada para reprodutibilidade.
 */
void pbt_init(pbt_context_t *ctx);

/**
 * @brief Gera um uint32_t no intervalo [min, max] inclusive.
 */
uint32_t pbt_gen_uint32(pbt_context_t *ctx, uint32_t min, uint32_t max);

/**
 * @brief Gera um uint8_t no intervalo [min, max] inclusive.
 */
uint8_t pbt_gen_uint8(pbt_context_t *ctx, uint8_t min, uint8_t max);

/**
 * @brief Gera um int32_t no intervalo [min, max] inclusive.
 */
int32_t pbt_gen_int32(pbt_context_t *ctx, int32_t min, int32_t max);

/**
 * @brief Macro para executar propriedade com PBT_NUM_ITERATIONS iterações.
 *        Reporta a seed em caso de falha para reprodução.
 */
#define PBT_RUN(test_func, ctx) do { \
    pbt_init(&(ctx)); \
    for (uint32_t _i = 0; _i < PBT_NUM_ITERATIONS; _i++) { \
        (ctx).iteration = _i; \
        test_func(&(ctx)); \
    } \
} while(0)

#endif /* SOLITON_PBT_RUNNER_H */
