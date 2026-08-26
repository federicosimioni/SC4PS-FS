"""Plot for Exercise 4 -- inverse transform exponential sampling.

Reads data/ex4_exponential.dat (a single column of Y samples) produced
by build/ex4_exponential, and overlays the sample histogram with the
exact exponential density f_Y(y) = lambda * exp(-lambda * y).
"""

import numpy as np
import matplotlib.pyplot as plt
from style import apply_style, data_path, fig_path, SAMPLE_COLOR, THEORY_COLOR

apply_style()

LAMBDA = 1.5  # must match the value passed to build/ex4_exponential

y = np.loadtxt(data_path("ex4_exponential.dat"))

grid = np.linspace(0.0, np.quantile(y, 0.995), 500)
pdf = LAMBDA * np.exp(-LAMBDA * grid)

fig, ax = plt.subplots()
ax.hist(y, bins=80, density=True, alpha=0.65, color=SAMPLE_COLOR, label="inverse-transform samples")
ax.plot(grid, pdf, color=THEORY_COLOR, linewidth=2, label=rf"$\lambda e^{{-\lambda y}}$, $\lambda={LAMBDA}$")
ax.set_xlabel("y")
ax.set_ylabel("density")
ax.set_xlim(0, grid[-1])
ax.set_title(r"Exercise 4: Inverse-transform exponential sampling")
ax.legend()
fig.tight_layout()
fig.savefig(fig_path("ex4_exponential.png"), dpi=150)
print("Saved figures/ex4_exponential.png")
print(f"  sample mean = {y.mean():.6f}  (theory 1/lambda = {1.0/LAMBDA:.6f})")
print(f"  sample var  = {y.var():.6f}  (theory 1/lambda^2 = {1.0/LAMBDA**2:.6f})")
