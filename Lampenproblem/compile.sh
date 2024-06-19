#!/bin/bash

chmod +x compile.sh

#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncurses -O2 -march=native
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lflint -lflintxx -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native
g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lflint -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native

#cp -v Lampenproblem ./IDA/
strip Lampenproblem
cp -v Lampenproblem ./testumgebung/
