# Homework 02 – Vector Operation in C

## 📌 Overview

This homework implements a simple numerical computing task in C, which can be considered the “Hello World” of linear algebra:
$$
d = a \cdot x + y
$$
Where:

* ( a ) is a scalar
* ( x, y, d ) are vectors of size ( N )

All elements of ( x ) and ( y ) are constant and provided via command line.

---

## ⚙️ Compilation

To compile the program, use:

```bash
gcc Homework02.c -o homework02
```

Explanation:

* `gcc`: GNU C compiler
* `-o homework02`: name of the output executable

---

## ▶️ Execution

Run the program as follows:

```bash
./homework02 N a x y
```

Example:

```bash
./homework02 10 3 0.1 7.1
```

Arguments:

* `N`: size of the vectors
* `a`: scalar value
* `x`: value for all elements of vector ( x )
* `y`: value for all elements of vector ( y )

---

## 📊 Program Behavior

The program:

1. Allocates three vectors (`x`, `y`, `d`) of size ( N )
2. Initializes:

   * ( x[i] = x_value )
   * ( y[i] = y_value )
3. Computes:
   $$
   d[i] = a \cdot x[i] + y[i]
   $$
4. Computes the expected value:
   $$
   expected = a \cdot x_value + y_value
   $$
5. Verifies that all elements of ( d ) match `expected` within a tolerance

---

## ✅ Correctness Test

The verification is done using:

```c
fabs(d[i] - expected) > tol
```

with:

```c
tol = 1e-12
```

This avoids direct comparison between floating point numbers.

---

## 📌 Results for Different Values of N

### N = 10

* Execution is instantaneous
* Memory usage is negligible
* All tests pass correctly

---

### N = 10⁶

* Execution is still fast
* Memory usage: $$ 3 \times 10^6 \times 8 \text{ bytes} = 24 \text{ MB}$$
* Fully manageable on a standard machine

---

### N = 10⁸

* Memory usage becomes critical:$$  3 \times 10^8 \times 8 = 2.4 \text{ GB}$$
* Possible outcomes:

  * `malloc` fails → program prints error
  * system slowdown due to memory pressure
* This is the main limitation of the current implementation

---

## ❗ Issues Observed

### 1. Memory Limitations

The program allocates three large arrays:

* `x`
* `y`
* `d`

For very large ( N ), this becomes impractical. In my specific case problems arise from N = $10^{9}$.

👉 The implementation is **not memory efficient**, especially since:

* all elements of `x` are identical
* all elements of `y` are identical

---

### 2. Floating Point Precision

Test case:

```bash
a = 3, x = 0.1, y = 7.1
```

Mathematically:

$$
3 \cdot 0.1 + 7.1 = 7.4
$$

However, in floating point arithmetic:

* `0.1` is not exactly representable in binary
* results may be:

  * `7.3999999999999995`
  * `7.4000000000000004`

Therefore:

* comparing with `==` is unreliable
* using a tolerance is necessary

---

## 🔍 Answer to Homework Questions

### Did you find any problems for some values of N?

Yes.

* For small values (`N = 10`, `N = 10^6`), the program works correctly.
* For large values (`N = 10^9`), the program fails due to insufficient memory.

This is because the program allocates approximately **24 GB of RAM**, which exceed system limits.

---

### Were you able to correctly test point 4?

Yes, but with an important consideration.

* The expression `a * x + y` is mathematically equal to `7.4`
* However, due to floating point representation, the computed result is only an approximation

In this implementation:

* `expected` and `d[i]` are computed using the same expression
* Therefore, they are likely to match within tolerance

However:

* comparing directly to the literal `7.4` may fail
* exact equality (`==`) should be avoided

---
