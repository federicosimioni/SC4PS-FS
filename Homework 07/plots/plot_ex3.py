"""Plot for Exercise 3 -- change of variables Y = U^2.

Reads data/ex3_change_of_variables.dat (columns: u, y) produced by
build/ex3_change_of_variables, and overlays the sample histogram of Y
with the analytic density f_Y(y) = 1 / (2 sqrt(y)).
"""

import numpy as np
import matplotlib.pyplot as plt
from style import apply_style, data_path, fig_path, SAMPLE_COLOR, THEORY_COLOR

apply_style()

u, y = np.loadtxt(data_path("ex3_change_of_variables.dat"), unpack=True)

grid = np.linspace(1e-4, 1.0, 500)
pdf = 1.0 / (2.0 * np.sqrt(grid))

fig, ax = plt.subplots()
ax.hist(y, bins=80, density=True, alpha=0.65, color=SAMPLE_COLOR, label="samples of $Y=U^2$")
ax.plot(grid, pdf, color=THEORY_COLOR, linewidth=2, label=r"$f_Y(y)=1/(2\sqrt{y})$")
ax.set_xlabel("y")
ax.set_ylabel("density")
ax.set_ylim(0, np.nanpercentile(pdf[np.isfinite(pdf)], 99) * 1.3)
ax.set_title(r"Exercise 3: Change of variables $Y=U^2$")
ax.legend()
fig.tight_layout()
fig.savefig(fig_path("ex3_change_of_variables.png"), dpi=150)
print("Saved figures/ex3_change_of_variables.png")
