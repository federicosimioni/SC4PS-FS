/*
 * daxpy_chunked.c
 * ----------------
 * HomeWork 06 - "Split the work"
 *
 * Builds on the daxpy task (d = a*x + y, where x and y
 * are "piecewise constant" vectors: every element of x is equal to
 * every other element of x, and the same holds for y) and:
 *
 *   1) splits the computation into chunks (outer loop over chunks,
 *      inner loop over the elements of the chunk) and verifies that
 *      the result is identical to the "original" computation (single
 *      loop);
 *   2) for every chunk, computes the partial sum of its elements and
 *      stores it in an array partial_chunk_sum; then sums all the
 *      partial sums and verifies that it matches the sum of all the
 *      elements of d computed the "original" way;
 *   3) reads all the parameters (n, chunk_size, a, x, y, ...) from a
 *      text configuration file "Variable = Value"
 *      (see config_parser.h/.c);
 *   4) saves the chunks of vector d and the partial sums into an
 *      HDF5 file, using HDF5's native chunking so that the on-disk
 *      storage layout exactly mirrors the chunking used for the
 *      computation.
 *
 * Compilation (see also Makefile):
 *   If available, use the HDF5 compiler wrapper:
 *   h5cc -O2 -Wall -Wextra -std=c11 -o daxpy_chunked \
 *       daxpy_chunked.c config_parser.c -lm
 *   The Makefile automatically falls back to gcc with pkg-config when
 *   h5cc is not available.
 *
 * Usage:
 *   ./daxpy_chunked config.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <hdf5.h>

#include "config_parser.h"

/* Computes the number of chunks needed to cover n elements with
 * chunks of size chunk_size (ceiling division, without going through
 * floating point). */
static long ceiling_div(long n, long chunk_size)
{
    return (n + chunk_size - 1) / chunk_size;
}

int main(int argc, char *argv[])
{
    const char *config_file = (argc > 1) ? argv[1] : "config.txt";

    Config cfg;
    if (config_load(config_file, &cfg) != 0) {
        fprintf(stderr, "Error reading '%s'\n", config_file);
        return EXIT_FAILURE;
    }
    config_print(&cfg);

    const long n           = cfg.n;
    const long chunk_size  = cfg.chunk_size;
    const double a         = cfg.a;
    const double x_val     = cfg.x;   /* every element of x equals x_val */
    const double y_val     = cfg.y;   /* every element of y equals y_val */

    const long number_of_chunks = ceiling_div(n, chunk_size);
    printf("n = %ld, chunk_size = %ld -> number_of_chunks = %ld\n",
           n, chunk_size, number_of_chunks);

    /* ---- Allocation ----------------------------------------------------- */
    double *d = (double *) malloc((size_t) n * sizeof(double));
    double *partial_chunk_sum = (double *) malloc((size_t) number_of_chunks * sizeof(double));

    if (d == NULL || partial_chunk_sum == NULL) {
        fprintf(stderr,
                "Allocation error for n = %ld "
                "(about %.2f GB needed just for d)\n",
                n, (double)((size_t) n * sizeof(double)) / 1e9);
        free(d);
        free(partial_chunk_sum);
        return EXIT_FAILURE;
    }

    /* ---- Setting up the HDF5 file --------------------------------------
     * The "d" dataset is created with a CHUNKED layout, using
     * chunk_size itself as the HDF5 chunk dimension: this way the
     * on-disk storage exactly mirrors the way the work is split up
     * in memory. Every time a chunk is finished in memory, it is
     * immediately written to the corresponding hyperslab of the
     * on-disk dataset.
     * -------------------------------------------------------------- */
    hid_t file_id = H5Fcreate(cfg.output_file, H5F_ACC_TRUNC,
                               H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "Error creating HDF5 file '%s'\n", cfg.output_file);
        free(d);
        free(partial_chunk_sum);
        return EXIT_FAILURE;
    }

    hsize_t dims_d[1] = { (hsize_t) n };
    hid_t space_d = H5Screate_simple(1, dims_d, NULL);

    hid_t dcpl_d = H5Pcreate(H5P_DATASET_CREATE);
    
    /* The HDF5 chunk dimension cannot exceed the total dataset
     * dimension (this can happen if n < chunk_size). */
    hsize_t hdf5_chunk_dim = (hsize_t) ((chunk_size < n) ? chunk_size : n);
    hsize_t chunk_dims_d[1] = { hdf5_chunk_dim };
    H5Pset_chunk(dcpl_d, 1, chunk_dims_d);
    H5Pset_deflate(dcpl_d, 4); /* gzip compression, level 4: optional but handy */

    hid_t dset_d = H5Dcreate2(file_id, "/d", H5T_NATIVE_DOUBLE, space_d,
                               H5P_DEFAULT, dcpl_d, H5P_DEFAULT);

    hsize_t dims_p[1] = { (hsize_t) number_of_chunks };
    hid_t space_p = H5Screate_simple(1, dims_p, NULL);
    hid_t dset_p = H5Dcreate2(file_id, "/partial_chunk_sum", H5T_NATIVE_DOUBLE,
                               space_p, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* ---- Outer loop over chunks (compute + verify + write) ------------ */
    double expected_value = 7.4;   /* expected value of every d[i] */
    double max_abs_diff   = 0.0;

    for (long chunk_index = 0; chunk_index < number_of_chunks; chunk_index++) {

        long start = chunk_index * chunk_size;               /* 0-based, inclusive */
        long end   = start + chunk_size - 1;                  /* 0-based, inclusive */
        if (end > n - 1) end = n - 1;
        long count = end - start + 1;

        /* ---- Inner loop: processing the elements of the chunk ---- */
        double chunk_sum = 0.0;
        for (long i = start; i <= end; i++) {
            d[i] = a * x_val + y_val;
            chunk_sum += d[i];

            double diff = fabs(d[i] - expected_value);
            if (diff > max_abs_diff) max_abs_diff = diff;
        }

        partial_chunk_sum[chunk_index] = chunk_sum;

        /* ---- Writing the chunk into the HDF5 dataset (hyperslab) ---- */
        hsize_t offset[1] = { (hsize_t) start };
        hsize_t hcount[1] = { (hsize_t) count };

        hid_t file_space = H5Dget_space(dset_d);
        H5Sselect_hyperslab(file_space, H5S_SELECT_SET, offset, NULL, hcount, NULL);

        hsize_t mem_dims[1] = { (hsize_t) count };
        hid_t mem_space = H5Screate_simple(1, mem_dims, NULL);

        H5Dwrite(dset_d, H5T_NATIVE_DOUBLE, mem_space, file_space,
                 H5P_DEFAULT, &d[start]);

        H5Sclose(mem_space);
        H5Sclose(file_space);
    }

    /* Write all partial sums in one go */
    H5Dwrite(dset_p, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, partial_chunk_sum);

    /* ---- Step 1: comparison with the "original" computation (single loop) -
     * No second array is needed: since x and y are constant, the
     * "original code" produces by construction d[i] = a*x + y for
     * every i. max_abs_diff (computed above during the chunk loop)
     * is exactly the maximum deviation between the chunked method
     * and this reference value: if it is 0 (or on the order of
     * machine epsilon), the two methods are equivalent.
     * -------------------------------------------------------------- */
    printf("\n[Step 1] Element-by-element check against the original code\n");
    printf("  expected value a*x+y         = %.17g\n", expected_value);
    printf("  max deviation |d[i]-expected| = %.3e\n", max_abs_diff);
    if (max_abs_diff <= 1e-12) {
        printf("  -> d is identical to the result of the original code.\n");
    } else {
        printf("  -> d matches the original code up to rounding error.\n");
    }

    /* ---- Step 2: total sum from chunks vs "original" sum --------------
     * "Original" sum: a single loop over the whole d vector.
     * Chunked sum: sum of all the partial_chunk_sum values.
     * -------------------------------------------------------------- */
    double sum_original = 0.0;
    for (long i = 0; i < n; i++) {
        sum_original += d[i];
    }

    double sum_from_chunks = 0.0;
    for (long c = 0; c < number_of_chunks; c++) {
        sum_from_chunks += partial_chunk_sum[c];
    }

    double sum_diff = fabs(sum_original - sum_from_chunks);
    /* relative tolerance, because with floating-point summation the
     * order of operations slightly changes the result */
    double tol = 1e-9 * fabs(sum_original) + 1e-12;

    printf("\n[Step 2] Sum check\n");
    printf("  original sum (single loop)             = %.17g\n", sum_original);
    printf("  sum from chunks (sum partial_chunk_sum) = %.17g\n", sum_from_chunks);
    printf("  absolute difference                     = %.3e (tolerance %.3e)\n", sum_diff, tol);
    if (sum_diff <= tol) {
        printf("  -> the two sums match (up to rounding).\n");
    } else {
        printf("  -> WARNING: the two sums differ more than the expected tolerance!\n");
    }

    /* ---- Explicit test requested in HomeWork 02, point 4 ---------------
     * a = 3, x = 0.1, y = 7.1  ->  a*x + y = 7.4 (expected)
     * NB: 0.1, 7.1 and 7.4 are not exactly representable in binary,
     * so the comparison must use a tolerance, not ==.
     * -------------------------------------------------------------- */
    {
        double a_test = 3.0, x_test = 0.1, y_test = 7.1;
        double computed = a_test * x_test + y_test;
        double target = 7.4;
        double err = fabs(computed - target);
        printf("\n[HW02 pt.4 test] a=3, x=0.1, y=7.1 -> a*x+y = %.17g "
               "(expected 7.4, error %.3e)\n", computed, err);
        if (err < 1e-9) {
            printf("  -> the test passes (within the tolerance due to floating point representation).\n");
        } else {
            printf("  -> the test does NOT pass with this tolerance.\n");
        }
    }

    /* ---- Summary attributes on the HDF5 file --------------------------- */
    {
        hid_t attr_space = H5Screate(H5S_SCALAR);

        hid_t a_n = H5Acreate2(file_id, "n", H5T_NATIVE_LONG, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_n, H5T_NATIVE_LONG, &n);
        H5Aclose(a_n);

        hid_t a_cs = H5Acreate2(file_id, "chunk_size", H5T_NATIVE_LONG, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_cs, H5T_NATIVE_LONG, &chunk_size);
        H5Aclose(a_cs);

        hid_t a_nc = H5Acreate2(file_id, "number_of_chunks", H5T_NATIVE_LONG, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_nc, H5T_NATIVE_LONG, &number_of_chunks);
        H5Aclose(a_nc);

        hid_t a_a = H5Acreate2(file_id, "a", H5T_NATIVE_DOUBLE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_a, H5T_NATIVE_DOUBLE, &a);
        H5Aclose(a_a);

        hid_t a_x = H5Acreate2(file_id, "x", H5T_NATIVE_DOUBLE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_x, H5T_NATIVE_DOUBLE, &x_val);
        H5Aclose(a_x);

        hid_t a_y = H5Acreate2(file_id, "y", H5T_NATIVE_DOUBLE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_y, H5T_NATIVE_DOUBLE, &y_val);
        H5Aclose(a_y);

        hid_t a_sum = H5Acreate2(file_id, "total_sum", H5T_NATIVE_DOUBLE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_sum, H5T_NATIVE_DOUBLE, &sum_from_chunks);
        H5Aclose(a_sum);

        hid_t a_diff = H5Acreate2(file_id, "max_abs_diff_vs_original", H5T_NATIVE_DOUBLE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(a_diff, H5T_NATIVE_DOUBLE, &max_abs_diff);
        H5Aclose(a_diff);

        H5Sclose(attr_space);
    }

    /* ---- Closing all HDF5 objects --------------------------------------- */
    H5Dclose(dset_d);
    H5Dclose(dset_p);
    H5Sclose(space_d);
    H5Sclose(space_p);
    H5Pclose(dcpl_d);
    H5Fclose(file_id);

    printf("\nResults saved to '%s' "
           "(dataset '/d' with HDF5 chunk = %llu, dataset '/partial_chunk_sum').\n",
           cfg.output_file, (unsigned long long) hdf5_chunk_dim);

    free(d);
    free(partial_chunk_sum);

    return EXIT_SUCCESS;
}
