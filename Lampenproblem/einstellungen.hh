#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <mutex>

// Funktion zum Schreiben aller Einstellungen in eine .ini-Datei
void schreibeEinstellungenInDatei(const std::string& dateiName) {
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock
    std::ofstream datei(dateiName);

    if (datei.is_open()) {
        for (const auto& einstellung : Einstellungen) {
            datei << einstellung.first << "=" << einstellung.second << std::endl;
        }
        datei.close();
    } else {
        std::cerr << "Konnte die Datei nicht öffnen: " << dateiName << std::endl;
    }
}

// Funktion zum Lesen aller Einstellungen aus einer .ini-Datei
void leseEinstellungenAusDatei(const std::string& dateiName) {
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock
    std::ifstream datei(dateiName);
    std::string zeile;

    if (datei.is_open()) {
        while (std::getline(datei, zeile)) {
            // Suche nach dem Trennzeichen '='
            size_t trennPos = zeile.find('=');
            if (trennPos != std::string::npos) {
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
std::string leseEinstellung(const std::string& schluessel, const std::string& defaultWert) {
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock

    auto it = Einstellungen.find(schluessel);
    if (it != Einstellungen.end()) {
        return it->second; // Einstellung gefunden
    } else {
        // Einstellung existiert nicht, auf Default-Wert setzen
        Einstellungen[schluessel] = defaultWert;
        return defaultWert;
    }
}

// Funktion zum Schreiben einer einzelnen Einstellung
void schreibeEinstellung(const std::string& schluessel, const std::string& wert, const std::string& dateiName) {
    std::lock_guard<std::mutex> lock(einst_mutex); // Mutex lock

    // Einstellung in die Map einfügen oder aktualisieren
    Einstellungen[schluessel] = wert;

    // Aktualisierte Map in die Datei schreiben
    schreibeEinstellungenInDatei(dateiName);
}
