SHELL := /bin/bash

# Find the most current g++ version
GXX := $(shell command -v g++-14 || command -v g++-13 || command -v g++-12 || command -v g++)

# Compiler and Linker flags
CXXFLAGS := -I/usr/local/include -g -O2 -march=native -DNCURSES_WIDECHAR=1
LDFLAGS := -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -lncursesw

# Check for FLINT library
CHECK_FLINT := $(shell ldconfig -p | grep -q libflint && echo "yes" || echo "no")
ifeq ($(CHECK_FLINT),yes)
    LDFLAGS += -lflint
else
    CXXFLAGS += -D_DISABLELIBFLINT_
endif

# Check for Archive library
CHECK_ARCHIVE := $(shell ldconfig -p | grep -q libarchive && echo "yes" || echo "no")
ifeq ($(CHECK_ARCHIVE),yes)
    LDFLAGS += -larchive
else
    @echo "Warning: Archive library not found, compiling without -larchive."
endif

TARGET := Lampenproblem
SRC := Lampenproblem.cpp
OBJ := $(SRC:.cpp=.o)

all: check-deps run-betterxxd build restore-data install

check-deps:
	@echo "Using compiler: $(GXX)"
	@if [ "$(CHECK_FLINT)" = "no" ]; then echo "FLINT library not found, disabling support."; fi
	@if [ "$(CHECK_ARCHIVE)" = "no" ]; then echo "Archive library not found, compiling without -larchive."; fi

build: $(TARGET)

$(TARGET): $(SRC)
	/usr/bin/time -v $(GXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

run-betterxxd:
	bash betterxxd.sh BenchmarkingGMPlibV1.tar BenchmarkingGMPlibV1-data.h || exit 2

clean:
	rm -fv $(TARGET) BenchmarkingGMPlibV1-data.h

restore-data:
	cp -v BenchmarkingGMPlibV1-data.h.bagup BenchmarkingGMPlibV1-data.h

install:
	cp -v $(TARGET) ./testumgebung/

strip:
	strip $(TARGET) || echo 'the "strip" command is not installed lmao'

.PHONY: all check-deps build run-betterxxd clean restore-data install strip
