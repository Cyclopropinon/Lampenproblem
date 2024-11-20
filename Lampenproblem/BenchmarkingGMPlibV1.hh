#pragma once

#include "extract.hh"
#include "zwischenVar.h"

#include "BenchmarkingGMPlibV1-data.h"

int64_t EinzelkernBenchmarkingGMPlibV1(std::string Session)
{
	_PRINTINPUT_3_("Funktionsaufruf: EinzelkernBenchmarkingGMPlibV1")

	using namespace std;

	unsigned long long n;
	uint64_t anz;
	bool einsenAnzeigen;
	int timerOrtx;
	int timerOrty;

    mpz_class AnzRunden = 2;
    vector<bool> Lampen(n, true);
    vector<mpz_class> PositiveRunden;
    mpz_t tmp_n_gmplib;
    mpz_init_set_ui(tmp_n_gmplib, n);
    mpz_class Schritte(tmp_n_gmplib);
    const mpz_class n_gmplib(tmp_n_gmplib);
    unsigned long long Lampejetzt;
    unsigned long long print = 0;		// Anz bereits durchgeführter Iterationen
    unsigned long long cPrint = 0;		// Checkpoint für print
    unsigned long long dPrint = 0;		// Anz Iterationen zwischen den 2 letzten Sicherungen
	bool increasedBackupFrequency = false;
    auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
    auto berechnungsEndeHR = berechnungsStartHR;
    auto berechnungsZwCP_HR = berechnungsStartHR;
	std::chrono::nanoseconds Laufzeit;
	std::chrono::nanoseconds ZeitAuserhalbDieserSession(0);
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSGv6(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, ZeitAuserhalbDieserSession);
	} else {
		return -2;
	}

    vector<bool> AlleLampenAn(n, true);
    vector<bool> AlleLampenAus(n, false);
	
	berechnungsStartHR = std::chrono::high_resolution_clock::now();

    while (AnzPR < anz)
    {
        Schritte += AnzRunden;

		if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
		{
			PositiveRunden.push_back(AnzRunden);
			AnzPR = PositiveRunden.size();
			_PRINTINPUT_4_("PR gefunden für n = " << n << "; Größe: " << mpz_sizeinbase(AnzRunden.get_mpz_t(), 265) << " Bytes")
		}

		Lampejetzt = mpz_tdiv_q_ui(AnzRunden.get_mpz_t(), Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];
		AnzRunden++;

        print++;

		if(print % 32768 == 0)
		{
			berechnungsEndeHR = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(berechnungsEndeHR - berechnungsStartHR) + ZeitAuserhalbDieserSession;
			int64_t total_ns = duration.count();
			return total_ns;
		}
	}
/**/
}

void InteraktivBenchmarkingGMPlibV1()
{
	_PRINTINPUT_3_("Funktionsaufruf: InteraktivBenchmarkingGMPlibV1")

	namespace fs = std::filesystem;

	// Benötigte Daten extrahieren
	const char *Basisordner = "/tmp/Lampenproblem";
	std::string Session = Basisordner + std::string("/BenchmarkingGMPlibV1");
	try
	{
		// Versuche, den Ordner zu erstellen
		fs::create_directory(Basisordner);
	} catch (const std::filesystem::filesystem_error& e) {
		// Fehlerbehandlung, falls eine Ausnahme auftritt
		_PRINTERROR_
		std::cerr << "Fehler beim Erstellen des Ordners: " << e.what() << std::endl;
		std::cerr << "Fehlercode: " << e.code() << std::endl;
	}
	// Überprüfen, ob der Ordner existiert
	if (fs::exists(Session)) {
		// Entfernen des Ordners und aller Inhalte darin
		fs::remove_all(Session);
	}
	extract_tar(BenchmarkingGMPlibV1_tar, BenchmarkingGMPlibV1_tar_len, Basisordner);
}
