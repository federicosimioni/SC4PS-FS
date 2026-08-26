# HW07 — Random Number Generators
Solutions to the five exercises assigned at the end of the lecture 7
notebook. As requested by the assignment, every random-number generator
and every exercise's numerical core is written in **C**; all plots are
made in **Python** (NumPy + Matplotlib). Everything is orchestrated by
a single `Makefile`.

## Structure

```
hw07_rng/
├── Makefile
├── src/
│   ├── lcg.h                       shared LCG header (x_{n+1} = a x_n + c mod 2^32)
│   ├── ex1_coin_tosses.c           Exercise 1: coin tosses / LLN
│   ├── ex2_mc_pi.c                 Exercise 2: Monte Carlo estimate of pi
│   ├── ex3_change_of_variables.c   Exercise 3: Y = U^2
│   ├── ex4_exponential.c           Exercise 4: inverse-transform exponential
│   └── ex5_empirical_cdf.c         Exercise 5: empirical CDF
├── plots/
│   ├── style.py                    shared Matplotlib style/paths
│   ├── plot_ex1.py ... plot_ex5.py one script per exercise
├── data/                           *.dat files produced by the C programs (generated)
└── figures/                        *.png figures produced by the Python scripts (generated)
```

## How to run

```
make            # default target: full pipeline (compile + run + generate plots)
make compile    # compile the five C programs into build/
make run        # compile + execute them, writing data/*.dat
make plots      # compile + run + generate figures/*.png
make clean      # remove build/, data/, figures/
```

The default target is `all`, which depends on `plots`. Consequently,
invoking `make` without arguments triggers the full workflow: compile the
C programs, execute them to generate the corresponding `.dat` files,
and then run the Python scripts to create the figures. In other words,
`make` is equivalent to `make all`, and the complete pipeline is:

```bash
mkdir -p build
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex1_coin_tosses src/ex1_coin_tosses.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex2_mc_pi src/ex2_mc_pi.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex3_change_of_variables src/ex3_change_of_variables.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex4_exponential src/ex4_exponential.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex5_empirical_cdf src/ex5_empirical_cdf.c -lm

mkdir -p data
build/ex1_coin_tosses 100000 > data/ex1_coin_tosses.dat
build/ex2_mc_pi 20 > data/ex2_mc_pi.dat
build/ex3_change_of_variables 200000 > data/ex3_change_of_variables.dat
build/ex4_exponential 200000 1.5 > data/ex4_exponential.dat
build/ex5_empirical_cdf 50000 1.5 > data/ex5_empirical_cdf.dat

mkdir -p figures
python3 plots/plot_ex1.py
python3 plots/plot_ex2.py
python3 plots/plot_ex3.py
python3 plots/plot_ex4.py
python3 plots/plot_ex5.py
```

This reproduces the entire homework from scratch: five standalone C
executables, one per exercise, sharing a common LCG implementation,
followed by five Python scripts that read the generated `.dat` files and
produce the corresponding figures.

## Representative output of `make`

A typical execution log produced by the default target is shown below.
This illustrates the complete workflow implemented by the Makefile: the
five C programs are compiled, executed, and the resulting datasets are
transformed into figures by the Python plotting scripts.

```bash
mkdir -p build
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex1_coin_tosses src/ex1_coin_tosses.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex2_mc_pi src/ex2_mc_pi.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex3_change_of_variables src/ex3_change_of_variables.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex4_exponential src/ex4_exponential.c -lm
gcc -O2 -Wall -Wextra -std=c11 -Isrc -o build/ex5_empirical_cdf src/ex5_empirical_cdf.c -lm
mkdir -p data
build/ex1_coin_tosses            100000            > data/ex1_coin_tosses.dat
  n =       10   fraction of heads = 0.400000   |deviation from 0.5| = 0.100000
  n =      100   fraction of heads = 0.490000   |deviation from 0.5| = 0.010000
  n =     1000   fraction of heads = 0.520000   |deviation from 0.5| = 0.020000
  n =    10000   fraction of heads = 0.506900   |deviation from 0.5| = 0.006900
  n =   100000   fraction of heads = 0.498840   |deviation from 0.5| = 0.001160
Final fraction of heads after 100000 tosses: 0.498840
build/ex2_mc_pi                  20                > data/ex2_mc_pi.dat
Estimated pi at 20 repetitions per sample size.
build/ex3_change_of_variables    200000            > data/ex3_change_of_variables.dat
Max |F_emp - F_exact| over a 20-point grid: 0.00306 (n=200000)
build/ex4_exponential            200000     1.5    > data/ex4_exponential.dat
Sample mean = 0.667898  (theory: 1/lambda = 0.666667)
Sample var  = 0.446021  (theory: 1/lambda^2 = 0.444444)
build/ex5_empirical_cdf          50000      1.5    > data/ex5_empirical_cdf.dat
Kolmogorov-Smirnov statistic D_n = 0.004284 (n=50000, sqrt(n)*D_n = 0.9579)
mkdir -p figures
python3 plots/plot_ex1.py
Saved figures/ex1_coin_tosses.png
python3 plots/plot_ex2.py
Saved figures/ex2_mc_pi.png
  N=     100  mean_pi_estimate=3.170000  mean_abs_error=0.161363
  N=    1000  mean_pi_estimate=3.122400  mean_abs_error=0.046237
  N=   10000  mean_pi_estimate=3.144860  mean_abs_error=0.009579
  N=  100000  mean_pi_estimate=3.141382  mean_abs_error=0.003313
  N= 1000000  mean_pi_estimate=3.141234  mean_abs_error=0.001795
python3 plots/plot_ex3.py
Saved figures/ex3_change_of_variables.png
python3 plots/plot_ex4.py
Saved figures/ex4_exponential.png
  sample mean = 0.667898  (theory 1/lambda = 0.666667)
  sample var  = 0.446021  (theory 1/lambda^2 = 0.444444)
python3 plots/plot_ex5.py
Saved figures/ex5_empirical_cdf.png
  Kolmogorov-Smirnov statistic D_n = 0.004284
```

These values are consistent with the expected numerical behavior of the
five exercises: the coin-toss fraction approaches 0.5 as the sample size
increases, the Monte Carlo estimate of $\pi$ improves with the inverse
square-root scaling in the number of samples, the transformed variable
$Y = U^2$ follows the expected density, the exponential sampling matches
the theoretical mean and variance, and the empirical CDF remains close to
its exact counterpart within the expected KS deviation.

## Comments on the results

- **Exercise 1**: the running fraction of heads fluctuates widely for
  small `n` and settles inside a shrinking band around 0.5 as `n`
  grows, consistent with the `1/sqrt(n)` shrinkage of typical
  fluctuations predicted by the LLN/CLT.
- **Exercise 2**: the mean absolute error decreases essentially like
  `N^{-1/2}`, as expected for a Monte Carlo estimator (the reference
  dashed line and the data track each other closely on the log-log
  plot).
- **Exercise 3**: the histogram of `Y = U^2` matches the diverging
  density `1/(2 sqrt(y))` near `y = 0`, exactly as predicted by the
  change-of-variables formula.
- **Exercise 4**: the histogram matches the exponential density
  `lambda * exp(-lambda y)` closely, and the sample mean/variance are
  within Monte Carlo noise of the theoretical `1/lambda`, `1/lambda^2`.
- **Exercise 5**: the empirical CDF tracks the exact exponential CDF
  everywhere, with a maximum Kolmogorov–Smirnov gap of order
  `1/sqrt(n)`, as expected for a correctly-sampled distribution.
