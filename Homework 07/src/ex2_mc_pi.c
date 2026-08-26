/*
 * ex2_mc_pi.c
 *
 * Exercise 2 -- Monte Carlo estimate of pi.
 *
 * For each sample size N we throw N points (x, y) uniformly in the unit
 * square using two independent LCG streams (one for x, one for y) and
 * estimate pi as 4 * (points inside the quarter circle) / N.
 *
 * Instead of a single realization per N (which gives a noisy, jagged 
 * error curve), we repeat the experiment R times per N with freshly-offset 
 * seeds and report the mean absolute error together with its standard 
 * deviation across repetitions. This makes the error-vs-N trend easier to 
 * read and lets us compare it directly against the theoretical Monte Carlo
 * scaling error(N) ~ N^{-1/2}.
 *
 * Usage:
 *   ./ex2_mc_pi [R]
 *   default: R = 20 repetitions per sample size
 *
 * Output (stdout):
 *   N   mean_pi_estimate   mean_abs_error   std_abs_error
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcg.h"

/* One realization of the pi estimator at sample size n, given two seeds. */
static double estimate_pi_once(long n, uint32_t seed_x, uint32_t seed_y) {
    uint32_t state_x = lcg_seed(seed_x);
    uint32_t state_y = lcg_seed(seed_y);
    long inside = 0;

    for (long i = 0; i < n; ++i) {
        double x = lcg_next_double(&state_x);
        double y = lcg_next_double(&state_y);
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }
    return 4.0 * (double)inside / (double)n;
}

int main(int argc, char *argv[]) {
    int repetitions = 20;
    if (argc > 1) repetitions = atoi(argv[1]);
    if (repetitions <= 0) {
        fprintf(stderr, "Number of repetitions must be positive.\n");
        return 1;
    }

    const long sizes[] = {100, 1000, 10000, 100000, 1000000};
    const int n_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    const double pi_true = 3.14159265358979323846;

    /* Two base seeds, one per stream; each repetition perturbs them with
     * a distinct large odd offset so repetitions do not reuse the same
     * sub-sequence of either stream. */
    const uint32_t base_seed_x = 123u;
    const uint32_t base_seed_y = 987654321u;
    const uint32_t offset = 2654435761u; /* Knuth's multiplicative constant */

    for (int s = 0; s < n_sizes; ++s) {
        long n = sizes[s];
        double sum_err = 0.0, sum_err2 = 0.0, sum_pi = 0.0;

        for (int r = 0; r < repetitions; ++r) {
            uint32_t seed_x = base_seed_x + (uint32_t)r * offset;
            uint32_t seed_y = base_seed_y + (uint32_t)r * offset;

            double pi_est = estimate_pi_once(n, seed_x, seed_y);
            double err = fabs(pi_est - pi_true);

            sum_pi += pi_est;
            sum_err += err;
            sum_err2 += err * err;
        }

        double mean_pi = sum_pi / repetitions;
        double mean_err = sum_err / repetitions;
        double var_err = sum_err2 / repetitions - mean_err * mean_err;
        double std_err = (var_err > 0.0) ? sqrt(var_err) : 0.0;

        printf("%ld %.10f %.10f %.10f\n", n, mean_pi, mean_err, std_err);
    }

    fprintf(stderr, "Estimated pi at %d repetitions per sample size.\n", repetitions);
    return 0;
}
