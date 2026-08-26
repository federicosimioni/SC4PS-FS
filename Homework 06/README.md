# HomeWork 06 – Split the work

C solution that builds on the daxpy from HomeWork 02 (`d = a*x + y`, taking into consideration also the same values for a, x and y) and:

1. splits the computation into **chunks** (outer loop over chunks, inner
   loop over the elements of the chunk) and checks that the result is
   identical to the original single-loop code;
2. computes, for every chunk, a partial sum (`partial_chunk_sum`) and
   checks that the sum of all partial sums matches the sum of `d` computed
   the original way;
3. reads all the parameters from a text configuration file in the format
   `Variable = Value`;
4. saves the chunks of `d` and the partial sums into an **HDF5** file.

## Files

| File                 | Content                                              |
|----------------------|-------------------------------------------------------|
| `daxpy_chunked.c`    | Main program                                           |
| `config_parser.h/.c` | Parser for the `Variable = Value` configuration file   |
| `config.txt`         | Example configuration file                             |
| `Makefile`           | Build with `h5cc`, with `pkg-config` as a fallback    |
| `verify_hdf5.py`     | Verifies the structure and contents of `output.h5`     |

## Building

You need the HDF5 development library (package `libhdf5-dev` on
Debian/Ubuntu, or the `hdf5` module on an HPC cluster). The Makefile
automatically selects the available HDF5 compilation method. If the
`h5cc` compiler wrapper is installed, it is used as the preferred
compiler because it already knows the correct HDF5 header and library
paths. Then run:

```bash
make
```

When `h5cc` is available, the command executed by `make` is conceptually
equivalent to:

```bash
h5cc -O2 -Wall -Wextra -std=c11 -o daxpy_chunked \
   daxpy_chunked.c config_parser.c -lm
```

The `h5cc` wrapper adds the HDF5 include and library options internally.
If `h5cc` is not available, the Makefile falls back to `gcc` and first
tries to obtain those options through `pkg-config`:

```bash
gcc -O2 -Wall -Wextra -std=c11 -o daxpy_chunked \
   daxpy_chunked.c config_parser.c \
   $(pkg-config --cflags --libs hdf5) -lm
```

If `pkg-config` cannot find `hdf5.pc`, compile by specifying the paths
manually, for example:

```bash
gcc -O2 -Wall -Wextra -std=c11 -o daxpy_chunked \
   daxpy_chunked.c config_parser.c \
    -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
   -lhdf5_serial -lm
```

The exact paths and library name depend on the distribution and HDF5
installation. You can inspect the compilation strategy selected by the
Makefile with:

```bash
make info
```

Compilation and execution are separate steps. The command `make` creates
the `daxpy_chunked` executable but does not run it. To execute the program,
run:

```bash
./daxpy_chunked config.txt
```

Alternatively, `make run` performs both steps in sequence.

## Troubleshooting common build issues

**`fatal error: hdf5.h: No such file or directory`**
This means the HDF5 development library is not installed (or not on the
compiler's default search paths). On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install libhdf5-dev
```

On Ubuntu this package often installs the headers in a "non-standard"
location (`/usr/include/hdf5/serial/`) and the library is named
`libhdf5_serial` instead of `libhdf5`: because of this, plain
`gcc ... -lhdf5` can keep failing even after installation. The `Makefile`
here handles this case automatically: if the `h5cc` command (the compiler
wrapper installed alongside HDF5, which always knows the correct paths) is
present on the system, it is used instead of calling `gcc` directly. You
can check what `make` detected with:

```bash
make info
```

If `h5cc` is not available, check where the headers/libraries ended up
with:

```bash
dpkg -L libhdf5-dev | grep hdf5.h
dpkg -L libhdf5-dev | grep -E '\.so$'
```

and compile by hand pointing to the paths you found, e.g.:

```bash
gcc -O2 -o daxpy_chunked daxpy_chunked.c config_parser.c \
    -I/usr/include/hdf5/serial \
    -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
    -lhdf5_serial -lm
```

## Configuration file (`config.txt`)

```
n = 1000000
chunk_size = 8
a = 3.0
x = 0.1
y = 7.1
output_file = output.h5
```

- `n`: length of the vectors (try 10, 10^6, 10^8)
- `chunk_size`: chunk size
- `a`, `x`, `y`: the scalar `a`, and the constant values of `x` and `y`
  (every element of `x` equals `x`, every element of `y` equals `y`, as
  required in HomeWork 02)
- `output_file`: name of the output HDF5 file

The parser (`config_parser.c`) is written "from scratch" to avoid depending
on external libraries to download; it is still organized around a single
`Config` struct, so it is easy to replace with a ready-made library (e.g.
the one suggested in the assignment,
https://github.com/welljsjs/Config-Parser-C) by changing only
`config_parser.c`.

## Running

```bash
./daxpy_chunked config.txt
```

The program prints to screen:

1. the element-by-element check between the chunked result and the
   expected value `a*x+y` (equivalent to the original code, since here `x`
   and `y` are constant);
2. the comparison between the "original" sum (single loop over `d`) and
   the sum obtained by adding up all the `partial_chunk_sum` values;
3. the explicit test required in HomeWork 02 point 4 (`a=3, x=0.1, y=7.1`,
   expected `a*x+y=7.4`).

To inspect the generated HDF5 file with Python, install `h5py` and `numpy`
and run:

```bash
python3 -m pip install h5py numpy
python3 verify_hdf5.py
```

The script checks the two datasets, their dimensions and HDF5 layout, all
root attributes, the expected DAXPY values, the per-chunk sums, and the
consistency of the stored summary attributes. A different HDF5 file can be
passed as an optional argument:

```bash
python3 verify_hdf5.py path/to/file.h5
```

### Example execution

The following output was obtained by running `make run` with the example
configuration above:

```text
h5cc -O2 -Wall -Wextra -std=c11  -o daxpy_chunked daxpy_chunked.c config_parser.c  -lm
./daxpy_chunked config.txt
Configuration read:
   n           = 1000000
   chunk_size  = 8
   a           = 3
   x           = 0.1
   y           = 7.1
   output_file = output.h5
n = 1000000, chunk_size = 8 -> number_of_chunks = 125000

[Step 1] Element-by-element check against the original code
   expected value a*x+y         = 7.4000000000000004
   max deviation |d[i]-expected| = 8.882e-16
   -> d is identical to the result of the original code.

[Step 2] Sum check
   original sum (single loop)             = 7400000.0001241192
   sum from chunks (sum partial_chunk_sum) = 7400000.0000155205
   absolute difference                     = 1.086e-04 (tolerance 7.400e-03)
   -> the two sums match (up to rounding).

[HW02 pt.4 test] a=3, x=0.1, y=7.1 -> a*x+y = 7.3999999999999995 (expected 7.4, error 8.882e-16)
   -> the test passes (within the tolerance due to floating point representation).

Results saved to 'output.h5' (dataset '/d' with HDF5 chunk = 8, dataset '/partial_chunk_sum').
```

This run confirms that the chunked computation reproduces the element-wise
result of the original implementation to within machine precision. The two
sum values differ slightly because floating-point addition is not associative:
the single-loop sum and the sum of chunk partial sums use different addition
orders. The observed difference (`1.086e-04`) remains well below the relative
tolerance (`7.400e-03`), so the sum check succeeds. Finally, the explicit
HomeWork 02 test passes within tolerance, as decimal values such as `0.1`,
`7.1`, and `7.4` are generally not represented exactly in binary
double-precision arithmetic. The final line confirms that both the chunked
dataset `/d` and the per-chunk sums were written to `output.h5`.

## Structure of the produced HDF5 file

```
output.h5
├── /d                    dataset (n,) double, CHUNKED layout
│                         (HDF5 chunk dimension = chunk_size)
├── /partial_chunk_sum    dataset (number_of_chunks,) double
└── file attributes: n, chunk_size, number_of_chunks, a, x, y,
                      total_sum, max_abs_diff_vs_original
```

The `/d` dataset is created with `H5Pset_chunk` using **`chunk_size`
itself** as the HDF5 chunk dimension, and every chunk computed in memory is
immediately written with `H5Sselect_hyperslab` + `H5Dwrite` to the
corresponding portion of the file. This way the "logical" chunking of the
computation and the "physical" chunking of the HDF5 storage coincide: it's
the same idea, applied both to the computation and to the on-disk storage.

## Observations

- For very large `N` (e.g. 10^8), the `d` vector alone takes about 800 MB
  of RAM (`N * sizeof(double)`); with `N` = 10^6 or 10^8 and a small
  `chunk_size` (e.g. 8), the number of chunks is huge (10^6/8 ≈ 125,000,
  10^8/8 = 12.5 million): this is not a problem for the computation itself
  (the inner loop always does very little work), but it **would have
  been** a problem if we had chosen to save each chunk as a separate HDF5
  group (`/chunks/chunk_0`, `/chunks/chunk_1`, ...): with millions of
  groups/datasets, HDF5's metadata overhead becomes enormous and write
  performance collapses. This is why I chose to use **a single dataset
  with HDF5's native chunked layout**, which internally handles millions
  of chunks efficiently.
- A second effect, not really an "error" but still worth noting: **the sum
  computed with a single sequential loop and the sum obtained by adding up
  the chunk partial sums are not bit-for-bit identical** for large N.
  Floating-point addition is not associative, so changing the order of the
  additions (as happens when you accumulate first inside each chunk and
  then sum the partial results) produces a slightly different result, on
  the order of 10^-4–10^-11 relative, depending on N. This is why the
  comparison between the two sums in the code uses a **relative
  tolerance** instead of exact equality.

