#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/*
 * Homework 03 - Matrix Multiplication
 *
 * This program:
 *   - reads a, b, N, fileout [, repetitions]
 *   - allocates A, B, C as contiguous 1D arrays
 *   - initializes A and B with the homework prescription:
 *         A[i][j] = a,   B[i][j] = b
 *   - benchmarks the 6 possible loop orderings for matrix multiplication
 *   - validates the computed result:
 *         1) exact comparison with a trusted reference implementation
 *         2) Freivalds' algorithm (probabilistic O(N^2) checker)
 *   - writes the fastest result matrix to a text file
 */

#define IDX(i,j,N) ((size_t)(i) * (size_t)(N) + (size_t)(j))
#define FREIVALDS_ITERS 5

static double now_seconds(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

static void *xmalloc(size_t nbytes) {
    void *p = malloc(nbytes);
    if (!p) {
        fprintf(stderr, "Allocation failed for %zu bytes\n", nbytes);
        exit(EXIT_FAILURE);
    }
    return p;
}

static double *alloc_matrix(int N) {
    return (double *)xmalloc((size_t)N * (size_t)N * sizeof(double));
}

static void zero_matrix(double *M, int N) {
    memset(M, 0, (size_t)N * (size_t)N * sizeof(double));
}

static void fill_constant(double *M, int N, double value) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            M[IDX(i,j,N)] = value;
        }
    }
}

/* Optional helper for generic experiments: not used by default.
static void fill_pseudorandom(double *M, int N, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            M[IDX(i,j,N)] = ((double)(rand() % 1000) / 100.0) - 5.0;
        }
    }
}
*/

static int save_matrix_to_file(const char *filename, const double *M, int N) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot open output file '%s': %s\n", filename, strerror(errno));
        return 0;
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            fprintf(fp, "%.10g", M[IDX(i,j,N)]);
            if (j < N - 1) {
                fputc(' ', fp);
            }
        }
        fputc('\n', fp);
    }

    fclose(fp);
    return 1;
}

static int compare_matrices(const double *X, const double *Y, int N, double tol,
                            double *max_abs_diff, int *mi, int *mj) {
    double worst = 0.0;
    int wi = -1, wj = -1;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double diff = fabs(X[IDX(i,j,N)] - Y[IDX(i,j,N)]);
            if (diff > worst) {
                worst = diff;
                wi = i;
                wj = j;
            }
            if (diff > tol) {
                if (max_abs_diff) *max_abs_diff = worst;
                if (mi) *mi = wi;
                if (mj) *mj = wj;
                return 0;
            }
        }
    }

    if (max_abs_diff) *max_abs_diff = worst;
    if (mi) *mi = wi;
    if (mj) *mj = wj;
    return 1;
}

/*
 * Freivalds' algorithm:
 * checks whether C = A*B with probabilistic cost O(k N^2).
 * Repeating it a few times makes false positives very unlikely.
 */
static int freivalds_check(const double *A, const double *B, const double *C,
                           int N, int iterations, double tol) {
    double *r  = (double *)xmalloc((size_t)N * sizeof(double));
    double *Br = (double *)xmalloc((size_t)N * sizeof(double));
    double *Cr = (double *)xmalloc((size_t)N * sizeof(double));
    double *ABr = (double *)xmalloc((size_t)N * sizeof(double));

    srand(1234567u);

    for (int t = 0; t < iterations; ++t) {
        for (int i = 0; i < N; ++i) {
            r[i] = (double)(rand() & 1u); /* random 0/1 vector */
        }

        for (int i = 0; i < N; ++i) {
            double sumB = 0.0;
            double sumC = 0.0;
            for (int j = 0; j < N; ++j) {
                sumB += B[IDX(i,j,N)] * r[j];
                sumC += C[IDX(i,j,N)] * r[j];
            }
            Br[i] = sumB;
            Cr[i] = sumC;
        }

        for (int i = 0; i < N; ++i) {
            double sum = 0.0;
            for (int j = 0; j < N; ++j) {
                sum += A[IDX(i,j,N)] * Br[j];
            }
            ABr[i] = sum;
        }

        for (int i = 0; i < N; ++i) {
            if (fabs(ABr[i] - Cr[i]) > tol) {
                free(r); free(Br); free(Cr); free(ABr);
                return 0;
            }
        }
    }

    free(r); free(Br); free(Cr); free(ABr);
    return 1;
}

/* ===== Matrix multiplication kernels ===== */

static void matmul_ijk(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C[IDX(i,j,N)] += A[IDX(i,k,N)] * B[IDX(k,j,N)];
}

static void matmul_ikj(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int i = 0; i < N; ++i)
        for (int k = 0; k < N; ++k) {
            double aik = A[IDX(i,k,N)];
            for (int j = 0; j < N; ++j)
                C[IDX(i,j,N)] += aik * B[IDX(k,j,N)];
        }
}

static void matmul_jik(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
            for (int k = 0; k < N; ++k)
                C[IDX(i,j,N)] += A[IDX(i,k,N)] * B[IDX(k,j,N)];
}

static void matmul_jki(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int j = 0; j < N; ++j)
        for (int k = 0; k < N; ++k) {
            double bkj = B[IDX(k,j,N)];
            for (int i = 0; i < N; ++i)
                C[IDX(i,j,N)] += A[IDX(i,k,N)] * bkj;
        }
}

static void matmul_kij(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int k = 0; k < N; ++k)
        for (int i = 0; i < N; ++i) {
            double aik = A[IDX(i,k,N)];
            for (int j = 0; j < N; ++j)
                C[IDX(i,j,N)] += aik * B[IDX(k,j,N)];
        }
}

static void matmul_kji(const double *A, const double *B, double *C, int N) {
    zero_matrix(C, N);
    for (int k = 0; k < N; ++k)
        for (int j = 0; j < N; ++j) {
            double bkj = B[IDX(k,j,N)];
            for (int i = 0; i < N; ++i)
                C[IDX(i,j,N)] += A[IDX(i,k,N)] * bkj;
        }
}

typedef void (*matmul_fn)(const double *, const double *, double *, int);

typedef struct {
    const char *name;
    matmul_fn fn;
    double best_time;
} Variant;

static double benchmark_variant(matmul_fn fn, const double *A, const double *B,
                                double *C, int N, int repetitions) {
    double best = 1e300;
    for (int r = 0; r < repetitions; ++r) {
        double t0 = now_seconds();
        fn(A, B, C, N);
        double t1 = now_seconds();
        double dt = t1 - t0;
        if (dt < best) best = dt;
    }
    return best;
}

int main(int argc, char **argv) {
    if (argc < 5 || argc > 6) {
        fprintf(stderr, "Usage: %s a b N fileout [repetitions]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const double a = atof(argv[1]);
    const double b = atof(argv[2]);
    const int N = atoi(argv[3]);
    const char *fileout = argv[4];
    const int repetitions = (argc == 6) ? atoi(argv[5]) : 3;

    if (N <= 0 || repetitions <= 0) {
        fprintf(stderr, "Error: N and repetitions must be positive integers.\n");
        return EXIT_FAILURE;
    }

    const size_t nbytes = (size_t)N * (size_t)N * sizeof(double);
    const double mem_gb = (3.0 * (double)nbytes) / (1024.0 * 1024.0 * 1024.0);

    printf("Allocating A, B, C as contiguous arrays\n");
    printf("Approx memory for A, B, C: %.6f GB\n", mem_gb);

    double *A = alloc_matrix(N);
    double *B = alloc_matrix(N);
    double *C = alloc_matrix(N);
    double *Cref = alloc_matrix(N);
    double *Cbest = alloc_matrix(N);

    /* Homework initialization */
    fill_constant(A, N, a);
    fill_constant(B, N, b);

    Variant variants[] = {
        {"ijk", matmul_ijk, 0.0},
        {"ikj", matmul_ikj, 0.0},
        {"jik", matmul_jik, 0.0},
        {"jki", matmul_jki, 0.0},
        {"kij", matmul_kij, 0.0},
        {"kji", matmul_kji, 0.0}
    };
    const int nvariants = (int)(sizeof(variants) / sizeof(variants[0]));

    /* Trusted reference */
    matmul_ijk(A, B, Cref, N);

    int best_idx = -1;
    double best_time = 1e300;
    const double tol = 1e-9;

    printf("\nBenchmarking loop orderings (%d repetitions, best time kept)\n", repetitions);
    printf("------------------------------------------------------------\n");

    for (int v = 0; v < nvariants; ++v) {
        variants[v].best_time = benchmark_variant(variants[v].fn, A, B, C, N, repetitions);

        double maxdiff = 0.0;
        int mi = -1, mj = -1;
        int exact_ok = compare_matrices(C, Cref, N, tol, &maxdiff, &mi, &mj);
        int freivalds_ok = freivalds_check(A, B, C, N, FREIVALDS_ITERS, tol);

        printf("%s : %10.6f s   | exact check: %s   | Freivalds: %s\n",
               variants[v].name,
               variants[v].best_time,
               exact_ok ? "OK" : "FAIL",
               freivalds_ok ? "OK" : "FAIL");

        if (!exact_ok) {
            fprintf(stderr,
                    "Validation failed for ordering %s. Worst diff so far %.3e at (%d,%d)\n",
                    variants[v].name, maxdiff, mi, mj);
            free(A); free(B); free(C); free(Cref); free(Cbest);
            return EXIT_FAILURE;
        }

        if (!freivalds_ok) {
            fprintf(stderr, "Freivalds check failed for ordering %s\n", variants[v].name);
            free(A); free(B); free(C); free(Cref); free(Cbest);
            return EXIT_FAILURE;
        }

        if (variants[v].best_time < best_time) {
            best_time = variants[v].best_time;
            best_idx = v;
            memcpy(Cbest, C, nbytes);
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Fastest ordering: %s (best time = %.6f s)\n",
           variants[best_idx].name, variants[best_idx].best_time);

    if (!save_matrix_to_file(fileout, Cbest, N)) {
        free(A); free(B); free(C); free(Cref); free(Cbest);
        return EXIT_FAILURE;
    }

    printf("Matrix C saved to '%s'\n", fileout);

    if (N > 0) {
        printf("Sample result: C[0][0] = %.10g\n", Cbest[IDX(0,0,N)]);
        printf("For the specific homework initialization, expected C[0][0] = N*a*b = %.10g\n", (double)N * a * b);
    }

    free(A);
    free(B);
    free(C);
    free(Cref);
    free(Cbest);
    return EXIT_SUCCESS;
}
