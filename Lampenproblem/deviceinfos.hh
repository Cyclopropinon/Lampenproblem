#pragma once

#ifndef STR(x)
    #define STR_HELPER1(x) #x
    #define STR_HELPER2(x) STR_HELPER1(x)
    #define STR_HELPER3(x) STR_HELPER2(x)
    #define STR_HELPER4(x) STR_HELPER3(x)
    #define STR_HELPER5(x) STR_HELPER4(x)
    #define STR(x) STR_HELPER5(x)
#endif

#include "ctdi.hh"

#include "tostr.hh"

static constexpr const char * av = to_string<__KOMPILER_V__>;
static constexpr const char * av = to_string<__ARCHITEKTUR_V__>;

constexpr auto Kompilierungsdetails = 
         "Datum:          " __DATE__
    "\n" "Uhrzeit:        " __TIME__
    "\n" "Betriebssystem: " __OS__
    "\n" "Plattform:      " __PLATTFORM__
    "\n" "Kompiler:       " __KOMPILER__ STR(__KOMPILER_V__)
    "\n" "Architektur:    " __ARCHITEKTUR__ STR(__ARCHITEKTUR_V__);

