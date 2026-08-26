"""Plot for Exercise 5 -- empirical CDF of the exponential sample.

Reads data/ex5_empirical_cdf.dat (columns: x_(i), F_n(x_(i))) produced
by build/ex5_empirical_cdf, which already returns points sorted and
ready to plot, and overlays them with the exact exponential CDF
F(y) = 1 - exp(-lambda y). The Kolmogorov-Smirnov statistic D_n printed
by the C program to stderr is the single-number summary of the largest
gap between the two curves shown here.
"""

import numpy as np
import matplotlib.pyplot as plt
from style import apply_style, data_path, fig_path, SAMPLE_COLOR, THEORY_COLOR

apply_style()

LAMBDA = 1.5  # must match the value passed to build/ex5_empirical_cdf

x_emp, f_emp = np.loadtxt(data_path("ex5_empirical_cdf.dat"), unpack=True)

x_grid = np.linspace(0.0, x_emp[-1], 500)
cdf = 1.0 - np.exp(-LAMBDA * x_grid)

fig, ax = plt.subplots()
ax.plot(x_emp, f_emp, color=SAMPLE_COLOR, linewidth=1.3, label="empirical CDF")
ax.plot(x_grid, cdf, color=THEORY_COLOR, linestyle="--", linewidth=2, label="exact CDF")

# Highlight the point of largest deviation (the KS statistic location).
f_exact_at_samples = 1.0 - np.exp(-LAMBDA * x_emp)
gap = np.abs(f_emp - f_exact_at_samples)
i_max = np.argmax(gap)
ax.annotate(f"max gap = {gap[i_max]:.4f}",
            xy=(x_emp[i_max], f_emp[i_max]),
            xytext=(x_emp[i_max] + 0.5, f_emp[i_max] - 0.15),
            arrowprops=dict(arrowstyle="->", color="black", lw=1),
            fontsize=9)

ax.set_xlabel("y")
ax.set_ylabel("F(y)")
ax.set_title("Exercise 5: Empirical CDF vs exact exponential CDF")
ax.legend()
fig.tight_layout()
fig.savefig(fig_path("ex5_empirical_cdf.png"), dpi=150)
print("Saved figures/ex5_empirical_cdf.png")
print(f"  Kolmogorov-Smirnov statistic D_n = {gap[i_max]:.6f}")
