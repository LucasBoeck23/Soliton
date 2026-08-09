/* test/utils/pbt_runner.c — Implementação do PBT runner para o Sóliton */

#include "pbt_runner.h"
#include <stdio.h>
#include <time.h>

/* -------------------------------------------------------------------------
 * PRNG: xorshift32 — algoritmo simples, portátil, sem dependência de rand()
 * Período: 2^32 - 1
 * ------------------------------------------------------------------------- */
static uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* -------------------------------------------------------------------------
 * pbt_init — Inicializa contexto com seed de time(NULL)
 * ------------------------------------------------------------------------- */
void pbt_init(pbt_context_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    ctx->seed = (uint32_t)time(NULL);
    /* Garantir que seed nunca é zero (xorshift32 requer state != 0) */
    if (ctx->seed == 0) {
        ctx->seed = 1;
    }
    ctx->iteration = 0;
    ctx->failures = 0;

    printf("[PBT] Seed: %u\n", ctx->seed);
}

/* -------------------------------------------------------------------------
 * pbt_gen_uint32 — Gera valor em [min, max] inclusive
 * ------------------------------------------------------------------------- */
uint32_t pbt_gen_uint32(pbt_context_t *ctx, uint32_t min, uint32_t max)
{
    if (ctx == NULL || min > max) {
        return min;
    }

    uint32_t raw = xorshift32(&ctx->seed);
    uint32_t range = max - min + 1;

    /* Se range é 0 (overflow: max - min + 1 == 0 quando range cobre todo uint32),
     * retornar o valor raw diretamente */
    if (range == 0) {
        return raw;
    }

    return min + (raw % range);
}

/* -------------------------------------------------------------------------
 * pbt_gen_uint8 — Gera valor em [min, max] inclusive
 * ------------------------------------------------------------------------- */
uint8_t pbt_gen_uint8(pbt_context_t *ctx, uint8_t min, uint8_t max)
{
    return (uint8_t)pbt_gen_uint32(ctx, (uint32_t)min, (uint32_t)max);
}

/* -------------------------------------------------------------------------
 * pbt_gen_int32 — Gera valor em [min, max] inclusive
 * ------------------------------------------------------------------------- */
int32_t pbt_gen_int32(pbt_context_t *ctx, int32_t min, int32_t max)
{
    if (ctx == NULL || min > max) {
        return min;
    }

    /* Converter para range unsigned para evitar overflow em aritmética signed */
    uint32_t range = (uint32_t)(max - min) + 1;
    uint32_t raw = xorshift32(&ctx->seed);

    /* Se range é 0 (overflow quando cobrindo todo int32 range), retornar raw cast */
    if (range == 0) {
        return (int32_t)raw;
    }

    uint32_t offset = raw % range;
    return min + (int32_t)offset;
}
