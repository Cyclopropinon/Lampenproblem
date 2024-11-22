#pragma once

#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <memory>
#include <string>
#include <unistd.h>

#ifdef _WIN32
    #include <intrin.h>  // For __cpuid on Windows
#endif

std::string getExecutablePath()
{
    // Ermittelt den Pfad des aktuellen Programms
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (len == -1)
    {
        throw std::runtime_error("Fehler beim Abrufen des Programmpfads");
    }
    buffer[len] = '\0'; // Nullterminierung des Strings
    return std::string(buffer);
}

std::string getLddOutput(const std::string& executable)
{
    // Öffne einen Prozess, der das Kommando 'ldd' mit dem gegebenen Programm ausführt
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(("ldd " + executable).c_str(), "r"), fclose);
    
    if (!pipe)
    {
        throw std::runtime_error("Fehler beim Öffnen des Pipes");
    }
    
    // Lese die Ausgabe und füge sie zum Ergebnis-String hinzu
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    
    return result;
}

// get CPU name
std::string getCpuName()
{
    #ifdef _WIN32
        // Windows-specific code (using CPUID instruction)
        std::array<int, 4> cpui;
        char cpuName[49] = { 0 };

        // Call __cpuid with EAX=0x80000002, 0x80000003, and 0x80000004
        __cpuid(cpui.data(), 0x80000002);
        memcpy(cpuName, cpui.data(), sizeof(cpui));
        __cpuid(cpui.data(), 0x80000003);
        memcpy(cpuName + 16, cpui.data(), sizeof(cpui));
        __cpuid(cpui.data(), 0x80000004);
        memcpy(cpuName + 32, cpui.data(), sizeof(cpui));

        return std::string(cpuName);

    #elif __linux__
        // Linux-specific code (read from /proc/cpuinfo)
        std::ifstream cpuInfoFile("/proc/cpuinfo");
        std::string line;
        std::string key = "model name";
        while (std::getline(cpuInfoFile, line))
        {
            if (line.substr(0, key.size()) == key)
            {
                // Extract the value after ": "
                return line.substr(line.find(": ") + 2);
            }
        }
        return "Unknown CPU";

    #else
        return "Unsupported platform";  // Handle other platforms
    #endif
}
