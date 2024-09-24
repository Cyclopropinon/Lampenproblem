#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

void concatenateFiles(const std::string& directoryPath, const std::string& outputFilePath) {
    std::ofstream outputFile(outputFilePath, std::ios::out | std::ios::app);

    if (!outputFile.is_open()) {
        std::cerr << "Konnte die Datei " << outputFilePath << " nicht öffnen.\n";
        return;
    }

    // Durchlaufe alle Unterverzeichnisse von "./abcd/"
    for (const auto& dirEntry : fs::directory_iterator(directoryPath)) {
        if (fs::is_directory(dirEntry)) {
            std::string summaryFilePath = dirEntry.path().string() + "/Zusammenfassung";
            std::ifstream inputFile(summaryFilePath);

            if (inputFile.is_open()) {
                std::cout << "Hänge Datei an: " << summaryFilePath << std::endl;

                // Inhalte in die Ausgabe-Datei schreiben
                outputFile << inputFile.rdbuf();  // Dateiinhalt lesen und anhängen
                outputFile << "\n";  // Füge einen Zeilenumbruch hinzu, um Dateien zu trennen
                inputFile.close();
            } else {
                std::cerr << "Konnte die Datei " << summaryFilePath << " nicht öffnen.\n";
            }
        }
    }

    outputFile.close();
    std::cout << "Zusammengeführte Dateien in " << outputFilePath << " geschrieben.\n";
}

int main() {
    std::string directoryPath = "./abcd";
    std::string outputFilePath = "./Zusammenfassung.txt";

    concatenateFiles(directoryPath, outputFilePath);

    return 0;
}
