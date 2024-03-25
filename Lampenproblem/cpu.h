#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

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

    // @brief Methode zum besseren Extrahieren der CPU-Zeit diese Threads in einen String.
    // @warning geht nur für Linux zurzeit
    static std::string cpuTimeStr()
    {
        #ifdef _WIN32 //Windows
            return "cpuTimeStr() unavailable in Windows";
        #else // Linux
            clockid_t clockId;
            struct timespec ts;

            pthread_getcpuclockid(pthread_self(), &clockId);
            clock_gettime(clockId, &ts);

                // Umrechnung in Nanosekunden
            char buffer[50];
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat"
            sprintf(buffer, "%llu,%09llus", static_cast<uint64_t>(ts.tv_sec), static_cast<uint64_t>(ts.tv_nsec));
            #pragma GCC diagnostic pop
            return buffer;
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

    // @brief Methode zum besseren Extrahieren der gesamten CPU-Zeit in einen String.
    // @warning geht nur für Linux zurzeit
    static std::string cpuFamilyTimeStr()
    {
        #ifdef _WIN32 //Windows
            return "cpuFamilyTimeStr() unavailable in Windows";
        #else // Linux
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            
            uint64_t seconds = static_cast<uint64_t>(usage.ru_utime.tv_sec) + usage.ru_stime.tv_sec;    // Sekunden
            uint64_t micsecs = static_cast<uint64_t>(usage.ru_utime.tv_usec) + usage.ru_stime.tv_usec;  // Mikrosekunden

            char buffer[50];
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat"
            sprintf(buffer, "%llu,%06llus (%lluh)", seconds, micsecs, seconds / 3600);
            #pragma GCC diagnostic pop
            return buffer;
        #endif
    }
};
