/*
 * ex4_exponential.c
 *
 * Exercise 4 -- Inverse transform sampling for an exponential variable.
 *
 * U ~ Uniform(0,1) is generated with the LCG and transformed via
 *     Y = -ln(1 - U) / lambda
 * which yields Y ~ Exponential(lambda), with density
 *     f_Y(y) = lambda * exp(-lambda * y),  y >= 0.
 *
 * In addition to the raw samples (for the histogram comparison in Python), 
 * the program reports the sample mean and variance and compares them with 
 * the theoretical values 1/lambda and 1/lambda^2, as a quick numerical 
 * check that the transform behaves as expected before even looking at a 
 * plot.
 *
 * Usage:
 *   ./ex4_exponential [N] [lambda] [seed]
 *   defaults: N = 200000, lambda = 1.5, seed = 246813579
 *
 * Output (stdout):
 *   y
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcg.h"

int main(int argc, char *argv[]) {
    long n = 200000;
    double lambda = 1.5;
    uint32_t seed = 246813579u;

    if (argc > 1) n = atol(argv[1]);
    if (argc > 2) lambda = atof(argv[2]);
    if (argc > 3) seed = (uint32_t)atol(argv[3]);
    if (n <= 0 || lambda <= 0.0) {
        fprintf(stderr, "N must be positive and lambda must be > 0.\n");
        return 1;
    }

    uint32_t state = lcg_seed(seed);
    double sum = 0.0, sum2 = 0.0;

    for (long i = 0; i < n; ++i) {
        double u = lcg_next_double(&state);
        double y = -log(1.0 - u) / lambda;

        sum += y;
        sum2 += y * y;

        printf("%.12f\n", y);
    }

    double mean = sum / n;
    double var = sum2 / n - mean * mean;

    fprintf(stderr, "Sample mean = %.6f  (theory: 1/lambda = %.6f)\n", mean, 1.0 / lambda);
    fprintf(stderr, "Sample var  = %.6f  (theory: 1/lambda^2 = %.6f)\n", var, 1.0 / (lambda * lambda));

    return 0;
}
