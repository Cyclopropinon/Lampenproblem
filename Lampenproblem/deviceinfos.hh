#pragma once

#ifndef STR(x)
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
#endif

#include "ctdi.hh"

//#include "tostr.hh"

//static constexpr const char * av = cts<__KOMPILER_V__, 10>;
//static constexpr const char * av = cts<__ARCHITEKTUR_V__, 10>;

constexpr auto Kompilierungsdetails = 
      "  " "Datum:          " __DATE__
    "\n  " "Uhrzeit:        " __TIME__
    "\n  " "Betriebssystem: " __OS__
    "\n  " "Plattform:      " __PLATTFORM__
    "\n  " "Kompiler:       " __KOMPILER__    "\tVersion: " STR(__KOMPILER_V__)
    "\n  " "Architektur:    " __ARCHITEKTUR__ "\tVersion: " STR(__ARCHITEKTUR_V__);
