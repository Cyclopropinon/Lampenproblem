#pragma once

#include "extract.hh"
#include "zwischenVar.h"

#include "BenchmarkingGMPlibV1-data.h"

void EinzelkernBenchmarkingGMPlibV1()
{
	_PRINTINPUT_3_("Funktionsaufruf: EinzelkernBenchmarkingGMPlibV1")
vector<mpz_class> LampenSimulierenGMPLIBv8(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv8")
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
		//berechnungsStartHR -= Laufzeit;

		// for debugging
		increasedBackupFrequency = DefaultIncreasedBackupFrequency;
	}

    vector<bool> AlleLampenAn(n, true);
    vector<bool> AlleLampenAus(n, false);

    PositiveRunden.reserve(anz);
    if (einsenAnzeigen)
    {
        PositiveRunden.push_back(1);
    }

	start_color();  // Aktiviert die Farbunterstützung
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_BLUE, COLOR_BLACK);

    Lampen[0] = false;

	// 1. Stufe
    while (AnzRunden <= n_gmplib)
    {
        Schritte += AnzRunden;
        if (AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
				_PRINTINPUT_1_("!!!WICHTIG!!! PR gefunden (n,k): (" << n << ", " << AnzRunden << ") WIE IST DAS MÖGLICH?????\nBITTE OBIGES SOFORT MELDEN!!!")
            }

            AnzRunden = 1 + Schritte / n_gmplib;
        }
        Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

        print++;
	}

	// 2. Stufe
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
			if (increasedBackupFrequency || print % 1048576 == 0)
			{
				dPrint = print - cPrint;
				cPrint = print;
				Laufzeit = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR) + ZeitAuserhalbDieserSession;
				if(!increasedBackupFrequency) increasedBackupFrequency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsEndeHR).count() >= 7200'000'000'000; // wenn die Zwischenzeit länger als 2 Stunden sind
				CheckpointLSGv6(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
				berechnungsZwCP_HR = berechnungsEndeHR;
				zdt(berechnungsStartHR, durHR);
				zdt(berechnungsZwCP_HR, CP_HR);
				zPdt(berechnungsZwCP_HR, CPdHR);

				// Redirect output to the ncurses window
				
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	zdt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	zPdt(berechnungsZwCP_HR, CPdHR);

    return PositiveRunden;
}

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
