#pragma once

#ifndef LOGLEVEL
    #define LOGLEVEL
#endif

#define LOGFILE Lampenproblem_logfile
#ifdef __FUNCSIG__      //_MSC_VER //if compiling with vs
    #define _PRINTWAYPOINT_     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
    #define _PRINTERROR_        {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
    #define _PRINTINPUT_(x)     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
#else
    #define _PRINTWAYPOINT_     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
    #define _PRINTERROR_        {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
    #define _PRINTINPUT_(x)     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
#endif

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
    std::ofstream LOGFILE(filename);                                                                /*/ File to write logs into*/\
    LOGFILE << "Logfile of Lampenproblem, version: " << _V << "; compiled: " << __DATE__ << ' ' << __TIME__ << "; filename: \"" << filename << "\"; run: " << std::put_time(std::localtime(&t), "%Y-%m-%d %T") << "." << std::setfill('0') << std::setw(3) << '\n';\
    std::cout << "Logfile: " << filename << std::endl;\
    \
    auto Starttime = std::chrono::steady_clock::now();\
    auto Nowtime = std::chrono::steady_clock::now();\
    _PRINTINPUT_("#Arguments: " << argc)\
}