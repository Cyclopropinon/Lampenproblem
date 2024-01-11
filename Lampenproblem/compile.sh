chmod +x compile.sh
g++-13 -o Lampenproblem Lampenproblem.cpp -lgmp -lgmpxx -O2 -march=native
cp -v Lampenproblem ./testumgebung/
