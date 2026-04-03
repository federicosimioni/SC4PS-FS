# Homework 03 — Matrix Multiplication

## 📌 Overview

This program implements and benchmarks different loop orderings for matrix multiplication in C. It also includes correctness checks using both exact comparison and a probabilistic algorithm (Freivalds’ algorithm).

The main goals are:

- Understand how loop ordering affects performance
- Use contiguous memory for matrices
- Validate correctness efficiently
- Benchmark different implementations

---

## ⚙️ How the Program Works

### 1. Input Parameters

The program expects:

./program a b N fileout [repetitions]

- a: constant value for matrix A  
- b: constant value for matrix B  
- N: matrix size (NxN)  
- fileout: output file for matrix C  
- repetitions (optional): number of repetitions for benchmarking (default = 3)  

---

### 2. Memory Allocation

Matrices are allocated as contiguous 1D arrays.

Indexing is handled via:

IDX(i,j,N) = i * N + j

This ensures:
- better cache locality  
- simpler memory layout  
- higher performance  

---

### 3. Initialization

Matrices are filled with constants:

A[i][j] = a  
B[i][j] = b  

This implies that the result matrix has a known analytical solution:

C[i][j] = N * a * b  

This is used as a sanity check in the end.

---

### 4. Matrix Multiplication Variants

The program implements all 6 loop permutations:

- ijk
- ikj
- jik
- jki
- kij
- kji

---

### 5. Benchmarking

Each variant is executed multiple times, and the best time is kept.

---

## ✅ Correctness Checks

### 1. Exact Comparison

- Compares result with a reference implementation (ijk)  
- Uses a tolerance (1e-9)  

Pros:
- Deterministic  
- Fully reliable  

Cons:
- Cost is O(N^3)  (requires computing the full reference matrix)  

---

### 2. Freivalds’ Algorithm

A probabilistic algorithm to verify:

C = A * B

Idea:
- Generate random vector r  
- Check if A(Br) = Cr  

Pros:
- Complexity: O(N^2)  
- Faster than full multiplication  

Cons:
- Small probability of false positives  
- Reduced by repeating the test  

---

### Detailed Comparison of Correctness Checks

Their actual computational costs differ significantly due to the operations involved:

- **Exact Comparison**:
  - Computes the full result matrix C using the tested loop ordering (e.g., ikj): O(N^3).
  - Computes a reference matrix C_ref using a reliable implementation (ijk): O(N^3).
  - Compares C and C_ref element-by-element with a tolerance (e.g., 1e-9): O(N^2).
  - **Total cost: O(N^3)** (dominated by matrix multiplications). This makes it reliable but slow for large N.

- **Freivalds’ Algorithm**:
  - Generates a random vector r of size N.
  - Computes B * r (matrix-vector multiplication): O(N^2).
  - Computes A * (B * r) (another matrix-vector multiplication): O(N^2).
  - Computes C * r (matrix-vector multiplication on the result): O(N^2).
  - Compares the two resulting vectors with tolerance: O(N).
  - **Total cost: O(N^2)**. No full matrix multiplication is needed, making it much faster. The probabilistic nature means there's a tiny chance of false positives (e.g., ~1/2^k for k repetitions), but it's negligible in practice.

In summary, Exact Comparison is deterministic and exact but expensive (O(N^3)), while Freivalds is probabilistic but efficient (O(N^2)), ideal for quick validations in performance-critical scenarios.

---

## 🚀 How to Compile and Run

### Compilation

To compile the program you use the following GCC command:

```
gcc -O3 -std=c11 -Wall -Wextra -pedantic homework03_matmul_genericcheck.c -o homework03
```

Here is a detailed explanation of each element of the command:

- **gcc**: The GNU compiler for the C language. It is used to transform source code (.c) into an executable.
- **-O3**: Enables optimization level 3 (the highest available). This optimizes the code for high performance (execution speed), but may increase compilation time and binary size. It is useful for programs requiring efficiency, such as compute-intensive algorithms (e.g., matrix multiplication).
- **-std=c11**: Specifies the C11 standard (ISO/IEC 9899:2011). Ensures the compiler uses the rules and features of this standard, avoiding non-standard or deprecated behaviors. Recommended for modern and portable code.
- **-Wall**: Enables all common compiler warnings. This helps identify potential bugs, unsafe code, or poor practices, such as uninitialized variables or problematic comparisons.
- **-Wextra**: Adds additional warnings beyond those from -Wall. Catches more subtle issues, such as unused variables or implicit conversions that might cause data loss.
- **-pedantic**: Forces the compiler to strictly adhere to the specified C standard (-std=c11). Disables non-standard extensions and reports errors for non-conforming code, improving portability across different compilers.
- **homework03_matmul_genericcheck.c**: The C source file to compile. It contains the program code (likely implementing matrix multiplication with generic checks).
- **-o homework03**: Specifies the name of the output executable file (without extension on Unix-like systems, but executable on Windows if configured). Without this, GCC would use the default name "a.out".

This command is recommended to produce an optimized, safe, and standards-compliant executable, reducing the risk of runtime errors or portability issues.

### Execution

./homework03 2 3 512 output.txt 5

---

## 📊 Example Output
```
Allocating A, B, C as contiguous arrays
Approx memory for A, B, C: 0.005859 GB

Benchmarking loop orderings (5 repetitions, best time kept)
------------------------------------------------------------
ijk :   1.026426 s   | exact check: OK   | Freivalds: OK
ikj :   0.028248 s   | exact check: OK   | Freivalds: OK
jik :   1.043534 s   | exact check: OK   | Freivalds: OK
jki :   2.157110 s   | exact check: OK   | Freivalds: OK
kij :   0.032131 s   | exact check: OK   | Freivalds: OK
kji :   2.183589 s   | exact check: OK   | Freivalds: OK
------------------------------------------------------------
Fastest ordering: ikj (best time = 0.028248 s)
Matrix C saved to 'output.txt'
Sample result: C[0][0] = 3072
For the specific homework initialization, expected C[0][0] = N*a*b = 3072
```
---

## ⚡ Loop Ordering Analysis

### Fastest: ikj and kij

- Sequential memory access  
- Better cache locality  

### Slowest: jki and kji

- Poor memory access pattern  
- Cache misses  

Key insight:

Performance is dominated by memory access patterns.

---

## 🧠 Conclusions

- Loop ordering has a huge impact on performance  
- Cache locality is crucial  
- Freivalds’ algorithm is efficient for validation  
- Contiguous memory improves performance  

---

## 📁 Output

The program writes matrix C to:

output.txt

And, specifically for this Homework, it verifies:

C[0][0] = N * a * b
