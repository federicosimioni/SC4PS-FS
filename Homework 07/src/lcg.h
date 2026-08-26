/*
 * lcg.h -- Linear Congruential Generator.
 *
 * This is the same generator introduced in the lecture notebook:
 *
 *     x_{n+1} = (a * x_n + c) mod 2^32
 *     u_n     = x_n / 2^32   in [0, 1)
 *
 * It is factored out into a single header (instead of being copy-pasted
 * into every exercise, as in the lecture examples) so that all five
 * programs in this homework share exactly one definition of the
 * generator. If the recurrence ever needs to change, it changes here
 * only.
 *
 * The multiplier/increment pair (a, c) below are the classic Numerical
 * Recipes constants also used in the lecture notes.
 */

#ifndef HW07_LCG_H
#define HW07_LCG_H

#include <stdint.h>

#define LCG_A 1664525u
#define LCG_C 1013904223u
/* modulus is implicit: 2^32, i.e. the natural wraparound of uint32_t */

/* Advance the generator state by one step and return the raw 32-bit output. */
static inline uint32_t lcg_next_u32(uint32_t *state) {
    *state = LCG_A * (*state) + LCG_C;
    return *state;
}

/* Return the next pseudo-random double, uniform on [0, 1). */
static inline double lcg_next_double(uint32_t *state) {
    return lcg_next_u32(state) / 4294967296.0; /* 2^32 */
}

/*
 * Initialize a state from an integer seed.
 *
 * A seed of 0 would make the very first call degenerate to just "c",
 * which is harmless for this LCG but slightly unpleasant, so we fold
 * the seed with a fixed odd constant before use. This does not change
 * the generator's statistical properties; it only avoids relying on
 * the caller never passing 0.
 */
static inline uint32_t lcg_seed(uint32_t seed) {
    return seed ^ 0x9E3779B9u;
}

#endif /* HW07_LCG_H */
