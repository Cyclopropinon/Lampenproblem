#pragma once

#include "extract.hh"
#include "zwischenVar.h"

#include "BenchmarkingGMPlibV1-data.h"

void EinzelkernBenchmarkingGMPlibV1()
{
	//
}

void InteraktivBenchmarkingGMPlibV1()
{
	namespace fs = std::filesystem;

	// Benötigte Daten extrahieren
	const char *Basisordner = "/run/Lampenproblem";
	std::string Session = Basisordner + std::string("/BenchmarkingGMPlibV1");
	erstelleVerzeichnis(Basisordner);
	// Überprüfen, ob der Ordner existiert
	if (fs::exists(Session)) {
		// Entfernen des Ordners und aller Inhalte darin
		fs::remove_all(Session);
	}
	extract_tar(BenchmarkingGMPlibV1_tar, BenchmarkingGMPlibV1_tar_len, Basisordner);
}
