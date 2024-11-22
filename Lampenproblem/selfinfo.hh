#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <array>
#include <unistd.h>
#include <limits.h>

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
