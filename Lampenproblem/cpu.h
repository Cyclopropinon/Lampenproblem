#pragma once

#include <iostream>
#include <cstdint>

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
    // Methode zur Berechnung der CPU-Zeit in Nanosekunden
    static uint64_t cpuTime()
    {
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

    // Methode zur Berechnung der gesamten CPU-Zeit (einschließlich Kinder) in Nanosekunden
    static uint64_t cpuFamilyTime()
    {
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
};
