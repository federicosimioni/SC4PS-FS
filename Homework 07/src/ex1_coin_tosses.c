/*
 * ex1_coin_tosses.c
 *
 * Exercise 1 -- Coin tosses and the Law of Large Numbers.
 *
 * Simulate N fair coin tosses with the notebook LCG: a draw u < 0.5 counts
 * as heads. We print the running fraction of heads after every toss, so
 * that the convergence toward 0.5 predicted by the LLN can be plotted.
 *
 * Usage:
 *   ./ex1_coin_tosses [N] [seed]
 *   defaults: N = 100000, seed = 123456789
 *
 * Output (stdout):
 *   toss_index   running_fraction_of_heads
 *
 * A short convergence table (fraction of heads at a few checkpoints,
 * i.e. at powers of ten) is printed to stderr as a quick numerical
 * sanity check, in addition to the full data file used for plotting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcg.h"

int main(int argc, char *argv[]) {
    long n = 100000;
    uint32_t seed = 123456789u;

    if (argc > 1) n = atol(argv[1]);
    if (argc > 2) seed = (uint32_t)atol(argv[2]);
    if (n <= 0) {
        fprintf(stderr, "N must be a positive integer.\n");
        return 1;
    }

    uint32_t state = lcg_seed(seed);
    long heads = 0;

    for (long i = 1; i <= n; ++i) {
        double u = lcg_next_double(&state);
        heads += (u < 0.5);

        printf("%ld %.10f\n", i, (double)heads / (double)i);

        /* Print a short progress table on stderr at powers of ten. */
        if (i == 10 || i == 100 || i == 1000 || i == 10000 ||
            i == 100000 || i == n) {
            fprintf(stderr, "  n = %8ld   fraction of heads = %.6f   |deviation from 0.5| = %.6f\n",
                    i, (double)heads / (double)i, fabs((double)heads / (double)i - 0.5));
        }
    }

    fprintf(stderr, "Final fraction of heads after %ld tosses: %.6f\n",
            n, (double)heads / (double)n);

    return 0;
}
