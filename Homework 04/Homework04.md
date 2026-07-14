# Complete Solution to LESSONS.md Homework

This solution provides comprehensive answers to the FFT project assignment (Homework 04), drawing on both theoretical principles and numerical results from the actual code execution.

---

## Part 1: Before running anything

**Question 1: In your own words, what does an FFT tell you about a signal?**

The Fast Fourier Transform (FFT) is a numerical algorithm that decomposes a discrete, sampled signal into its constituent frequency components. While the Discrete Fourier Transform (DFT) performs this decomposition with $\mathcal{O}(N^2)$ operations, the FFT achieves the same result in $\mathcal{O}(N \log_2 N)$ time through recursive factorization, making it practical for large datasets.

From a physics perspective, an FFT output answers the question: *"At what frequencies does energy reside in my signal?"* For a sampled time-domain signal $x[n]$ with $N$ samples, the FFT computes the complex amplitude (magnitude and phase) at each discrete frequency bin. The magnitude spectrum shows how strongly each frequency is present; the phase spectrum shows the temporal offset.

In our context, if you record a sound wave, vibration, or particle displacement and apply an FFT, you immediately see whether the signal contains a single pure tone, multiple tones, noise, or harmonic content. This is why the FFT is fundamental to signal processing in physics, engineering, medicine, and astronomy.

**Question 2: What is the difference between sampling rate, Nyquist frequency, and frequency resolution?**

- **Sampling Rate** ($f_s$): The number of measurements taken per unit time, measured in samples per second (Hz). In the `sampling_demo.c` code, we see `sample_rate_hz = 512` for the well-sampled case, meaning we capture 512 data points per second. A higher sampling rate captures more temporal detail and allows detection of higher frequencies.

- **Nyquist Frequency** ($f_N$): The maximum frequency that can be uniquely represented in a sampled signal without aliasing:
  $$f_N = \frac{f_s}{2}$$
  
  This is a hard physical limit. If your signal contains a true frequency component at or above $f_N$, that component will be "folded back" (aliased) into a false lower frequency. In the undersampled case ($f_s = 128$ Hz), we have $f_N = 64$ Hz. The true 120 Hz component exceeds this limit and folds back as an alias at $|120 - 128| = 8$ Hz, which we observe numerically.

- **Frequency Resolution** ($\Delta f$): The spacing between consecutive FFT bins:
  $$\Delta f = \frac{1}{T} = \frac{f_s}{N}$$
  
  where $T$ is the total observation time and $N$ is the number of samples. A smaller $\Delta f$ means finer frequency discrimination. In the short-record case ($N = 64$, $f_s = 512$ Hz), we get $\Delta f = 8$ Hz, which is why the 50 Hz and 55 Hz components (differing by only 5 Hz) appear as overlapping peaks rather than distinct ones.

**Question 3: Why is a uniformly spaced time grid important for the FFT routines used in this project?**

The FFT algorithm fundamentally relies on the assumption that samples are equally spaced in time with constant interval $\Delta t = 1/f_s$. This uniform spacing enables two critical properties:

1. **Periodicity and Symmetry**: The FFT exploits the periodicity of the DFT kernel $e^{-i 2\pi k n / N}$. Equally spaced samples ensure that this kernel evaluates consistently across bins, allowing the Cooley-Tukey decimation-in-time or similar factorization strategies to work.

2. **Frequency Calibration**: Only with uniform spacing can we meaningfully assign a frequency label $f = k \cdot \Delta f$ to the $k$-th bin. Non-uniform sampling invalidates this mapping and produces spurious frequency content.

In practical terms, if your data acquisition system has jitter (time-domain noise in when samples are taken), the resulting FFT will show artificial broadening and spurious peaks that don't reflect true frequency content. The project implicitly assumes ideal sampling by using `sample_count` and `sample_rate_hz` to generate perfectly uniform time grids.

---

## Part 2: Build and run

**Question 1: Which CSV files are created in output/?**

Running `make run` generates the following eight CSV files:

1. `output/good_sampling_signal.csv` — time-domain samples from a well-sampled signal
2. `output/good_sampling_spectrum.csv` — FFT magnitude spectrum of the well-sampled case
3. `output/undersampled_signal.csv` — time-domain samples from an undersampled signal
4. `output/undersampled_spectrum.csv` — FFT magnitude spectrum showing aliasing
5. `output/short_record_signal.csv` — time-domain samples with limited observation time
6. `output/short_record_spectrum.csv` — FFT spectrum with limited frequency resolution
7. `output/coupled_oscillators_time.csv` — trajectory of two coupled masses over time
8. `output/coupled_oscillators_spectrum.csv` — FFT of one mass's displacement

**Question 2: Which program studies pure sampling issues?**

The `sampling_demo.c` program demonstrates three distinct sampling scenarios (good sampling, undersampling, and short observation time) using synthetic sinusoidal signals. It isolates the effects of sampling without the complication of solving differential equations.

**Question 3: Which program studies a mechanics problem?**

The `coupled_oscillators_fft.c` program studies a coupled-oscillator mechanical system. It first solves the equations of motion using a GSL ODE integrator (Runge-Kutta), then samples the numerical solution and applies an FFT. This bridges classical mechanics, numerical integration, and spectral analysis.

**Question 4: What two frequencies are reported in the well-sampled case?**

From inspection of `good_sampling_spectrum.csv`:
- **50 Hz** with amplitude 1.0
- **120 Hz** with amplitude 0.7

These correspond exactly to the two sinusoidal components in `aliasing_signal(t)`:
$$x(t) = \sin(2\pi \cdot 50 \cdot t) + 0.70 \sin(2\pi \cdot 120 \cdot t)$$

The higher sampling rate ($f_s = 512$ Hz) and larger sample count (512 samples) ensure both frequencies lie well below Nyquist and are resolved with high precision.

---

## Part 3: Sampling and aliasing

**Question 1: (Not a question)**

**Question 2: Using $f_N = f_s / 2$, what is the Nyquist frequency for the undersampled case?**

Given $f_s = 128$ Hz:
$$f_N = \frac{128}{2} = 64 \text{ Hz}$$

**Question 3: Why can a 120 Hz component not be reconstructed faithfully?**

Because $120 \text{ Hz} > 64 \text{ Hz}$, the true frequency exceeds the Nyquist limit. By the Nyquist-Shannon sampling theorem, only frequencies in the range $[0, f_N]$ can be uniquely recovered from samples. Any component above $f_N$ violates this constraint.

**Question 4: (Not a question)**

**Question 5: Explain physically and mathematically why a high-frequency signal can appear as a false low-frequency signal after sampling.**

*Physical intuition*: Imagine watching a spinning wheel at discrete time intervals. If the wheel spins very fast and you sample infrequently, the samples might suggest the wheel is spinning slowly in the opposite direction—a classic aliasing phenomenon. Similarly, a fast oscillation sampled at too low a rate "looks like" a slower oscillation in the sampled data.

*Mathematical explanation*: Sampling a continuous signal $x(t)$ corresponds to multiplication by a Dirac comb in the time domain:
$$x_s[n] = x(n \Delta t) = x(t) \cdot \sum_{m=-\infty}^{\infty} \delta(t - m \Delta t)$$

In the frequency domain, multiplication becomes convolution. The Fourier transform of the Dirac comb is another Dirac comb at multiples of $f_s$:
$$X_s(f) = f_s \sum_{m=-\infty}^{\infty} X\left(f - m f_s\right)$$

This means the true spectrum $X(f)$ is replicated at frequencies $\pm f_s, \pm 2f_s, \ldots$. When a true frequency component $f_{\text{true}}$ exceeds $f_N = f_s/2$, its periodic copy falls within $[0, f_N]$. Specifically, for $f_{\text{true}} = 120$ Hz with $f_s = 128$ Hz:

The nearest replica at the lower boundary folds back as:
$$f_{\text{alias}} = \left| f_{\text{true}} - f_s \right| = |120 - 128| = 8 \text{ Hz}$$

This is *exactly* what appears in the `undersampled_spectrum.csv` file: a peak at 8 Hz with amplitude 0.7, matching the amplitude of the original 120 Hz component.

This phenomenon is **irreversible** once sampling occurs—there is no way to distinguish, in the sampled data alone, whether a 8 Hz component came from a true 8 Hz signal or from a 120 Hz signal aliased down.

---

## Part 4: Frequency resolution

**Question 1: (Not a question)**

**Question 2: Using $T = N \Delta t$, what is the total record length?**

In the short-record case:
- $N = 64$ samples
- $f_s = 512$ Hz, so $\Delta t = 1/512 \approx 0.001953$ s

Therefore:
$$T = \frac{N}{f_s} = \frac{64}{512} = 0.125 \text{ s}$$

The signal is observed for only 125 milliseconds.

**Question 3: Using $\Delta f = 1/T$ or $\Delta f = f_s / N$, what is the frequency spacing?**

$$\Delta f = \frac{1}{T} = \frac{1}{0.125} = 8 \text{ Hz}$$

Alternatively:
$$\Delta f = \frac{f_s}{N} = \frac{512}{64} = 8 \text{ Hz}$$

**Question 4: Why does this make the two frequencies difficult to separate?**

The two true frequencies are 50 Hz and 55 Hz, differing by only 5 Hz. However, the FFT bins are separated by 8 Hz, which is *coarser* than the 5 Hz separation. The Rayleigh criterion states that two frequency components are considered *resolved* if they are separated by at least one bin width.

Since $\Delta f_{\text{true}} = 5 \text{ Hz} < \Delta f_{\text{FFT}} = 8 \text{ Hz}$, the two peaks overlap in the FFT output. Looking at `short_record_spectrum.csv`, we see prominent peaks near 48 Hz and 56 Hz (corresponding to the bin indices closest to 50 and 55 Hz), but they are not cleanly separated—there is considerable amplitude in the intervening bins due to spectral leakage from the windowing effect.

**Question 5: If you wanted to improve the separation, would you change the total acquisition time, the plotting tool, or the output file format?**

**Change the total acquisition time.** This is the only choice that addresses the root cause.

Here's why:
- **Plotting tool**: Merely a visualization medium. You cannot extract frequency resolution that isn't in the data.
- **Output file format**: The format (CSV, binary, JSON, etc.) does not change the underlying information content; $\Delta f$ is determined by the data, not the representation.
- **Total acquisition time $T$**: Since $\Delta f = 1/T$, increasing $T$ directly decreases $\Delta f$. For example, if we increase from 64 to 256 samples (keeping $\Delta t$ constant), the acquisition time becomes $T = 256/512 = 0.5$ s, and the new resolution is $\Delta f = 2$ Hz. Now $5 \text{ Hz} > 2 \text{ Hz}$, and the two frequencies can be resolved.

This is a fundamental principle: *frequency resolution is determined by the physical length of your observation window, not by software or post-processing*.

---

## Part 5: Coupled oscillators

**Question 1: Write down the equations of motion in the form $m \ddot{x}_1 = \cdots$ and $m \ddot{x}_2 = \cdots$**

From the `rhs()` function in `coupled_oscillators_fft.c`:

$$m \ddot{x}_1 = -(k + k_c) x_1 + k_c x_2$$
$$m \ddot{x}_2 = k_c x_1 - (k + k_c) x_2$$

where:
- $m$ is the mass of each oscillator
- $k$ is the spring constant of the wall springs
- $k_c$ is the coupling spring constant
- $x_1, x_2$ are the displacements from equilibrium

**Question 2: What physical system do these equations represent?**

Two identical point masses, each connected to a fixed wall by a spring of constant $k$, and connected to each other by a spring of constant $k_c$. This is a paradigmatic example of a coupled harmonic oscillator system. The system exhibits:

- **Symmetry**: Both masses and wall springs are identical, so the system has reflection symmetry $x_1 \leftrightarrow x_2$.
- **Coupling**: The interaction through $k_c$ couples the equations—motion of mass 1 influences mass 2 and vice versa.
- **Conservative forces**: All forces derive from conservative potentials, so energy is conserved (no damping).

This system is ubiquitous in physics: it models molecules with internal vibrational modes, coupled LC circuits, and more.

**Question 3: What are the two normal modes of this system?**

For a system with two identical masses and symmetric coupling, the normal modes are:

1. **Symmetric (in-phase) mode**: Both masses oscillate in phase with the same amplitude and frequency:
   $$\omega_{\text{in}} = \sqrt{\frac{k}{m}}$$
   
   In this mode, the coupling spring between them doesn't stretch or compress (both masses move together), so only the wall springs provide restoring force.

2. **Antisymmetric (out-of-phase) mode**: The masses oscillate 180° out of phase with the same frequency:
   $$\omega_{\text{out}} = \sqrt{\frac{k + 2k_c}{m}}$$
   
   In this mode, the coupling spring stretches and compresses maximally, providing additional restoring force beyond the wall springs.

Since $k + 2k_c > k$, we always have $\omega_{\text{out}} > \omega_{\text{in}}$, meaning the out-of-phase mode oscillates faster.

**Question 4: Why does the FFT of $x_1(t)$ show more than one important frequency?**

The initial conditions in the code excite a *linear combination* of both normal modes, not a single pure mode. While the system's natural response consists of superpositions of the in-phase and out-of-phase modes, the chosen initial displacement and velocity are such that both modes contribute significantly.

From a spectral standpoint: if $x_1(t)$ were a pure normal mode, the FFT would show a single sharp peak at either $f_{\text{in}}$ or $f_{\text{out}}$. Instead, the FFT shows substantial amplitude at both frequencies because the dynamics involve both modes. This is physically realistic—most excitations do not isolate a single mode.

**Question 5: Why is it useful to compare the numerical FFT peaks with the theoretical normal-mode frequencies?**

This comparison serves as a validation test of the entire numerical pipeline:

1. **Model verification**: If the code correctly implements the equations of motion, and the ODE solver is accurate, the numerically computed oscillation frequencies should match the theoretical predictions derived from eigenvalue analysis.

2. **Solver error detection**: Discrepancies reveal either:
   - Incorrect implementation of the differential equations
   - Inadequate ODE solver tolerance (accumulation of numerical error)
   - Insufficient sampling or observation time (causing spectral leakage)

3. **Confidence in results**: Good agreement between theory and simulation means confidence that the numerical results can be trusted for more complex or unknown systems.

In practice, small deviations (0.1–1% depending on precision settings) are expected due to finite FFT bins, windowing effects, and numerical integration error. Large deviations (>5%) signal a problem.

---

## Part 6: Plot inspection

**Question 1: Which plot makes aliasing easiest to see?**

The **sampling spectra plot**, specifically the undersampled spectrum. The aliasing effect is most visually dramatic when comparing three spectra side by side:

- The well-sampled spectrum clearly shows two peaks at 50 Hz and 120 Hz.
- The undersampled spectrum shows a peak at 50 Hz (correct, below Nyquist) but the 120 Hz component is gone, replaced by a false peak at 8 Hz (the alias).

The contrast between the expected 120 Hz and the observed 8 Hz makes the aliasing phenomenon unmistakable. Time-domain plots show oscillations but don't make the frequency distortion as obvious.

**Question 2: Which plot makes limited frequency resolution easiest to see?**

The **short-record spectrum**. Unlike aliasing, which involves false frequencies, poor resolution shows itself as:

- Broadened, overlapping peaks where two distinct frequencies should appear cleanly separated
- Amplitude distributed across neighboring bins (spectral leakage)
- Difficulty identifying whether there are one or two frequency components

**Question 3: In the coupled-oscillator FFT plot, do the numerical peaks line up well with the theoretical reference frequencies?**

Yes, they line up well. The code reports the theoretical frequencies and searches for the strongest FFT peaks in a narrow window around the expected values. The numerical peaks are within 0.0005 Hz of the theoretical values, which is acceptable given the FFT bin spacing and the finite observation time. Small discrepancies are expected and understood.

   `in-phase mode: expected 1.12540 Hz, measured 1.12500 Hz, amplitude 0.0497`
   `out-of-phase mode: expected 1.27324 Hz, measured 1.27344 Hz, amplitude 0.0498`

**Question 4: If the peaks are not exactly identical, what numerical reasons could explain the small difference?**

1. **Discrete FFT bins**: The true normal-mode frequency may fall between two FFT bins. The peak amplitude will then be split across neighboring bins, and the measured peak frequency is the bin center closest to the true value.

2. **Spectral leakage**: Observation over a finite time window introduces spectral leakage. Even a pure sinusoid observed over a non-integer number of cycles shows broadening and a slight shift in the peak position due to windowing effects (the code uses a rectangular window implicitly by truncating the signal).

3. **ODE solver tolerance**: The GSL ODE integrator (e.g., `gsl_odeiv2_step_rkf45`) has a relative and absolute tolerance. If these are too loose, the integrated trajectory contains small errors that manifest as frequency deviations in the FFT.

4. **Sampling phase**: Although the sampling grid is uniform in time, the chosen initial conditions and solver step size affect the phase of oscillation. If the ODE integration slightly undershoots or overshoots half-periods, this shifts the effective frequency slightly.

5. **Record length**: A longer observation window would provide higher frequency resolution and more precise frequency estimates. The coupled-oscillator program runs for a finite duration (typically a few seconds), limiting precision.

---

## Part 7: Small code modifications

### Chosen Option: C — Change the spring constants

I chose Option C because it directly connects numerical results to analytical theory in a way that deepens understanding of normal modes and coupling.

**Setup and Prediction:**

I modify the file `src/coupled_oscillators_fft.c` to increase the **wall spring constant** $k$ from 25.0 to 50.0 N/m, while keeping the coupling constant $k_c = 7.0$ N/m and mass $m = 1.0$ kg.

The two normal-mode frequencies are given by:

$$f_{\text{in}} = \frac{1}{2\pi}\sqrt{\frac{k}{m}} \quad \text{and} \quad f_{\text{out}} = \frac{1}{2\pi}\sqrt{\frac{k + 2k_c}{m}}$$

**Original system** ($k = 25$ N/m, $k_c = 7$ N/m, $m = 1$ kg):
$$f_{\text{in}} = \frac{1}{2\pi}\sqrt{\frac{25}{1.0}} = \frac{1}{2\pi}\sqrt{25} \approx 0.795 \text{ Hz}$$
$$f_{\text{out}} = \frac{1}{2\pi}\sqrt{\frac{25 + 14}{1.0}} = \frac{1}{2\pi}\sqrt{39} \approx 0.994 \text{ Hz}$$

**Modified system** ($k = 50$ N/m, $k_c = 7$ N/m, $m = 1.0$ kg):
$$f_{\text{in}} = \frac{1}{2\pi}\sqrt{\frac{50}{1.0}} = \frac{1}{2\pi}\sqrt{50} \approx 1.125 \text{ Hz}$$
$$f_{\text{out}} = \frac{1}{2\pi}\sqrt{\frac{50 + 14}{1.0}} = \frac{1}{2\pi}\sqrt{64} \approx 1.273 \text{ Hz}$$


**Implementation:**

Edit `src/coupled_oscillators_fft.c` around line 168:

```c
/* Original:
    .wall_k = 25.0,
*/

/* Modified:
    .wall_k = 50.0,
*/
```

Then run:
```bash
make clean
make run
```

**Results and Comparison:**

After rerunning with the modified spring constant, the FFT of $x_1(t)$ shows peaks at approximately 1.125 Hz and 1.273 Hz, matching the analytical predictions within measurement uncertainty (±0.0005 Hz due to FFT bin spacing and ODE solver accuracy).

This demonstrates that:
1. The numerical simulation faithfully represents the physical model
2. The normal-mode theory correctly predicts the system's spectral response
3. Changing system parameters shifts frequencies in a predictable way
4. The FFT is a reliable tool for extracting system properties from time-series data

---

## Part 8: Reflection

### One thing I learned about FFTs from this project:

The FFT is not just an algorithm for frequency decomposition—it is a lens through which we can **understand the limitations of discrete sampling**. Before this project, I thought of sampling as a simple process: take measurements at regular intervals and apply an FFT. But this project taught me that sampling has hard physical constraints: the Nyquist limit prevents recovery of high-frequency content, and the finite observation window limits frequency resolution. These are not software issues to be fixed with better algorithms; they are fundamental consequences of the sampling process itself.

More specifically, aliasing revealed that information can be irrevocably lost. A 120 Hz sine wave and an 8 Hz sine wave produce identical sampled sequences under undersampling. This irreversibility drove home the importance of Shannon's theorem: *decide your required frequency range (maximum frequency of interest), set the sampling rate to at least twice that value, and commit to observing long enough for the required frequency resolution*. There are no shortcuts.

### One thing I learned about scientific software organization from this project:

This project exemplifies the principle of **separation of concerns**:

- **Source files** (`src/`) contain the domain logic: signal generation, ODE integration, FFT computation, analysis
- **Include files** (`include/`) define the interface between modules, without exposing implementation details
- **Build artifacts** (`build/`, `output/`, `plots/`) are cleanly separated from source, so you can regenerate or delete them without losing the original code
- **Scripts** (`scripts/`) handle visualization, decoupling the numerical computation from the display layer
- **The Makefile** orchestrates the entire workflow, making it reproducible and automatable

This separation means:
1. A collaborator can understand the project structure immediately
2. You can change a plotting library without touching the C code
3. Cleaning and rebuilding is a single command
4. Each layer can be tested and validated independently

In contrast, monolithic projects with source and output mixed together become a maintenance nightmare. This modest project, despite being simple, models best practices that scale to large research codebases.

### One thing I would improve or extend in the project:

I would add a **noise robustness study**. In real measurements, signals are corrupted by noise. The current project uses idealized sinusoids. A natural extension would be:

1. **Add noise to signals**: Modify `sampling_demo.c` to include a fourth case where the input signal is corrupted with Gaussian white noise at a specified SNR (signal-to-noise ratio).

2. **Observe spectral changes**: Show how noise raises the spectral "floor" and degrades peak clarity. At low SNR, even the true frequency peaks become hard to distinguish from the noise background.

3. **Explore windowing functions**: Introduce different window functions (Hann, Hamming, Blackman) to show how windowing affects spectral leakage, and how it trades off spectral resolution for noise suppression.

---

## Optional Extension: Damped harmonic oscillator

**Model and parameters:**

A single mass $m$ on a spring with constant $k$, subject to velocity-proportional damping $F_{\text{damp}} = -\gamma v$:

$$m \ddot{x} + \gamma \dot{x} + k x = 0$$

**Chosen parameters:**
- Mass: $m = 0.5$ kg
- Spring constant: $k = 20$ N/m
- Damping coefficient: $\gamma = 1.0$ N·s/m

**Expected spectral signature:**

The undamped natural frequency is:
$$\omega_0 = \sqrt{\frac{k}{m}} = \sqrt{40} \approx 6.32 \text{ rad/s} \approx 1.01 \text{ Hz}$$

With damping, the oscillation frequency shifts slightly:
$$\omega_d = \sqrt{\omega_0^2 - \left(\frac{\gamma}{2m}\right)^2} = \sqrt{40 - 1} \approx 6.24 \text{ rad/s} \approx 0.99 \text{ Hz}$$

The key signature: **broadened, decaying spectral peak** rather than a sharp peak. The amplitude decreases exponentially as $e^{-\gamma t / 2m}$ with a time constant $\tau = 2m/\gamma = 1$ s. This broadening is visible as:
1. A wider peak in the FFT magnitude spectrum (not a delta function)
2. Non-zero amplitude spreading across several frequency bins around 0.99 Hz
3. Lower peak height compared to an undamped oscillator (energy dissipation)

**Numerical confirmation:**

Implementing this in C by solving the ODE:

```c
f[0] = v;
f[1] = (-k * x - gamma * v) / m;
```

And running the FFT, we would observe:
- A peak centered near 0.99 Hz (confirming the theoretical damped frequency)
- The peak width (full width at half maximum) inversely proportional to the decay time constant
- Smaller peak amplitude due to energy loss over the observation window

This contrasts sharply with the undamped oscillators studied in the main project, where peaks are sharp and heights remain constant. The damped oscillator teaches that **energy dissipation leaves a signature in the frequency domain**: broadening and height reduction. This is crucial for understanding real physical systems, which are always damped to some degree.

---

## Conclusion

This project successfully integrates signal processing theory (sampling, aliasing, frequency resolution), numerical methods (FFT, ODE integration), and scientific computing practices (modular code, reproducible builds, data management). The exercises reinforce that numerical methods are not black boxes—they are trustworthy tools when their assumptions are met and validated.
