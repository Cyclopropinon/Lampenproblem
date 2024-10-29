#pragma once

#include "ctdi.hh"

constexpr auto Kompilierungsdetails = 
         "Datum:          " __DATE__
    "\n" "Uhrzeit:        " __TIME__
    "\n" "Betriebssystem: " __OS__
    "\n" "Plattform:      " __PLATTFORM__
    "\n" "Kompiler:       " __KOMPILER__
    "\n" "Architektur:    " __ARCHITEKTUR__;

