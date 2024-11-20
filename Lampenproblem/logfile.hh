#pragma once

#include "globalVars.hh"
#include "deviceinfos.hh"

// Legt den default Loglevel fest, falls nicht vom Compiler angegeben
#ifndef LOGLEVEL
    // Bestimmt, was alles geloggt wird:
    // 0: Nichts
    // 1: Nur schwere Fehler und das Allernötigste
    // 2: Alle Fehler und mehr Details
    // 3: Funktionsaufrufe
    // 4: Zwischenwerte
    // 5: Funktionsaufrufe in zeitkritischen Schleifen
    #define LOGLEVEL 7
#endif

// Maximum Filename Length
#define _MaxFilenameLength_ 40

#if LOGLEVEL >= 1
    // Function to trim/extend the filename to the right size
    #define __LengthUnifiedFilename__(length) LengthUnifiedFilename = std::string(__FILE__); LengthUnifiedFilename.resize(length, ' ')

    #define LOGFILE Lampenproblem_logfile
    
    // Printing templates
    #ifdef __FUNCSIG__      //_MSC_VER //if compiling with vs
        #define _PRINTWAYPOINT_lv(v) {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tFunktion: " << __FUNCSIG__ << " \tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
        #define _PRINTERROR_lv(v)    {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tFunktion: " << __FUNCSIG__ << " \tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
        #define _PRINTINPUT_lv(x,v)  {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tFunktion: " << __FUNCSIG__ << " \tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
    #else
        #define _PRINTWAYPOINT_lv(v) {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
        #define _PRINTERROR_lv(v)    {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
        #define _PRINTINPUT_lv(x,v)  {lock_log; auto __LengthUnifiedFilename__(_MaxFilenameLength_); auto Nowtime = std::chrono::steady_clock::now(); LOGFILE << LengthUnifiedFilename << " Line: " << __LINE__ << " \tSeverity: " << v << "\tRuntime: " << std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
    #endif

    std::ofstream LOGFILE;
    // Initialisiert die Logdatei
    // sollte nur einmal aufgerufen werden
    #define _LOGFILEINIT_()         {\
        bool DateinameNichtGefunden = true;\
        std::string filename;\
        do\
        {\
            filename = "Lampenproblem_log_" + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) + ".LPlog";\
            std::fstream testlog(filename);\
            DateinameNichtGefunden = testlog.good();\
            testlog.close();\
        } while (DateinameNichtGefunden);\
        \
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());\
        \
        LOGFILE.open(filename);                                                                     /*/ File to write logs into*/\
        LOGFILE << "Logdatei vom Lampenproblem, Version: " << _V\
          << "\nKompilierungsdetails:\n" << Kompilierungsdetails\
          << "\nDateiname: \t\"" << filename << "\"\nLoglevel: \t" << LOGLEVEL << "\nStartzeit: \t" << std::put_time(std::localtime(&t), "%Y-%m-%d %T") << "." << std::setfill('0') << std::setw(3) << "\n\n";\
        std::cout << "Logfile: " << filename << std::endl;\
        \
        _PRINTINPUT_1_("#Arguments: " << argc)\
    }

    #define _PRINTWAYPOINT_1_ _PRINTWAYPOINT_lv(1)
    #define _PRINTFATALERROR_ _PRINTERROR_lv(1)
    #define _PRINTINPUT_1_(x) _PRINTINPUT_lv(x,1)
    #define _PRINTVAR_1_(x)   _PRINTINPUT_lv(#x << " = " << x,1)

    #if LOGLEVEL >= 2
        #define _PRINTWAYPOINT_2_ _PRINTWAYPOINT_lv(2)
        #define _PRINTERROR_      _PRINTERROR_lv(2)
        #define _PRINTINPUT_2_(x) _PRINTINPUT_lv(x,2)
        #define _PRINTVAR_2_(x)   _PRINTINPUT_lv(#x << " = " << x,2)
        #if LOGLEVEL >= 3
            #define _PRINTWAYPOINT_3_ _PRINTWAYPOINT_lv(3)
            #define _PRINTINPUT_3_(x) _PRINTINPUT_lv(x,3)
            #define _PRINTVAR_3_(x)   _PRINTINPUT_lv(#x << " = " << x,3)
            #if LOGLEVEL >= 4
                #define _PRINTWAYPOINT_4_ _PRINTWAYPOINT_lv(4)
                #define _PRINTINPUT_4_(x) _PRINTINPUT_lv(x,4)
                #define _PRINTVAR_4_(x)   _PRINTINPUT_lv(#x << " = " << x,4)
                #if LOGLEVEL >= 5
                    #define _PRINTWAYPOINT_5_ _PRINTWAYPOINT_lv(5)
                    #define _PRINTINPUT_5_(x) _PRINTINPUT_lv(x,5)
                    #define _PRINTVAR_5_(x)   _PRINTINPUT_lv(#x << " = " << x,5)
                    #if LOGLEVEL >= 6
                        #define _PRINTWAYPOINT_6_ _PRINTWAYPOINT_lv(6)
                        #define _PRINTINPUT_6_(x) _PRINTINPUT_lv(x,6)
                        #define _PRINTVAR_6_(x)   _PRINTINPUT_lv(#x << " = " << x,6)
                        #if LOGLEVEL >= 7
                            #define _PRINTWAYPOINT_7_ _PRINTWAYPOINT_lv(7)
                            #define _PRINTINPUT_7_(x) _PRINTINPUT_lv(x,7)
                            #define _PRINTVAR_7_(x)   _PRINTINPUT_lv(#x << " = " << x,7)
                        #else // LOGLEVEL = 6
                            #define _PRINTWAYPOINT_7_
                            #define _PRINTINPUT_7_(x)
                            #define _PRINTVAR_7_(x)   
                        #endif
                    #else // LOGLEVEL = 5
                        #define _PRINTWAYPOINT_6_
                        #define _PRINTINPUT_6_(x)
                        #define _PRINTVAR_6_(x)   
                    #endif
                #else // LOGLEVEL = 4
                    #define _PRINTWAYPOINT_5_
                    #define _PRINTINPUT_5_(x)
                    #define _PRINTVAR_5_(x)   
                #endif
            #else // LOGLEVEL = 3
                #define _PRINTWAYPOINT_4_
                #define _PRINTINPUT_4_(x)
                #define _PRINTVAR_4_(x)   
            #endif
        #else // LOGLEVEL = 2
            #define _PRINTWAYPOINT_3_
            #define _PRINTINPUT_3_(x)
            #define _PRINTVAR_3_(x)   
       #endif
    #else // LOGLEVEL = 1
        #define _PRINTWAYPOINT_2_
        #define _PRINTERROR_
        #define _PRINTINPUT_2_(x)
        #define _PRINTVAR_2_(x)   
    #endif
#else // LOGLEVEL = 0
    #define _LOGFILEINIT_()
    #define _PRINTWAYPOINT_1_
    #define _PRINTFATALERROR_
    #define _PRINTINPUT_1_(x)
    #define _PRINTVAR_1_(x)   
#endif
