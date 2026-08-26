# Makefile for HW07 -- Random Number Generators
#
# Pipeline: compile the C exercises -> run them to produce data files ->
# call Python/Matplotlib to turn the data into figures.
#
#   make            build the C programs only
#   make run        build + generate all data/*.dat files
#   make plots      build + run + generate all figures/*.png
#   make all        same as "make plots" (default target)
#   make clean      remove build/, data/ and figures/

CC      := gcc
CFLAGS  := -O2 -Wall -Wextra -std=c11 -Isrc
LDLIBS  := -lm

SRC_DIR    := src
BUILD_DIR  := build
DATA_DIR   := data
FIG_DIR    := figures
PLOT_DIR   := plots

EXES := ex1_coin_tosses ex2_mc_pi ex3_change_of_variables ex4_exponential ex5_empirical_cdf
BINARIES := $(addprefix $(BUILD_DIR)/,$(EXES))

.PHONY: all compile run plots clean

all: plots

# Note: this phony target is called "compile" rather than "build" on
# purpose -- "build" is already the name of the output directory
# ($(BUILD_DIR)), and giving both the same name confuses make into
# reporting a circular dependency.
compile: $(BINARIES)

$(BUILD_DIR)/%: $(SRC_DIR)/%.c $(SRC_DIR)/lcg.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

$(BUILD_DIR) $(DATA_DIR) $(FIG_DIR):
	mkdir -p $@

# --- run: execute each program, redirecting stdout to its data file and
#     letting the diagnostic stderr messages print to the terminal ---
run: compile | $(DATA_DIR)
	$(BUILD_DIR)/ex1_coin_tosses            100000            > $(DATA_DIR)/ex1_coin_tosses.dat
	$(BUILD_DIR)/ex2_mc_pi                  20                > $(DATA_DIR)/ex2_mc_pi.dat
	$(BUILD_DIR)/ex3_change_of_variables    200000            > $(DATA_DIR)/ex3_change_of_variables.dat
	$(BUILD_DIR)/ex4_exponential            200000     1.5    > $(DATA_DIR)/ex4_exponential.dat
	$(BUILD_DIR)/ex5_empirical_cdf          50000      1.5    > $(DATA_DIR)/ex5_empirical_cdf.dat

# --- plots: one Python/Matplotlib script per exercise ---
plots: run | $(FIG_DIR)
	python3 $(PLOT_DIR)/plot_ex1.py
	python3 $(PLOT_DIR)/plot_ex2.py
	python3 $(PLOT_DIR)/plot_ex3.py
	python3 $(PLOT_DIR)/plot_ex4.py
	python3 $(PLOT_DIR)/plot_ex5.py

clean:
	rm -rf $(BUILD_DIR) $(DATA_DIR) $(FIG_DIR)
