"""Shared Matplotlib style for the HW07 plotting scripts.

Centralizing the style in one module (rather than repeating the same
rcParams block in every plot_exN.py, as the lecture notebook does) keeps
the five figures visually consistent and makes it trivial to change the
look of the whole homework in one place.
"""

import os
import matplotlib.pyplot as plt

DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data")
FIG_DIR = os.path.join(os.path.dirname(__file__), "..", "figures")

# A small qualitative palette used consistently across the five figures.
SAMPLE_COLOR = "#3B6E8F"
THEORY_COLOR = "#D1495B"
SECONDARY_COLOR = "#61A46C"


def apply_style():
    available = plt.style.available
    style_candidates = [
        "seaborn-v0_8-whitegrid",
        "seaborn-whitegrid",
        "ggplot",
        "default",
    ]
    selected = next((style for style in style_candidates if style in available), "default")
    plt.style.use(selected)
    plt.rcParams["figure.figsize"] = (7.5, 4.5)
    plt.rcParams["axes.spines.top"] = False
    plt.rcParams["axes.spines.right"] = False
    plt.rcParams["font.size"] = 11


def data_path(filename):
    return os.path.join(DATA_DIR, filename)


def fig_path(filename):
    os.makedirs(FIG_DIR, exist_ok=True)
    return os.path.join(FIG_DIR, filename)
