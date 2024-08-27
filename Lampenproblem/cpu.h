#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "logfile.hh"

#ifdef _WIN32 // Windows
    #include <windows.h>
#else // Linux
    #include <ctime>
    #include <pthread.h>
    #include <sys/resource.h>
    #include <sys/time.h>
#endif

class CPUProfiler
{
public:
    // Details zur CPU
    static std::string cpuInfo()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuInfo")
        // Modellname der CPU aus der /proc/cpuinfo-Datei lesen
        std::ifstream cpuinfo("/proc/cpuinfo");
        std::string line;
        std::string model_name;

        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                size_t colon_pos = line.find(':');
                if (colon_pos != std::string::npos) {
                    model_name = line.substr(colon_pos + 2);
                    break;
                }
            }
        }

        cpuinfo.close();
        return model_name;
    }

    // Anzahll von Kernen
    #define cpuCores sysconf(_SC_NPROCESSORS_ONLN);

    // Methode zur Berechnung der CPU-Zeit in Nanosekunden
    static uint64_t cpuTime()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuTime")
        #ifdef _WIN32 // Windows
            FILETIME createTime, exitTime, kernelTime, userTime;

            GetThreadTimes(GetCurrentThread(), &createTime, &exitTime, &kernelTime, &userTime);

            ULARGE_INTEGER userTimeInt;
            userTimeInt.LowPart = userTime.dwLowDateTime;
            userTimeInt.HighPart = userTime.dwHighDateTime;

            // Umrechnung in Nanosekunden (100 ns Intervalle)
            return userTimeInt.QuadPart * 100;
        #else // Linux
            clockid_t clockId;
            struct timespec ts;

            pthread_getcpuclockid(pthread_self(), &clockId);
            clock_gettime(clockId, &ts);

            // Umrechnung in Nanosekunden
            return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000 + static_cast<uint64_t>(ts.tv_nsec);
        #endif
    }

    // funktioniert nicht wirklich. Warum?
    static timespec cpuTimeTs()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuTimeTs")
        #ifdef _WIN32 // Windows
            //return "cpuTimeTs() unavailable in Windows";
        #else // Linux
            clockid_t clockId;
            struct timespec ts;

            pthread_getcpuclockid(pthread_self(), &clockId);
            clock_gettime(clockId, &ts);
            return ts;
        #endif
    }

    // @brief Methode zum besseren Extrahieren der CPU-Zeit diese Threads in einen String.
    // @warning geht nur für Linux zurzeit
    static std::string cpuTimeStr()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuTimeStr")
        #ifdef _WIN32 //Windows
            return "cpuTimeStr() unavailable in Windows";
        #else // Linux
            clockid_t clockId;
            struct timespec ts;

            pthread_getcpuclockid(pthread_self(), &clockId);
            clock_gettime(clockId, &ts);

                // Umrechnung in Nanosekunden
            char buffer[50];
            sprintf(buffer, "%" PRIu64 ",%09" PRIu64 "s", static_cast<uint64_t>(ts.tv_sec), static_cast<uint64_t>(ts.tv_nsec));
            return buffer;
        #endif
    }

    // @brief Methode zum besseren Extrahieren der CPU-Zeitdifferenzen diese Threads in einen String.
    // @warning geht nur für Linux zurzeit
    static std::string cpuTimeStrDiff(timespec tsStart)
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuTimeStrDiff")
        #ifdef _WIN32 //Windows
            return "cpuTimeStrDiff() unavailable in Windows";
        #else // Linux
            clockid_t clockId;
            struct timespec tsNow;

            pthread_getcpuclockid(pthread_self(), &clockId);
            clock_gettime(clockId, &tsNow);

            // Umrechnung in Nanosekunden
            uint64_t ns = static_cast<uint64_t>(tsNow.tv_sec);
            uint64_t ss = static_cast<uint64_t>(tsStart.tv_sec);
            uint64_t nn = static_cast<uint64_t>(tsNow.tv_nsec);
            uint64_t sn = static_cast<uint64_t>(tsStart.tv_nsec);
            if (sn > nn)
            {
                ns--;
                nn += 1'000'000'000;
            }
            
            char buffer[50];
            sprintf(buffer, "%" PRIu64 ",%09" PRIu64 "s", (ns - ss), (nn - sn));
            return buffer;
        #endif
    }

    // Methode zur Berechnung der gesamten CPU-Zeit (einschließlich Kinder) in Nanosekunden
    static uint64_t cpuFamilyTime()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuFamilyTime")
        #ifdef _WIN32 // Windows
            FILETIME createTime, exitTime, kernelTime, userTime;

            GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime);

            ULARGE_INTEGER userTimeInt;
            userTimeInt.LowPart = userTime.dwLowDateTime;
            userTimeInt.HighPart = userTime.dwHighDateTime;

            // Umrechnung in Nanosekunden (100 ns Intervalle)
            return userTimeInt.QuadPart * 100;
        #else // Linux
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);

            // Gesamtzeit in Mikrosekunden umrechnen
            uint64_t totalTimeMicro = static_cast<uint64_t>(usage.ru_utime.tv_sec) * 1'000'000 + usage.ru_utime.tv_usec +
                                      static_cast<uint64_t>(usage.ru_stime.tv_sec) * 1'000'000 + usage.ru_stime.tv_usec;

            // Umrechnung in Nanosekunden
            return totalTimeMicro * 1000;
        #endif
    }

    // @brief Methode zum besseren Extrahieren der gesamten CPU-Zeit in einen String.
    // @warning geht nur für Linux zurzeit
    static std::string cpuFamilyTimeStr()
    {
	    _PRINTINPUT_5_("Funktionsaufruf: CPUProfiler::cpuFamilyTimeStr")
        #ifdef _WIN32 //Windows
            return "cpuFamilyTimeStr() unavailable in Windows";
        #else // Linux
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            
            uint64_t seconds = static_cast<uint64_t>(usage.ru_utime.tv_sec) + usage.ru_stime.tv_sec;    // Sekunden
            uint64_t micsecs = static_cast<uint64_t>(usage.ru_utime.tv_usec) + usage.ru_stime.tv_usec;  // Mikrosekunden

            char buffer[50];
            sprintf(buffer, "%" PRIu64 ",%06" PRIu64 "s (%" PRIu64 "h)", seconds, micsecs, seconds / 3600);
            return buffer;
        #endif
    }
};
