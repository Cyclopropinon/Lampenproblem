#!/bin/bash

chmod +x compile.sh

#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncurses -O2 -march=native
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native
#g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lflint -lflintxx -lncursesw -DNCURSES_WIDECHAR=1 -O2 -march=native
g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lflint -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || g++-14 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || g++-12 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || g++ -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native

#cp -v Lampenproblem ./IDA/

# uncomment for release
strip Lampenproblem || echo 'the "strip" command is not installed lmao'
cp -v Lampenproblem ./testumgebung/
cp -uv Onlinestatus.sh ./testumgebung/
