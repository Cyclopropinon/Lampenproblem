#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

// Zusammenfassung aller Zussammenfassungen
void ZAZ(const std::string& directoryPath, const std::string& outputFilePath)
{
	_PRINTINPUT_4_("Funktionsaufruf: ZAZ")
    namespace fs = std::filesystem;

    fs::remove(outputFilePath);

    std::ofstream outputFile(outputFilePath, std::ios::out | std::ios::app);

    if (!outputFile.is_open())
    {
	    _PRINTINPUT_2_("Konnte Datei \"" << outputFilePath << "\" nicht öffnen!")
        return;
    }

    // Durchlaufe alle Unterverzeichnisse von directoryPath
    for (const auto& dirEntry : fs::directory_iterator(directoryPath))
    {
        if (fs::is_directory(dirEntry))
        {
            std::string summaryFilePath = dirEntry.path().string() + "/Zusammenfassung";
            std::ifstream inputFile(summaryFilePath);

            if (inputFile.is_open())
            {
	            _PRINTINPUT_5_("Hänge Datei an: " << summaryFilePath)

                // Inhalte in die Ausgabe-Datei schreiben
                outputFile << inputFile.rdbuf();  // Dateiinhalt lesen und anhängen
                outputFile << "\n";  // Füge einen Zeilenumbruch hinzu, um Dateien zu trennen
                inputFile.close();
            } else {
	            _PRINTINPUT_2_("Konnte Datei \"" << summaryFilePath << "\" nicht öffnen!")
            }
        }
    }

    outputFile.close();
	_PRINTINPUT_5_("Zusammengeführte Dateien in " << outputFilePath << " geschrieben.\n");
}
