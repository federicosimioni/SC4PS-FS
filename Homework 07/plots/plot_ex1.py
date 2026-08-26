"""Plot for Exercise 1 -- coin tosses and the Law of Large Numbers.

Reads data/ex1_coin_tosses.dat (columns: toss_index, running_fraction)
produced by the C program build/ex1_coin_tosses, and plots the running
fraction of heads against the number of tosses on a log-x axis.
"""

import numpy as np
import matplotlib.pyplot as plt
from style import apply_style, data_path, fig_path, SAMPLE_COLOR, THEORY_COLOR

apply_style()

toss_index, running_fraction = np.loadtxt(data_path("ex1_coin_tosses.dat"), unpack=True)

fig, ax = plt.subplots()
ax.plot(toss_index, running_fraction, color=SAMPLE_COLOR, linewidth=1.0,
        label="running fraction of heads")
ax.axhline(0.5, color=THEORY_COLOR, linestyle="--", linewidth=1.5,
           label="theoretical mean = 0.5")

# Shade the theoretical 1/sqrt(n) fluctuation band around 0.5 as a visual
# reference for how fast the LLN convergence should be.
n = toss_index
band = 0.5 / np.sqrt(n)
ax.fill_between(n, 0.5 - band, 0.5 + band, color=THEORY_COLOR, alpha=0.12,
                 label=r"$0.5 \pm 1/(2\sqrt{n})$ reference band")

ax.set_xscale("log")
ax.set_xlabel("number of tosses (n)")
ax.set_ylabel("running fraction of heads")
ax.set_title("Exercise 1: Coin tosses and the Law of Large Numbers")
ax.legend(loc="upper right", fontsize=9)
fig.tight_layout()
fig.savefig(fig_path("ex1_coin_tosses.png"), dpi=150)
print("Saved figures/ex1_coin_tosses.png")
