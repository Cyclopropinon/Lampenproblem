#pragma once

#include <vector>

#include "extract.hh"
#include "zwischenVar.h"

#include "Benchmarkergebnisse.hh"

#include "BenchmarkingGMPlibV1-data.h"

void CheckpointLSGv6(const std::string& ordner, const bool retrieve, unsigned long long& n_ULL, uint64_t& anz, bool& einsenAnzeigen, mpz_class& AnzRunden, std::vector<bool>& Lampen, std::vector<mpz_class>& PositiveRunden, mpz_class& Schritte, unsigned long long& Lampejetzt_ULL, unsigned long long& print_ULL, unsigned long long& cPrint_ULL, unsigned long long& dPrint_ULL, std::chrono::nanoseconds Laufzeit);

int64_t EinzelkernBenchmarkingGMPlibV1(std::string Session, unsigned long long n)
{
	_PRINTINPUT_3_("Funktionsaufruf: EinzelkernBenchmarkingGMPlibV1")

	using namespace std;

	uint64_t anz;
	bool einsenAnzeigen;
	int timerOrtx;
	int timerOrty;

	_PRINTINPUT_5_("Debug")

    mpz_class AnzRunden = 2;
    vector<bool> Lampen(n, true);
    vector<mpz_class> PositiveRunden;
    mpz_t tmp_n_gmplib;
    mpz_init_set_ui(tmp_n_gmplib, n);
    mpz_class Schritte(tmp_n_gmplib);
    const mpz_class n_gmplib(tmp_n_gmplib);

	_PRINTINPUT_6_("Debug")

    unsigned long long Lampejetzt;
    unsigned long long print = 0;		// Anz bereits durchgeführter Iterationen
    unsigned long long cPrint = 0;		// Checkpoint für print
    unsigned long long dPrint = 0;		// Anz Iterationen zwischen den 2 letzten Sicherungen
	bool increasedBackupFrequency = false;
    auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
    auto berechnungsEndeHR = berechnungsStartHR;
    auto berechnungsZwCP_HR = berechnungsStartHR;

	_PRINTINPUT_6_("Debug")

	std::chrono::nanoseconds Laufzeit;
	std::chrono::nanoseconds ZeitAuserhalbDieserSession(0);
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	_PRINTINPUT_5_("Debug")

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSGv6(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, ZeitAuserhalbDieserSession);
	} else {
		return -2;
	}

	_PRINTINPUT_5_("Debug")

    vector<bool> AlleLampenAn(n, true);
    vector<bool> AlleLampenAus(n, false);
	
	berechnungsStartHR = std::chrono::high_resolution_clock::now();

	_PRINTINPUT_5_("Debug")

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
	using std::cin, std::cout, std::cerr, std::endl, std::string;

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

	// Start der interaktion

	char eb;

	cout << "Einzelkern Benchmarking? (y/n) ";
	cin >> eb;
	if(eb == 'y' || eb == 'j')
	{
		auto Ergebnis = EinzelkernBenchmarkingGMPlibV1(Session + "/31", 31);
		_PRINTINPUT_4_("EinzelkernBenchmarkingGMPlibV1-Ergebnis: " << Ergebnis)

		if (Ergebnis > 0)
		{
			size_t Stringlänge = 20;
			string Gesamtzeit;
			string ZpI;
			string Iks;
			
			{
				char Puffer[50];
				uint64_t total_seconds = Ergebnis / 1'000'000'000;
				uint64_t remaining_ns = Ergebnis % 1'000'000'000;
				sprintf(Puffer, "%" PRIu64 ",%09" PRIu64 " s", total_seconds, remaining_ns);
				Gesamtzeit = Puffer;

				// Die Länge des Strings auf 20 auffüllen
				Gesamtzeit.insert(Gesamtzeit.begin(), Stringlänge - Gesamtzeit.length(), ' ');
			}

			{
				auto Zwischenergebnis = Ergebnis / 32768;

				char Puffer[50];
				uint64_t total_seconds = Zwischenergebnis / 1'000'000'000;
				uint64_t remaining_ns = Zwischenergebnis % 1'000'000'000;
				sprintf(Puffer, "%" PRIu64 ",%09" PRIu64 " s", total_seconds, remaining_ns);
				ZpI = Puffer;

				// Die Länge des Strings auf 20 auffüllen
				ZpI.insert(ZpI.begin(), Stringlänge - ZpI.length(), ' ');
			}

			{
				auto Zwischenergebnis = 32768'000'000'000'000 / Ergebnis;		_PRINTVAR_5_(Zwischenergebnis)
				Iks = std::to_string(Zwischenergebnis);
				Iks += " Iks";													_PRINTVAR_5_(Iks)

				// Die Länge des Strings auf 20 auffüllen
				Iks.insert(Iks.begin(), (Stringlänge + 2) - Iks.length(), ' ');
			}

			cout << "Einzelkernergebnis:"
			   "\n" "  Gesamtzeit:     " << Gesamtzeit <<
			   "\n" "  Zeit/Iteration: " << ZpI <<
			   "\n"
			   "\n" "  Einzelkernwert: " << Iks <<								// Iks = Iterationen je 1000s (kilosekunde; ks)
			   "\n" << endl;

			_PRINTINPUT_4_("Einzelkernergebnis (detailliert):")
			_PRINTINPUT_4_("  Gesamtzeit:     " << Gesamtzeit)
			_PRINTINPUT_4_("  Zeit/Iteration: " << ZpI)
			_PRINTINPUT_4_("  Einzelkernwert: " << Iks)
			_PRINTINPUT_4_("")

			{
				erstelleVerzeichnis(BenchmarkErgebnisOrdner);
				std::string Historie = BenchmarkErgebnisOrdner + std::string("/IBGV1EK.csv");

				// Überprüfen, ob die Datei existiert, und ggf. initialisieren
				if (!fileExists(Historie)) initialisireCSV(Historie); // Initialisiere die Datei mit den Spaltenüberschriften
				writeBenchmarkDataToCSV(Historie, Ergebnis);
			}
		} else {
			cout << "Etwas ist Schiefgelaufen :("
			      "\nFehlercode: " << Ergebnis << endl;
		}
	}
}
