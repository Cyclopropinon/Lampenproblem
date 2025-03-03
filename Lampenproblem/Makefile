SHELL := /bin/bash
TIME  := /usr/bin/time -v

# Finde die aktuellste g++-Version
GXX := $(shell command -v g++-14 || command -v g++-13 || command -v g++-12 || command -v g++)
GCC := $(shell command -v gcc-14 || command -v gcc-13 || command -v gcc-12 || command -v gcc)

# Compiler- und Linkerflags
CXXFLAGS := -I/usr/local/include -g -O2 -march=native -DNCURSES_WIDECHAR=1
TARFLAGS := -g -std=c90
LDFLAGS := -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -lncursesw

# Prüfe FLINT-Bibliothek
CHECK_FLINT := $(shell ldconfig -p | grep -q libflint && echo "yes" || echo "no")
ifeq ($(CHECK_FLINT),yes)
    LDFLAGS += -lflint
else
    CXXFLAGS += -D_DISABLELIBFLINT_
endif

# Prüfe Archive-Bibliothek
CHECK_ARCHIVE := $(shell ldconfig -p | grep -q libarchive && echo "yes" || echo "no")
ifeq ($(CHECK_ARCHIVE),yes)
    LDFLAGS += -larchive
else
    @echo "Warning: Archive library not found, compiling without -larchive."
endif

# Targets
TARGET := Lampenproblem
SRC := Lampenproblem.cpp
OBJ := $(SRC:.cpp=.o)
BG_TAR_SRC := BenchmarkingGMPlibV1-data.c
BG_TAR_OBJ := $(BG_TAR_SRC:.c=.o)
BG_TAR_HEADER := BenchmarkingGMPlibV1-data.h
BG_TAR_SOURCE := BenchmarkingGMPlibV1.tar

all: check-deps run-betterxxd build restore-data install

# Prüfe Abhängigkeiten (Bibliotheken)
check-deps:
	@echo "Using compiler: $(GXX)"
	@if [ "$(CHECK_FLINT)" = "no" ]; then echo "FLINT library not found, disabling support."; fi
	@if [ "$(CHECK_ARCHIVE)" = "no" ]; then echo "Archive library not found, compiling without -larchive."; fi

# Erstelle BG_tar.cpp, falls BenchmarkingGMPlibV1.tar neuer ist
$(BG_TAR_SRC): $(BG_TAR_SOURCE)
	@echo "Generating $(BG_TAR_SRC) from $(BG_TAR_SOURCE)..."
	bash betterxxd.sh $(BG_TAR_SOURCE) $(BG_TAR_SRC)
# Erstelle BG_tar.cpp, falls BenchmarkingGMPlibV1.tar neuer ist
	echo '#include "$(BG_TAR_HEADER)"' | cat - $(BG_TAR_SRC) > temp && mv temp $(BG_TAR_SRC)

# Erstelle Objektdatei für BG_tar
$(BG_TAR_OBJ): $(BG_TAR_SRC)
	$(TIME) $(GCC) $(TARFLAGS) -c $(BG_TAR_SRC) -o $(BG_TAR_OBJ)

# Erstelle Hauptprogramm
$(TARGET): $(SRC) $(BG_TAR_OBJ)
	$(TIME) $(GXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

build: $(TARGET)

# Führe betterxxd aus (zwingend vor Kompilierung)
run-betterxxd: $(BG_TAR_SRC)

clean:
	rm -fv $(TARGET) $(BG_TAR_OBJ) $(BG_TAR_SRC)

restore-data:
	cp -v $(BG_TAR_SRC).bagup $(BG_TAR_SRC)

install:
	cp -v $(TARGET) ./testumgebung/

strip:
	strip $(TARGET) || echo 'the "strip" command is not installed lmao'

compress:
	upx --best --lzma $(TARGET) || echo 'UPX compression failed'

.PHONY: Lampenproblem.cpp all check-deps build run-betterxxd clean restore-data install strip compress
