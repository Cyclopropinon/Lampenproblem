#!/bin/bash

chmod +x compile.sh

bash betterxxd.sh BenchmarkingGMPlibV1.tar BenchmarkingGMPlibV1-data.h || exit 2

     /usr/bin/time -v g++-13 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -lflint             -larchive -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-13 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_ -larchive -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-14 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_ -larchive -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-12 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_ -larchive -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++    -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_ -larchive -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-13 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -lflint                       -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-13 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_           -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-14 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_           -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++-12 -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_           -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native \
  || /usr/bin/time -v g++    -o Lampenproblem Lampenproblem.cpp -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lgmp -lgmpxx -D_DISABLELIBFLINT_           -lncursesw -DNCURSES_WIDECHAR=1 -g -O2 -march=native

rm -v BenchmarkingGMPlibV1-data.h
cp -v BenchmarkingGMPlibV1-data.h.bagup BenchmarkingGMPlibV1-data.h

ldd Lampenproblem

#cp -v Lampenproblem ./IDA/

# uncomment for release
#strip Lampenproblem || echo 'the "strip" command is not installed lmao'
cp -v Lampenproblem ./testumgebung/
cp -uv Onlinestatus.sh ./testumgebung/
