#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

#include "globalVars.hh"
#include "logfile.hh"

// Funktion zum Überprüfen, ob eine Datei existiert
bool dateiExistiert(const std::string& dateiName)
{
	_PRINTINPUT_5_("Funktionsaufruf: dateiExistiert")
    return std::filesystem::exists(dateiName); // C++17 Funktion zum Überprüfen der Existenz
}

// Funktion zum Erstellen einer neuen Datei
void erstelleLeereDatei(const std::string& dateiName)
{
 	_PRINTINPUT_5_("Funktionsaufruf: erstelleLeereDatei")
    std::ofstream datei(dateiName); // Erstellt eine leere Datei
    if (datei.is_open())
    {
        datei.close();
        std::cout << "Neue Einstellungsdatei erstellt: " << dateiName << std::endl;
    } else {
        std::cerr << "Konnte die Datei nicht erstellen: " << dateiName << std::endl;
    }
}

void touchDatei(const std::string& dateiName)
{
	_PRINTINPUT_4_("Funktionsaufruf: touchDatei")
    if (!dateiExistiert(dateiName)) erstelleLeereDatei(dateiName);
}

// Funktion zum Schreiben aller Einstellungen in eine .ini-Datei
void schreibeEinstellungenInDatei(const std::string& dateiName, bool MutexAktiv)
{
	_PRINTINPUT_3_("Funktionsaufruf: schreibeEinstellungenInDatei")
    if (!MutexAktiv) std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock
    touchDatei(dateiName);
    std::ofstream datei(dateiName);

    if (datei.is_open())
    {
        for (const auto& einstellung : Einstellungen)
        {
            datei << einstellung.first << "=" << einstellung.second << std::endl;
        }
        datei.close();
    } else {
        std::cerr << "Konnte die Datei nicht öffnen: " << dateiName << std::endl;
    }
}

// Funktion zum Lesen aller Einstellungen aus einer .ini-Datei
void leseEinstellungenAusDatei(const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: leseEinstellungenAusDatei")
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock
    touchDatei(dateiName);
    std::ifstream datei(dateiName);
    std::string zeile;

    if (datei.is_open())
    {
        while (std::getline(datei, zeile))
        {
            // Suche nach dem Trennzeichen '='
            size_t trennPos = zeile.find('=');
            if (trennPos != std::string::npos)
            {
                std::string schluessel = zeile.substr(0, trennPos);
                std::string wert = zeile.substr(trennPos + 1);
                Einstellungen[schluessel] = wert; // Einstellung in die Map laden
            }
        }
        datei.close();
    } else {
        std::cerr << "Konnte die Datei nicht öffnen: " << dateiName << std::endl;
    }
}

// Funktion zum Lesen einer einzelnen Einstellung
std::string leseEinstellung(const std::string& schluessel, const std::string& defaultWert, const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: leseEinstellung\tSchlüssel: " << schluessel)
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock

    auto it = Einstellungen.find(schluessel);
    if (it != Einstellungen.end())
    {
        return it->second; // Einstellung gefunden
    } else {
        // Einstellung existiert nicht, auf Default-Wert setzen
        Einstellungen[schluessel] = defaultWert;
        schreibeEinstellungenInDatei(dateiName, true);
        return defaultWert;
    }
}

// Funktion zum Lesen einer einzelnen Einstellung
int leseZahlAusEinstellung(const std::string& schluessel, const int& defaultWert, const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: leseZahlAusEinstellung\tSchlüssel: " << schluessel)
    std::string Zahl = leseEinstellung(schluessel, std::to_string(defaultWert), dateiName);
    try
    {
        int num = std::stoi(Zahl);
        return num;
    } catch (const std::invalid_argument& e)
    {
        _PRINTERROR_
        std::cerr << "Invalid Setting " << schluessel << ": \"" << Zahl << "\" is not a number" << std::endl;
    } catch (const std::out_of_range& e)
    {
        _PRINTERROR_
        std::cerr << "Input (" << Zahl << ") is out of range for an int" << std::endl;
    } catch (const std::exception& e)
    {
        _PRINTERROR_
		std::cerr << "\aFehler:\n" << e.what() << std::endl;
    }

    return defaultWert;
}

// Funktion zum Lesen einer einzelnen Einstellung
uint64_t leseZahlAusEinstellung(const std::string& schluessel, const uint64_t& defaultWert, const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: leseZahlAusEinstellung\tSchlüssel: " << schluessel)
    std::string Zahl = leseEinstellung(schluessel, std::to_string(defaultWert), dateiName);
    try
    {
        uint64_t num = std::stoull(Zahl);
        return num;
    } catch (const std::invalid_argument& e)
    {
        _PRINTERROR_
        std::cerr << "Invalid Setting " << schluessel << ": \"" << Zahl << "\" is not a number" << std::endl;
    } catch (const std::out_of_range& e)
    {
        _PRINTERROR_
        std::cerr << "Input (" << Zahl << ") is out of range for an uint64_t" << std::endl;
    } catch (const std::exception& e)
    {
        _PRINTERROR_
		std::cerr << "\aFehler:\n" << e.what() << std::endl;
    }
    
    return defaultWert;
}

// Funktion zum Lesen einer einzelnen Einstellung
uint32_t leseZahlAusEinstellung(const std::string& schluessel, const uint32_t& defaultWert, const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: leseZahlAusEinstellung\tSchlüssel: " << schluessel)
    std::string Zahl = leseEinstellung(schluessel, std::to_string(defaultWert), dateiName);
    try
    {
        uint32_t num = std::stoul(Zahl);
        return num;
    } catch (const std::invalid_argument& e)
    {
        _PRINTERROR_
        std::cerr << "Invalid Setting " << schluessel << ": \"" << Zahl << "\" is not a number" << std::endl;
    } catch (const std::out_of_range& e)
    {
        _PRINTERROR_
        std::cerr << "Input (" << Zahl << ") is out of range for an uint32_t" << std::endl;
    } catch (const std::exception& e)
    {
        _PRINTERROR_
		std::cerr << "\aFehler:\n" << e.what() << std::endl;
    }
    
    return defaultWert;
}

// Funktion zum Schreiben einer einzelnen Einstellung
void schreibeEinstellung(const std::string& schluessel, const std::string& wert, const std::string& dateiName)
{
	_PRINTINPUT_3_("Funktionsaufruf: schreibeEinstellung")
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock

    // Einstellung in die Map einfügen oder aktualisieren
    Einstellungen[schluessel] = wert;

    // Aktualisierte Map in die Datei schreiben
    schreibeEinstellungenInDatei(dateiName, true);
}

// Funktion, um zu prüfen, ob eine bestimmte Einstellung existiert
bool einstellungExistiert(const std::string& schluessel)
{
	_PRINTINPUT_3_("Funktionsaufruf: einstellungExistiert")
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock

    // Verwende die Methode `find`, um nach dem Schlüssel zu suchen
    return Einstellungen.find(schluessel) != Einstellungen.end();
}
