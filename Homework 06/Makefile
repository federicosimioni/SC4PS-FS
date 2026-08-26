CFLAGS  = -O2 -Wall -Wextra -std=c11
LDLIBS  = -lm

TARGET  = daxpy_chunked
SOURCES = daxpy_chunked.c config_parser.c

# --- HDF5 auto-detection -------------------------------------------
# 1) If h5cc exists (the compiler wrapper installed alongside HDF5
#    itself), use it: it always knows the correct header/library
#    paths, even when they are in "non-standard" locations (e.g. on
#    Ubuntu: /usr/include/hdf5/serial, libhdf5_serial instead of
#    libhdf5). This is the most robust option.
# 2) Otherwise try pkg-config (common names: hdf5, hdf5-serial).
# 3) As a last resort, fall back to plain "gcc ... -lhdf5", which
#    only works if hdf5.h and libhdf5 are already on the compiler's
#    default search paths.
H5CC := $(shell command -v h5cc 2>/dev/null)

ifneq ($(H5CC),)
    CC = h5cc
    HDF5_CFLAGS =
    HDF5_LIBS =
else
    CC = gcc
    HDF5_CFLAGS := $(shell pkg-config --cflags hdf5 2>/dev/null || pkg-config --cflags hdf5-serial 2>/dev/null)
    HDF5_LIBS   := $(shell pkg-config --libs hdf5 2>/dev/null || pkg-config --libs hdf5-serial 2>/dev/null)
    ifeq ($(HDF5_LIBS),)
        HDF5_LIBS = -lhdf5
    endif
endif
# ---------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(SOURCES) config_parser.h
	$(CC) $(CFLAGS) $(HDF5_CFLAGS) -o $(TARGET) $(SOURCES) $(HDF5_LIBS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET) config.txt

# Prints which HDF5 compilation strategy was detected (useful for
# debugging if 'make' still can't find hdf5.h).
info:
	@echo "CC          = $(CC)"
	@echo "HDF5_CFLAGS = $(HDF5_CFLAGS)"
	@echo "HDF5_LIBS   = $(HDF5_LIBS)"

clean:
	rm -f $(TARGET) *.o *.h5

.PHONY: all run info clean
