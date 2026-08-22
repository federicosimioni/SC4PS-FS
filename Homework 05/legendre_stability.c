/*
 * HomeWork05 - Stability of Legendre Polynomials
 *
 * Compares, for m = 0 and l = 0..lmax (lmax = 50), three ways of computing
 * the ordinary Legendre polynomials P_l(x):
 *
 *   1) Direct three-term recurrence (Bonnet's recurrence), in double precision.
 *   2) High-precision reference values computed with MPFR,
 *      using arbitrary-precision arithmetic.
 *   3) A "Miller-like" experiment: recurrence propagated backwards
 *      from a point L > lmax with arbitrary initial conditions
 *      Q[L+1] = 0, Q[L] = 1, then rescaled by imposing Q[0] = 1.
 *
 * For each x in {0.1, 0.5, 0.9, 0.99}, the values and absolute/relative
 * errors with respect to the reference are printed for every l in a text
 * file (tabular format, also readable by an external plotting tool,
 * e.g. Python/gnuplot).
 *
 * Compilation:
 *   gcc -std=c11 -Wall -Wextra -O2 legendre_stability.c -lm -o legendre_stability
 *   gcc -std=c11 -Wall -Wextra -O2 legendre_stability.c -lmpfr -lgmp -lm -o legendre_stability
 *
 * Execution:
 *   ./legendre_stability > results.dat
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__has_include)
#if __has_include(<mpfr.h>)
#include <mpfr.h>
#define LEGENDRE_HAVE_MPFR 1
#endif
#endif

#ifndef LEGENDRE_HAVE_MPFR
#define LEGENDRE_HAVE_MPFR 0
#endif

/* ------------------------------------------------------------------ */
/* 1. Direct forward recurrence (double precision)                    */
/* ------------------------------------------------------------------ */
/*
 * Bonnet:  P_0 = 1,  P_1 = x,
 *          P_{l+1} = ((2l+1) x P_l - l P_{l-1}) / (l+1),  l >= 1.
 */
void legendre_forward(double x, int lmax, double *P)
{
    P[0] = 1.0;
    if (lmax == 0) {
        return;
    }
    P[1] = x;
    for (int l = 1; l < lmax; ++l) {
        P[l + 1] = ((2.0 * l + 1.0) * x * P[l] - (double)l * P[l - 1])
                   / (double)(l + 1);
    }
}

/* ------------------------------------------------------------------ */
/* 2. High-precision reference values with MPFR                       */
/* ------------------------------------------------------------------ */
/*
 * The same Bonnet recurrence is evaluated with MPFR at high precision,
 * so the reference has negligible roundoff error compared with double
 * precision. This is a better test than repeating the same algorithm with
 * a type that is only slightly more precise, because it greatly reduces
 * the risk of sharing the same numerical error.
 */
void legendre_reference(double x, int lmax, double *Pref)
{
#if LEGENDRE_HAVE_MPFR
    const mpfr_prec_t prec = 256;
    mpfr_t xl;
    mpfr_t *P = malloc((size_t)(lmax + 1) * sizeof(*P));
    if (P == NULL) {
        fprintf(stderr, "allocation failed in legendre_reference\n");
        exit(EXIT_FAILURE);
    }

    mpfr_init2(xl, prec);
    mpfr_set_d(xl, x, MPFR_RNDN);

    for (int l = 0; l <= lmax; ++l) {
        mpfr_init2(P[l], prec);
    }

    mpfr_set_ui(P[0], 1, MPFR_RNDN);
    if (lmax >= 1) {
        mpfr_set(P[1], xl, MPFR_RNDN);
    }
    for (int l = 1; l < lmax; ++l) {
        mpfr_t num;
        mpfr_t prev;
        mpfr_init2(num, prec);
        mpfr_init2(prev, prec);

        mpfr_mul_si(num, P[l], 2 * l + 1, MPFR_RNDN);
        mpfr_mul(num, num, xl, MPFR_RNDN);
        mpfr_mul_si(prev, P[l - 1], l, MPFR_RNDN);
        mpfr_sub(num, num, prev, MPFR_RNDN);
        mpfr_div_ui(P[l + 1], num, (unsigned long)(l + 1), MPFR_RNDN);

        mpfr_clear(prev);
        mpfr_clear(num);
    }

    for (int l = 0; l <= lmax; ++l) {
        Pref[l] = mpfr_get_d(P[l], MPFR_RNDN);
        mpfr_clear(P[l]);
    }
    mpfr_clear(xl);
    free(P);
#else
    long double xl = (long double)x;
    long double *P = malloc((size_t)(lmax + 1) * sizeof(*P));
    if (P == NULL) {
        fprintf(stderr, "allocation failed in legendre_reference\n");
        exit(EXIT_FAILURE);
    }

    P[0] = 1.0L;
    if (lmax >= 1) {
        P[1] = xl;
    }
    for (int l = 1; l < lmax; ++l) {
        P[l + 1] = (((long double)(2 * l + 1)) * xl * P[l]
                    - (long double)l * P[l - 1]) / (long double)(l + 1);
    }

    for (int l = 0; l <= lmax; ++l) {
        Pref[l] = (double)P[l];
    }
    free(P);
#endif
}

/* ------------------------------------------------------------------ */
/* 3. Backward recurrence experiment (Miller-style)                   */
/* ------------------------------------------------------------------ */
/*
 * Solving Bonnet's recurrence for P_{l-1}:
 *
 *   P_{l-1}(x) = ((2l+1) x P_l(x) - (l+1) P_{l+1}(x)) / l,  l = L,...,1.
 *
 * We start with Q[L+1] = 0, Q[L] = 1 (ARBITRARY conditions), propagate
 * backwards to Q[0], and rescale by imposing c = 1 / Q[0], so that
 * P_0^(back) = 1. The test checks whether the rescaled sequence
 * P_l^(back) = c * Q[l] actually reproduces P_l(x).
 */
void legendre_backward_experiment(double x, int lmax, int L, double *P)
{
    if (L <= lmax) {
        fprintf(stderr, "need L > lmax in legendre_backward_experiment\n");
        exit(EXIT_FAILURE);
    }

    double *Q = calloc((size_t)(L + 2), sizeof(*Q));
    if (Q == NULL) {
        fprintf(stderr, "allocation failed in legendre_backward_experiment\n");
        exit(EXIT_FAILURE);
    }

    Q[L + 1] = 0.0;
    Q[L]     = 1.0;

    for (int l = L; l >= 1; --l) {
        Q[l - 1] = ((2.0 * l + 1.0) * x * Q[l]
                    - (double)(l + 1) * Q[l + 1]) / (double)l;
    }

    double scale = 1.0 / Q[0];
    for (int l = 0; l <= lmax; ++l) {
        P[l] = scale * Q[l];
    }

    free(Q);
}

/* ------------------------------------------------------------------ */
/* Relative error (falling back to absolute error when Pref ~ 0)       */
/* ------------------------------------------------------------------ */
static double relative_error(double value, double reference)
{
    double abs_err = fabs(value - reference);
    if (fabs(reference) > DBL_MIN) {
        return abs_err / fabs(reference);
    }
    return abs_err;
}

int main(void)
{
    const int lmax = 50;
    const int L = 80;               /* starting point for the backward recurrence */
    const double xs[] = {0.1, 0.5, 0.9, 0.99};
    const int nx = (int)(sizeof(xs) / sizeof(xs[0]));

    double Pf[51];
    double Pb[51];
    double Pref[51];

    printf("# x l P_ref P_forward P_backward_exp ");
    printf("abs_err_forward rel_err_forward abs_err_backward rel_err_backward\n");

    for (int ix = 0; ix < nx; ++ix) {
        double x = xs[ix];

        legendre_reference(x, lmax, Pref);
        legendre_forward(x, lmax, Pf);
        legendre_backward_experiment(x, lmax, L, Pb);

        for (int l = 0; l <= lmax; ++l) {
            double abs_err_f = fabs(Pf[l] - Pref[l]);
            double abs_err_b = fabs(Pb[l] - Pref[l]);
            double rel_err_f = relative_error(Pf[l], Pref[l]);
            double rel_err_b = relative_error(Pb[l], Pref[l]);

            printf("%.17g %d %.17e %.17e %.17e %.17e %.17e %.17e %.17e\n",
                   x, l, Pref[l], Pf[l], Pb[l],
                   abs_err_f, rel_err_f, abs_err_b, rel_err_b);
        }
        printf("\n");
    }

    return 0;
}
