#!/usr/bin/env python3
"""
Legge results.dat (prodotto da legendre_stability.c) e genera:
  - relative_error.png : errore relativo vs l, forward e backward, per ogni x
  - absolute_error.png : errore assoluto vs l, forward e backward, per ogni x

Uso:
    ./legendre_stability > results.dat
    python3 plot_results.py results.dat
"""
import sys
import numpy as np
import matplotlib.pyplot as plt

fname = sys.argv[1] if len(sys.argv) > 1 else "results.dat"

# colonne: x l Pref Pforward Pbackward abs_err_f rel_err_f abs_err_b rel_err_b
data = np.loadtxt(fname)

xs = np.unique(data[:, 0])
fig1, ax1 = plt.subplots(figsize=(7, 5))
fig2, ax2 = plt.subplots(figsize=(7, 5))

for x in xs:
    mask = data[:, 0] == x
    ell = data[mask, 1]
    rel_f = data[mask, 6]
    rel_b = data[mask, 8]
    abs_f = data[mask, 5]
    abs_b = data[mask, 7]

    ax1.semilogy(ell, np.maximum(rel_f, 1e-20), "-o", ms=3,
                 label=f"forward, x={x:g}")
    ax1.semilogy(ell, np.maximum(rel_b, 1e-20), "--s", ms=3,
                 label=f"backward, x={x:g}")

    ax2.semilogy(ell, np.maximum(abs_f, 1e-20), "-o", ms=3,
                 label=f"forward, x={x:g}")
    ax2.semilogy(ell, np.maximum(abs_b, 1e-20), "--s", ms=3,
                 label=f"backward, x={x:g}")

ax1.set_xlabel(r"$\ell$")
ax1.set_ylabel("errore relativo")
ax1.set_title("Errore relativo su $P_\\ell(x)$: forward vs backward (Miller)")
ax1.legend(fontsize=8, ncol=2)
ax1.grid(True, which="both", alpha=0.3)
fig1.tight_layout()
fig1.savefig("relative_error.png", dpi=150)

ax2.set_xlabel(r"$\ell$")
ax2.set_ylabel("errore assoluto")
ax2.set_title("Errore assoluto su $P_\\ell(x)$: forward vs backward (Miller)")
ax2.legend(fontsize=8, ncol=2)
ax2.grid(True, which="both", alpha=0.3)
fig2.tight_layout()
fig2.savefig("absolute_error.png", dpi=150)

print("Grafici salvati: relative_error.png, absolute_error.png")
