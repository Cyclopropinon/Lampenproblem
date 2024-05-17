#pragma once

#ifndef LOGLEVEL
    #define LOGLEVEL
#endif

#define LOGFILE std::cerr
#ifdef __FUNCSIG__      //_MSC_VER //if compiling with vs
    #define _PRINTWAYPOINT_     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
    #define _PRINTERROR_        {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
    #define _PRINTINPUT_(x)     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tFunktion: "<<__FUNCSIG__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
#else
    #define _PRINTWAYPOINT_     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns\n"<<std::flush;}
    #define _PRINTERROR_        {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \tERROR: \t\t"<<e.what()<<'\n'<<std::flush;}
    #define _PRINTINPUT_(x)     {auto Nowtime = std::chrono::steady_clock::now(); LOGFILE<<__FILE__<<" \tLine: "<<__LINE__<<" \tRuntime: "<<std::chrono::duration<int64_t,std::nano>{Nowtime-Starttime}.count() << " ns  \t" << x << '\n' << std::flush;}
#endif
