/*
 * ex5_empirical_cdf.c
 *
 * Exercise 5 -- Empirical CDF for the exponential sample of Exercise 4.
 *
 * We regenerate an exponential(lambda) sample with the same
 * inverse-transform recipe as Exercise 4 (same lambda, a dedicated
 * seed), sort it, and print the pairs (x_(i), i/n) that define the
 * empirical CDF.
 *
 * The program also computes the Kolmogorov-Smirnov statistic 
 * D_n = max_i |F_n(x_(i)) - F(x_(i))| against the exact exponential 
 * CDF F(y) = 1 - exp(-lambda y), and prints it to stderr.
 * This turns the qualitative "the two curves look close" comparison
 * from the lecture into a single quantitative number that can be
 * discussed in the report (e.g. compared with the 1/sqrt(n) scale of
 * KS fluctuations).
 *
 * Usage:
 *   ./ex5_empirical_cdf [N] [lambda] [seed]
 *   defaults: N = 50000, lambda = 1.5, seed = 97531
 *
 * Output (stdout):
 *   x_(i)   F_n(x_(i))
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcg.h"

static int compare_double(const void *pa, const void *pb) {
    double a = *(const double *)pa;
    double b = *(const double *)pb;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    long n = 50000;
    double lambda = 1.5;
    uint32_t seed = 97531u;

    if (argc > 1) n = atol(argv[1]);
    if (argc > 2) lambda = atof(argv[2]);
    if (argc > 3) seed = (uint32_t)atol(argv[3]);
    if (n <= 0 || lambda <= 0.0) {
        fprintf(stderr, "N must be positive and lambda must be > 0.\n");
        return 1;
    }

    uint32_t state = lcg_seed(seed);
    double *sample = malloc((size_t)n * sizeof(double));
    if (sample == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }

    for (long i = 0; i < n; ++i) {
        double u = lcg_next_double(&state);
        sample[i] = -log(1.0 - u) / lambda;
    }

    qsort(sample, (size_t)n, sizeof(double), compare_double);

    double d_stat = 0.0;
    for (long i = 0; i < n; ++i) {
        double f_emp = (double)(i + 1) / (double)n;
        double f_exact = 1.0 - exp(-lambda * sample[i]);
        double dev = fabs(f_emp - f_exact);
        if (dev > d_stat) d_stat = dev;

        printf("%.12f %.12f\n", sample[i], f_emp);
    }

    fprintf(stderr, "Kolmogorov-Smirnov statistic D_n = %.6f (n=%ld, sqrt(n)*D_n = %.4f)\n",
            d_stat, n, sqrt((double)n) * d_stat);

    free(sample);
    return 0;
}
