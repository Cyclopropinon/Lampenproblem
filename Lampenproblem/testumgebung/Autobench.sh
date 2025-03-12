#!/bin/bash

#rm -rv a* b* c* d* e* f* g* h* i* j* k* l* m* n* o* p* q* s* t* u* v* w* x* y* z*
cd ..
clear
#bash compile.sh
time make all
cd testumgebung
./Lampenproblem -B
cat Benchergs/ABGV1.csv
