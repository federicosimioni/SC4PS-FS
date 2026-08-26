"""Plot for Exercise 2 -- Monte Carlo estimate of pi.

Reads data/ex2_mc_pi.dat (columns: N, mean_pi_estimate, mean_abs_error,
std_abs_error) produced by build/ex2_mc_pi, which repeats the pi
estimator several times per N and reports the mean and standard
deviation of the absolute error. We plot the error-vs-N curve with
error bars, together with the theoretical 1/sqrt(N) Monte Carlo scaling
for comparison.
"""

import numpy as np
import matplotlib.pyplot as plt
from style import apply_style, data_path, fig_path, SAMPLE_COLOR, THEORY_COLOR

apply_style()

n, mean_pi, mean_err, std_err = np.loadtxt(data_path("ex2_mc_pi.dat"), unpack=True)

fig, ax = plt.subplots()
ax.errorbar(n, mean_err, yerr=std_err, fmt="o-", color=SAMPLE_COLOR,
            capsize=3, label="mean |pi_estimate - pi| (over repetitions)")

# Reference line for the expected N^{-1/2} Monte Carlo scaling, anchored
# to match the error at the first sample size.
ref = mean_err[0] * np.sqrt(n[0] / n)
ax.plot(n, ref, "--", color=THEORY_COLOR, linewidth=1.5, label=r"$\propto N^{-1/2}$ reference")

ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel("number of samples (N)")
ax.set_ylabel("absolute error")
ax.set_title(r"Exercise 2: Monte Carlo estimate of $\pi$")
ax.legend(fontsize=9)
fig.tight_layout()
fig.savefig(fig_path("ex2_mc_pi.png"), dpi=150)
print("Saved figures/ex2_mc_pi.png")

for ni, pi_i, err_i in zip(n, mean_pi, mean_err):
    print(f"  N={int(ni):>8d}  mean_pi_estimate={pi_i:.6f}  mean_abs_error={err_i:.6f}")
