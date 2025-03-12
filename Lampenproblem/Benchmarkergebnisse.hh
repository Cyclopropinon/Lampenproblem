#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "logfile.hh"

// Funktion zum Erstellen des Datums und Uhrzeit im Format YYYY-MM-DD HH:MM:SS
std::string get_current_datetime()
{
	_PRINTINPUT_5_("Funktionsaufruf: get_current_datetime")
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    // Formatierung des aktuellen Datums
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time_t));
    return std::string(buffer);
}

// Funktion zum Überprüfen, ob die Datei existiert
bool fileExists(const std::string& filename)
{
	_PRINTINPUT_6_("Funktionsaufruf: fileExists")
    return std::filesystem::exists(filename); // Existenz der Datei prüfen
}

// Funktion zum Initialisieren der Datei mit den Spaltenüberschriften, falls sie nicht existiert
void initialisireCSV(const std::string& filename, unsigned int anzWerte)
{
	_PRINTINPUT_4_("Funktionsaufruf: initialisireCSV")
    std::ofstream outFile(filename, std::ios_base::app); // Datei im Anhängemodus öffnen
    if (outFile.is_open())
    {
        if (outFile.tellp() == 0) // Wenn die Datei leer ist (noch keine Daten)
        {
            outFile << "Datum, Version"; // Überschriften schreiben
            for (unsigned int i = 1; i <= anzWerte; i++)
            {
                outFile << ", Rohscore" << i;
            }
            outFile << "\n";
        }
        outFile.close();
    } else {
        std::cerr << "Fehler beim Erstellen oder Öffnen der Datei!" << std::endl;
        _PRINTINPUT_2_("Fehler beim Erstellen der Datei: " << filename)
    }
}

// Funktion zum Schreiben der Benchmark-Daten in eine CSV-Datei
void writeBenchmarkDataToCSV(const std::string& filename, uint64_t score)
{
	_PRINTINPUT_4_("Funktionsaufruf: writeBenchmarkDataToCSV")
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app); // Öffnen im Anhängemodus
    if (outFile.is_open())
    {
        std::string timestamp = get_current_datetime();
        outFile << timestamp << ", " << _V << ", " << score << "\n"; // CSV-Format: Datum, Version, Score
        outFile.close();
    } else {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        _PRINTINPUT_2_("Fehler beim Öffnen der Datei: " << filename)
    }
}

// Funktion zum Schreiben der Benchmark-Daten in eine CSV-Datei, diesmal mit 2 Werten
void writeBenchmarkDataToCSV(const std::string& filename, uint64_t score1, uint64_t score2)
{
	_PRINTINPUT_4_("Funktionsaufruf: writeBenchmarkDataToCSV")
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app); // Öffnen im Anhängemodus
    if (outFile.is_open())
    {
        std::string timestamp = get_current_datetime();
        outFile << timestamp << ", " << _V << ", " << score1 << ", " << score2 << "\n"; // CSV-Format: Datum, Version, Score
        outFile.close();
    } else {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        _PRINTINPUT_2_("Fehler beim Öffnen der Datei: " << filename)
    }
}

// Funktion zum Schreiben der Benchmark-Daten in eine CSV-Datei, diesmal mit 3 Werten
void writeBenchmarkDataToCSV(const std::string& filename, uint64_t score1, uint64_t score2, uint64_t score3)
{
	_PRINTINPUT_4_("Funktionsaufruf: writeBenchmarkDataToCSV")
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app); // Öffnen im Anhängemodus
    if (outFile.is_open())
    {
        std::string timestamp = get_current_datetime();
        outFile << timestamp << ", " << _V << ", " << score1 << ", " << score2 << ", " << score3 << "\n"; // CSV-Format: Datum, Version, Score
        outFile.close();
    } else {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        _PRINTINPUT_2_("Fehler beim Öffnen der Datei: " << filename)
    }
}
