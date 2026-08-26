/*
 * ex3_change_of_variables.c
 *
 * Exercise 3 -- Change of variables: Y = U^2.
 *
 * U ~ Uniform(0,1) is generated with the LCG and transformed to
 * Y = U^2. By the change-of-variables formula the analytic density of
 * Y is f_Y(y) = 1 / (2 sqrt(y)) on (0, 1].
 *
 * Besides dumping the raw samples for Python to histogram, this program 
 * also estimates, on the C side, the empirical CDF of Y at a coarse grid 
 * of points and reports the maximum absolute deviation from the analytic 
 * CDF F_Y(y) = sqrt(y) (a simple Kolmogorov-Smirnov-style diagnostic). 
 * This gives a single quantitative number to comment on in the report, 
 * in addition to the qualitative histogram-vs-density plot.
 *
 * Usage:
 *   ./ex3_change_of_variables [N] [seed]
 *   defaults: N = 200000, seed = 13579
 *
 * Output (stdout):
 *   u   y
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcg.h"

int main(int argc, char *argv[]) {
    long n = 200000;
    uint32_t seed = 13579u;

    if (argc > 1) n = atol(argv[1]);
    if (argc > 2) seed = (uint32_t)atol(argv[2]);
    if (n <= 0) {
        fprintf(stderr, "N must be a positive integer.\n");
        return 1;
    }

    uint32_t state = lcg_seed(seed);
    double *y = malloc((size_t)n * sizeof(double));
    if (y == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }

    for (long i = 0; i < n; ++i) {
        double u = lcg_next_double(&state);
        y[i] = u * u;
        printf("%.12f %.12f\n", u, y[i]);
    }

    /* Quick KS-style diagnostic: max |empirical CDF - sqrt(y)| on a grid
     * of 20 equally spaced thresholds in (0, 1). This does not require
     * sorting the whole sample -- a simple counting pass per threshold
     * is enough for a coarse check. */
    const int n_grid = 20;
    double max_dev = 0.0;
    for (int g = 1; g <= n_grid; ++g) {
        double y_thr = g / (double)n_grid;
        long count_below = 0;
        for (long i = 0; i < n; ++i) {
            if (y[i] <= y_thr) count_below++;
        }
        double f_emp = (double)count_below / (double)n;
        double f_exact = sqrt(y_thr); /* F_Y(y) = sqrt(y) on (0,1] */
        double dev = fabs(f_emp - f_exact);
        if (dev > max_dev) max_dev = dev;
    }
    fprintf(stderr, "Max |F_emp - F_exact| over a 20-point grid: %.5f (n=%ld)\n", max_dev, n);

    free(y);
    return 0;
}
