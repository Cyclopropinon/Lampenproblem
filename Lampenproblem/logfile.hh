#pragma once

// Legt den default Loglevel fest, falls nicht vom Compiler angegeben
#ifndef LOGLEVEL
    // Bestimmt, was alles geloggt wird:
    // 0: Nichts
    // 1: Nur schwere Fehler und das Allernötigste
    // 2: Alle Fehler und mehr Details
    // 3: Funktionsaufrufe
    // 4: Zwischenwerte
    #define LOGLEVEL 3
#endif

#if LOGLEVEL >= 1
    #define LOGFILE Lampenproblem_logfile
    
    // Printing templates
    #ifdef __FUNCSIG__      //_MSC_VER //if compiling with vs
        #define _PRINTWAYPOINT_lv(v) {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
        #define _PRINTERROR_lv(v)    {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
        #define _PRINTINPUT_lv(x,v)  {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
    #else
        #define _PRINTWAYPOINT_lv(v) {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
        #define _PRINTERROR_lv(v)    {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
        #define _PRINTINPUT_lv(x,v)  {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tSeverity: " << v << "\tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
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
        LOGFILE << "Logfile of Lampenproblem, version: " << _V << "; compiled: " << __DATE__ << ' ' << __TIME__ << "; filename: \"" << filename << "\"; loglevel: \"" << LOGLEVEL << "\"; run: " << std::put_time(std::localtime(&t), "%Y-%m-%d %T") << "." << std::setfill('0') << std::setw(3) << '\n';\
        std::cout << "Logfile: " << filename << std::endl;\
        \
        _PRINTINPUT_1_("#Arguments: " << argc)\
    }

    #define _PRINTWAYPOINT_1_ _PRINTWAYPOINT_lv(1)
    #define _PRINTFATALERROR_ _PRINTERROR_lv(1)
    #define _PRINTINPUT_1_(x) _PRINTINPUT_lv(x,1)

    #if LOGLEVEL >= 2
        #define _PRINTWAYPOINT_2_ _PRINTWAYPOINT_lv(2)
        #define _PRINTERROR_      _PRINTERROR_lv(2)
        #define _PRINTINPUT_2_(x) _PRINTINPUT_lv(x,2)
        #if LOGLEVEL >= 3
            #define _PRINTWAYPOINT_3_ _PRINTWAYPOINT_lv(3)
            #define _PRINTINPUT_3_(x) _PRINTINPUT_lv(x,3)
            #if LOGLEVEL >= 4
                #define _PRINTWAYPOINT_4_ _PRINTWAYPOINT_lv(4)
                #define _PRINTINPUT_4_(x) _PRINTINPUT_lv(x,4)
            #else // LOGLEVEL = 3
                #define _PRINTWAYPOINT_4_
                #define _PRINTINPUT_4_(x)
            #endif
        #else // LOGLEVEL = 2
            #define _PRINTWAYPOINT_3_
            #define _PRINTINPUT_3_(x)
        #endif
    #else // LOGLEVEL = 1
        #define _PRINTWAYPOINT_2_
        #define _PRINTERROR_
        #define _PRINTINPUT_2_(x)
    #endif
#else // LOGLEVEL = 0
    #define _LOGFILEINIT_()
    #define _PRINTWAYPOINT_1_
    #define _PRINTFATALERROR_
    #define _PRINTINPUT_1_(x)
#endif
