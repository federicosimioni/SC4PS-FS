#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Uso: %s N a x y\n", argv[0]);
        printf("Esempio: %s 10 3 0.1 7.1\n", argv[0]);
        return 1;
    }

    /* Read parameters from command line */
    long long N = atoll(argv[1]);
    double a = atof(argv[2]);
    double x_value = atof(argv[3]);
    double y_value = atof(argv[4]);

    if (N <= 0) {
        printf("Errore: N deve essere positivo.\n");
        return 1;
    }

    /* Dynamically allocate arrays */
    double *x = (double *)malloc(N * sizeof(double));
    double *y = (double *)malloc(N * sizeof(double));
    double *d = (double *)malloc(N * sizeof(double));

    if (x == NULL || y == NULL || d == NULL) {
        printf("Errore: memoria non allocata. N troppo grande?\n");
        free(x);
        free(y);
        free(d);
        return 1;
    }

    /* Initialize arrays */
    for (long long i = 0; i < N; i++) {
        x[i] = x_value;
        y[i] = y_value;
    }

    /* Compute expected value */
    double expected = a * x_value + y_value;

    /* Compute vector d */
    for (long long i = 0; i < N; i++) {
        d[i] = a * x[i] + y[i];
    }

    /* Verify with tolerance */
    double tol = 1e-12;
    int ok = 1;

    for (long long i = 0; i < N; i++) {
        if (fabs(d[i] - expected) > tol) {
            ok = 0;
            printf("Errore al componente %lld: d[%lld] = %.17g, atteso = %.17g\n",
                   i, i, d[i], expected);
            break;
        }
    }

    /* Output */
    printf("N = %lld\n", N);
    printf("a = %.17g, x = %.17g, y = %.17g\n", a, x_value, y_value);
    printf("Valore atteso di ogni elemento di d: %.17g\n", expected);

    if (ok) {
        printf("Test superato: tutti gli elementi di d sono corretti entro tolleranza %.1e\n", tol);
    } else {
        printf("Test fallito.\n");
    }

    /* For small vectors, also print the elements */
    if (N <= 20) {
        printf("\nElementi di d:\n");
        for (long long i = 0; i < N; i++) {
            printf("d[%lld] = %.17g\n", i, d[i]);
        }
    }

    free(x);
    free(y);
    free(d);

    return 0;
}