// Lampenproblem.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
// TODO 1kb
// TODO Terminal width
//

// Version:
#define _V "NaN"  // idk rn

// Uncomment to enable big ints
//#define _ENABLEBIGINTS_

//check if correct boost installed
#ifdef _ENABLEBIGINTS_
	#if __has_include(<boost/multiprecision/cpp_int/add.hpp>)
		#if __has_include(<boost_1_79_0/boost/multiprecision/cpp_int.hpp>)
			#include <boost_1_79_0/boost/multiprecision/cpp_int.hpp>
		#elif __has_include("cpp_int.hpp")
			#include "cpp_int.hpp"
		#else
			#undef _ENABLEBIGINTS_
		#endif
	#else
		#undef _ENABLEBIGINTS_
	#endif
	#ifndef _LARGEINTS_ARE_INCLUDED_ //check wether boost got modified correctly
		#undef _ENABLEBIGINTS_
	#endif
#endif

#include <algorithm>
#include <chrono>
#include <future>
#include <fstream>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

//#include <stdio.h>
//#include <sys/ioctl.h>
//#include <unistd.h>

// if using a progressBar
#include "progBar.h"

using namespace std;
using namespace chrono;
using namespace chrono_literals;

#ifdef _ENABLEBIGINTS_
using namespace boost::multiprecision;
#endif

void Nix()
{}

#ifndef __defProgBar__
int getConsoleWidth()
{
	return 100;
}
#endif

void PrintProgressBar(double progress,int barWidth)
{
	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
}

string VectorenZuString(unsigned long long n, vector<vector<unsigned long long>> v)
{
	string	output = "";

	for (size_t i = 0; i < v.size(); i++)
	{
		if (!v[i].empty())					//vetor to string
		{
			ostringstream oss;

			// Convert all but the last element to avoid a trailing ","
			copy(v[i].begin(), v[i].end() - 1, ostream_iterator<unsigned long long>(oss, "\n\t"));

			// Now add the last element with no delimiter
			oss << v[i].back();

			output += "Lampenanzahl: ";
			output += to_string(i + n);
			output += "; positive Runde(n) :";
			output += oss.str() + "\n";
		}
		
	}
	return output;
}

string GeeigneteThreadgroeszenBerechnen(unsigned long long dN, unsigned int AnzThreads, unsigned int minG, unsigned int maxG, unsigned int MaxAbweichung)
{
	string			GuteThreadgroeszen;
	unsigned int	mod;
	unsigned int	Abweichung;
//	string			str;

	for (size_t i = minG; i <= maxG; i++)
	{
		mod = dN % (AnzThreads * i);
		if (mod != 0)
		{
			Abweichung = AnzThreads * i - mod;
		}
		else
		{
			Abweichung = 0;
		}
		if (Abweichung <= MaxAbweichung)
		{
			GuteThreadgroeszen += "\nThreadgröße: ";
			GuteThreadgroeszen += to_string(i);
			GuteThreadgroeszen += "\tAbweichung: ";
			GuteThreadgroeszen += to_string(Abweichung);
//			GuteThreadgrößen.push_back(str);
		}
	}

	return GuteThreadgroeszen;
}

/*
void speichereVariable(const std::string& ordner, const std::string& variablenname, const void* daten, size_t groesse) {
    std::ofstream file(ordner + "/" + variablenname, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(daten), groesse);
        file.close();
        std::cout << "Variable '" << variablenname << "' erfolgreich gespeichert." << std::endl;
    } else {
        std::cerr << "Fehler beim Öffnen der Datei für Variable '" << variablenname << "'" << std::endl;
    }
}
#define sav(vs, var) speichereVariable(ordner, vs, &var, sizeof(var))

void leseVariable(const std::string& ordner, const std::string& variablenname, void* daten, size_t groesse) {
    std::ifstream file(ordner + "/" + variablenname, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(daten), groesse);
        file.close();
        std::cout << "Variable '" << variablenname << "' erfolgreich geladen." << std::endl;
    } else {
        std::cerr << "Fehler beim Öffnen der Datei für Variable '" << variablenname << "'" << std::endl;
    }
}
#define reed(vs, var) leseVariable(ordner, vs, &var, sizeof(var))
*/

#define saveVar(var) saveVariable(std::string(#var), var)
#define readVar(var) var = readVariable<decltype(var)>(std::string(#var))

#define saveMpz(var) saveVariable(std::string(#var), var)
#define readMpz(var) var(readVariable<mpz_class>(std::string(#var)))

inline void save_mpz(mpz_class zahl, string dateiname)
{
    FILE* file = fopen(dateiname.c_str(), "wb");
    if (!file)
	{
        cerr << "Fehler beim Öffnen der Datei zum Speichern." << endl;
        return;
    }

    mpz_out_raw(file, zahl.get_mpz_t());

    fclose(file);
}

inline mpz_class load_mpz(string dateiname)
{
    mpz_class zahl;

    FILE* file = fopen(dateiname.c_str(), "rb");
    if (!file)
	{
        cerr << "Fehler beim Öffnen der Datei zum Laden." << endl;
        return 0;
    }

    mpz_inp_raw(zahl.get_mpz_t(), file);

    fclose(file);

    return zahl;
}

template <typename SaveType>
void saveVariable(const string dateiname, SaveType var)
{
	if (std::is_same<SaveType, mpz_class>::value)
	{
		save_mpz(var, dateiname);
	} else {
		ofstream outFile(dateiname, ios::binary | ios::out);
		if (outFile.is_open())
		{
			outFile.write(reinterpret_cast<const char*>(&var), sizeof(var));
			outFile.close();
		} else {
			cerr << "Fehler beim Öffnen der Datei zum Speichern." << endl;
		}
	}
}

template <typename SaveType>
SaveType readVariable(const string dateiname)
{
    SaveType var;

	if (std::is_same<SaveType, mpz_class>::value)
	{
		var = load_mpz(dateiname);
	} else {
		ifstream inFile(dateiname, ios::binary | ios::in);
		if (inFile.is_open())
		{
			inFile.read(reinterpret_cast<char*>(&var), sizeof(var));
			inFile.close();
			cout << "Variable erfolgreich geladen." << endl;
		} else {
			cerr << "Fehler beim Öffnen der Datei zum Lesen." << endl;
		}
	}
    return var;
}

void CheckpointLSGv3(const std::string& ordner, const bool retrieve, unsigned long long& n, uint64_t& anz, bool& einsenAnzeigen, mpz_class& AnzRunden, vector<bool>& Lampen, vector<mpz_class>& PositiveRunden, mpz_t& tmp_n_gmplib, mpz_class& Schritte, mpz_class& n_gmplib, unsigned long long& Lampejetzt, int& print)
{
	if (retrieve)				// wenn true, dann wird die datei gelesen, sonst geschrieben
	{
		readVar(n);
		readVar(anz);
		readVar(einsenAnzeigen);
		readVar(AnzRunden);
		readVar(Lampen);
		readVar(PositiveRunden);
		readMpz(tmp_n_gmplib);
		readVar(Schritte);
		readVar(n_gmplib);
		readVar(Lampejetzt);
		readVar(print);
	}
	else
	{
		saveVar(n);
		saveVar(anz);
		saveVar(einsenAnzeigen);
		saveVar(AnzRunden);
		saveVar(Lampen);
		saveVar(PositiveRunden);
		saveVar(tmp_n_gmplib);
		saveVar(Schritte);
		saveVar(n_gmplib);
		saveVar(Lampejetzt);
		saveVar(print);
	}
}

vector<unsigned long long> LampenSimulieren(unsigned long long n, unsigned long long k, bool einsenAnzeigen)
{
	unsigned long long				AnzRunden = 2;				// Aktuelle Runde
    vector<bool>					Lampen;
	vector<unsigned long long>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	unsigned long long				Schritte = n;				// Anzahl der Schritte, die schon gelaufen sind

	vector<bool>	AlleLampenAn;
	vector<bool>	AlleLampenAus;

	for (size_t i = 0; i < n; i++)						// n Lampen, die aus sind erstellen
	{
		Lampen.push_back(true);
		AlleLampenAn.push_back(true);
		AlleLampenAus.push_back(false);
	}
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
	{
		Schritte += AnzRunden;
		if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
		{
			if (Lampen==AlleLampenAn||Lampen==AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);	// Runde vermerken
			}

			AnzRunden = 1 + Schritte / n;
		}
		Lampen[Schritte%n] = !Lampen[Schritte%n];		// Lampe umschalten
	}

	return PositiveRunden;
}

vector<unsigned long long> LampenEinzelnPrüfen(unsigned long long n, unsigned long long maxK, unsigned long long testLampen/*, bool nurNNprüfen*/)		//testLampen <= n
{
	vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
//	vector<unsigned long long>	endePositiveRunden;		// Liste der Runden nachdem alle Lampen an/aus sind
	vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
	vector<bool>				Lampen;
	unsigned long long			rest = 0;
	bool						endeLsgTriv = false;

	vector<bool>				AlleLampenAn;
	vector<bool>				AlleLampenAus;

	Modulo.push_back(0);						// 0-ten Wert erstellen
	Modulo.push_back(rest);						// 1-ten Wert erstellen
	
	for (size_t i = 0; i <= testLampen; i++)
	{
		Lampen.push_back(false);
		AlleLampenAn.push_back(true);
		AlleLampenAus.push_back(false);
	}

	for (size_t i = 1; i <= maxK; i++)			// 2. bis maxK+1-ten Wert erstellen
	{
//		rest = i * ((n - rest) / i) + rest + i - n;
		unsigned long long zwischenwert = i * ((n - rest) / i) + rest + i;
		rest = (zwischenwert) % n;
		if (rest==0)							// Lampe 0 ist inexistent
		{
			rest = n;
		}
		if (zwischenwert - rest > n)
		{
			for (size_t j = 1; j < (zwischenwert-rest)/n; j++)
			{
				Modulo.push_back(0);
				i++;
			}
		}
		Modulo.push_back(rest);
	}

	for (size_t i = 1; i <= maxK; i++)
	{
//		if (!(Modulo[i] > testLampen && testLampen < i))	// Wenn keine Lampen betätigt werden, wird es übersprungen
		if (Modulo[i] <= testLampen)						// Wenn keine Lampen betätigt werden, wird es übersprungen
		{
			if (endeLsgTriv)
			{
				endeLsgTriv = false;
				PositiveRunden.push_back(i - 1);
			}
			for (size_t j = 1; j <= testLampen; j++)
			{
				if (j % i == Modulo[i] && Modulo[i] != 0)	// Schrott-Werte aussortieren
				{
					Lampen[j] = !Lampen[j];
				}
			}

			if (Modulo[i] != 0 && i != 1)			// Nur nicht-triviale Lösungen ausgeben
			{
				Lampen[0] = true;
				if (Lampen == AlleLampenAn)
				{
					PositiveRunden.push_back(i);
					endeLsgTriv = true;
				}

				Lampen[0] = false;
				if (Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(i);
					endeLsgTriv = true;
				}		
			}
		}
	}

	return PositiveRunden;
}

vector<unsigned long long> OptimierteForm1(unsigned long long n, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN)
{
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
		vector<bool>				Lampen;
		unsigned long long			rest = 0;
		unsigned long long			wechsel = (unsigned long long)n * wechselfaktorN;

		vector<bool>				AlleLampenAn;
		vector<bool>				AlleLampenAus;

		if (wechsel > maxK)
		{
			wechsel = maxK;
		}

		Modulo.push_back(0);						// 0-ten Wert erstellen
		Modulo.push_back(rest);						// 1-ten Wert erstellen

		for (size_t i = 0; i <= testLampen; i++)
		{
			Lampen.push_back(false);
			AlleLampenAn.push_back(true);
			AlleLampenAus.push_back(false);
		}

		for (size_t i = 1; i <= wechsel + 1; i++)			// 2. bis maxK+1-ten Wert erstellen
		{
			//		rest = i * ((n - rest) / i) + rest + i - n;
			unsigned long long zwischenwert = i * ((n - rest) / i) + rest + i;
			rest = (zwischenwert) % n;
			if (rest == 0)							// Lampe 0 ist inexistent
			{
				rest = n;
			}
			if (zwischenwert - rest > n)
			{
				for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
				{
					Modulo.push_back(0);
					i++;
				}
			}
			Modulo.push_back(rest);
		}

		for (size_t i = 1; i <= wechsel; i++)
		{
			if (!(Modulo[i] > testLampen && testLampen < i))	// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
				for (size_t j = 1; j <= testLampen; j++)
				{
					if (j % i == Modulo[i] && Modulo[i] != 0)	// Schrott-Werte aussortieren
					{
						Lampen[j] = !Lampen[j];
					}
				}

				if (Modulo[i] != 0 && i != 1)			// Nur nicht-triviale Lösungen ausgeben
				{
					Lampen[0] = true;
					if (Lampen == AlleLampenAn)
					{
						PositiveRunden.push_back(i);
					}

					Lampen[0] = false;
					if (Lampen == AlleLampenAus)
					{
						PositiveRunden.push_back(i);
					}
				}
			}
		}

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Schritte = n * (AnzRunden - 1) + Modulo[AnzRunden] - 1;	// Anzahl der Schritte, die schon gelaufen sind

		if (Modulo[AnzRunden] <= testLampen)
		{
			Lampen[Modulo[AnzRunden]] = !Lampen[Modulo[AnzRunden]];
		}

		while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{
			Schritte += AnzRunden;
			if (AnzRunden >= n || AnzRunden < 1 + Schritte / n)
			{
				Lampen[0] = true;								// Lampen prüfen
				if (Lampen == AlleLampenAn)
				{
					PositiveRunden.push_back(AnzRunden);
				}

				Lampen[0] = false;
				if (Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden);
				}

				AnzRunden = 1 + Schritte / n;
			}
			if (Schritte % n < testLampen)
			{
				Lampen[1 + (Schritte % n)] = !Lampen[1 + (Schritte % n)];	// Lampe umschalten
			}
		}

		return PositiveRunden;
	}
}

vector<unsigned long long> OptimierteForm2(unsigned long long n, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
		vector<bool>				Lampen;
//		bool						lösungJetzt = false;
		unsigned long long			rest = 0;
		double						wechselD = n * wechselfaktorN + maxK * wechselfaktorK;
		unsigned long long			wechsel;
		unsigned long long			unsinn = testLampen + 3;

		vector<bool>				AlleLampenAn;
		vector<bool>				AlleLampenAus;

		if (wechselD > maxK)			//wechsel anpassen
		{
			wechsel = maxK;
		}
		else if (wechselD < n && n < maxK)
		{
			wechsel = n;
		}
		else
		{
			wechsel = (unsigned long long)wechselD;
		}

		Modulo.push_back(unsinn);						// 0-ten Wert erstellen
		Modulo.push_back(unsinn);						// 1-ten Wert erstellen

		for (size_t i = 0; i <= testLampen; i++)
		{
			Lampen.push_back(true);
			AlleLampenAn.push_back(true);
			AlleLampenAus.push_back(false);
		}

		for (size_t i = 1; i < wechsel+1; i++)			// 2. bis maxK+1-ten Wert erstellen
		{
			//		rest = i * ((n - rest) / i) + rest + i - n;
			unsigned long long zwischenwert = i * ((n - rest) / i) + rest + i;
			rest = (zwischenwert) % n;
			if (rest == 0)							// Lampe 0 ist inexistent
			{
				rest = n;
			}
			if (zwischenwert - rest > n)
			{
				for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
				{
					Modulo.push_back(unsinn);
					i++;
				}
			}
			Modulo.push_back(rest);
		}

		for (size_t i = 1; i <= wechsel; i++)
		{
//			if (Modulo[i]!=0)
//			{
//			if (!(Modulo[i] > testLampen /* && testLampen < i */ ))		// Wenn keine Lampen betätigt werden, wird es übersprungen
			if (Modulo[i] <= testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
/*					if (lösungJetzt)
				{
					lösungJetzt = false;
					PositiveRunden.push_back(i-1)
				}*/

				for (size_t j = Modulo[i]; j <= testLampen; j+=i)
				{
//					if (j % i == Modulo[i]/* && Modulo[i] != 0*/)		// Schrott-Werte aussortieren
//					{
					Lampen[j] = !Lampen[j];
//					}
				}

/*					if (Modulo[i] != 0 && i != 1)							// Nur nicht-triviale Lösungen ausgeben
				{*/
				Lampen[0] = true;
				if (Lampen == AlleLampenAn)
				{
					PositiveRunden.push_back(i);
//						lösungJetzt = true;
				}

				Lampen[0] = false;
				if (Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(i);
//						lösungJetzt = true;
				}
//				}
			}
//			}
		}

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Schritte = n * (AnzRunden - 1) + Modulo[AnzRunden] - 1;	// Anzahl der Schritte, die schon gelaufen sind

		if (Modulo[AnzRunden]<=testLampen)
		{
//			for (size_t j = Modulo[AnzRunden]; j <= testLampen; j+=AnzRunden)
//			{
			Lampen[Modulo[AnzRunden]] = !Lampen[Modulo[AnzRunden]];
//			}
		}
		
/*		if (Modulo[AnzRunden] <= testLampen)
		{
			Lampen[Modulo[AnzRunden]] = !Lampen[Modulo[AnzRunden]];
		}*/

		while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{
			Schritte += AnzRunden;
			if (AnzRunden >= n || AnzRunden < 1 + Schritte / n)
			{
				Lampen[0] = true;								// Lampen prüfen
				if (Lampen == AlleLampenAn)
				{
					PositiveRunden.push_back(AnzRunden);
				}

				Lampen[0] = false;
				if (Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden);
				}

				AnzRunden = 1 + Schritte / n;
			}
			unsigned long long Lampejetzt = (Schritte + 1) % n;
			if (Lampejetzt == 0)
			{
				Lampejetzt = n;
			}

			if (Lampejetzt <= testLampen /* && ((Schritte + 1) % n) != 0*/)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
			}
		}

		return PositiveRunden;
	}
}

vector<unsigned long long> OptimierteForm3(unsigned long long n, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)	// maxK >= n
{
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
		vector<bool>				Lampen;
		unsigned long long			rest = 0;
		double						wechselD = n * wechselfaktorN + maxK * wechselfaktorK;
		unsigned long long			wechsel;
		unsigned long long			unsinn = testLampen + 3;

		vector<bool>				AlleLampenAn;
		vector<bool>				AlleLampenAus;

		if (wechselD > maxK)			//wechsel anpassen
		{
			wechsel = maxK;
		}
		else if (wechselD < n)
		{
			wechsel = n;
		}
		else
		{
			wechsel = (unsigned long long)wechselD;
		}

		Modulo.push_back(unsinn);						// 0-ten Wert erstellen
		Modulo.push_back(unsinn);						// 1-ten Wert erstellen

		for (size_t i = 0; i < testLampen; i++)
		{
			Lampen.push_back(true);
			AlleLampenAn.push_back(true);
			AlleLampenAus.push_back(false);
		}

		for (size_t i = 1; i < wechsel+1; i++)			// 2. bis maxK+1-ten Wert erstellen
		{
			unsigned long long zwischenwert = i * ((n - rest - 1) / i) + rest + i;
			rest = (zwischenwert) % n;
			if (zwischenwert - rest > n)
			{
				for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
				{
					Modulo.push_back(unsinn);
					i++;
				}
			}
			Modulo.push_back(rest);
		}

		for (size_t i = 1; i <= wechsel; i++)
		{
			if (Modulo[i] < testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
				for (size_t j = Modulo[i]; j < testLampen; j+=i)
				{
					Lampen[j] = !Lampen[j];
				}

				if (Lampen == AlleLampenAn)
				{
					PositiveRunden.push_back(i);
				}

				if (Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(i);
				}
			}
		}

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Lampejetzt = Modulo[AnzRunden];

		if (Lampejetzt < testLampen)
		{
			Lampen[Lampejetzt] = !Lampen[Lampejetzt];
		}
		
		/*while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{
			Schritte += AnzRunden;

			if (Lampen == AlleLampenAn)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			if (Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			AnzRunden = 1 + Schritte / n;
			unsigned long long Lampejetzt = Schritte % n;

			if (Lampejetzt < testLampen)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
			}
		}*/

		while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			if (Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			Lampejetzt += AnzRunden;
			AnzRunden += Lampejetzt / n;
			Lampejetzt %= n;

			if (Lampejetzt < testLampen)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
			}
		}
	
		return PositiveRunden;
	}
}

vector<unsigned long long> OptimierteForm4(unsigned long long n, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)	// maxK >= n
{
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
		vector<bool>				Lampen(testLampen, true);
		unsigned long long			rest = 0;
		double						wechselD = n * wechselfaktorN + maxK * wechselfaktorK;
		unsigned long long			wechsel;
		unsigned long long			unsinn = testLampen + 3;

		vector<bool>				AlleLampenAn(testLampen, true);
		vector<bool>				AlleLampenAus(testLampen, false);

		if (wechselD > maxK)			//wechsel anpassen
		{
			wechsel = maxK;
		}
		else if (wechselD < n)
		{
			wechsel = n;
		}
		else
		{
			wechsel = (unsigned long long)wechselD;
		}

		Modulo.reserve(wechsel + 7);
		Modulo.push_back(unsinn);						// 0-ten Wert erstellen
		Modulo.push_back(unsinn);						// 1-ten Wert erstellen

		for (size_t i = 1; i < wechsel + 1; i++)			// 2. bis maxK+1-ten Wert erstellen
		{
			unsigned long long zwischenwert = i * ((n - rest - 1) / i) + rest + i;
			rest = (zwischenwert) % n;
//			if (zwischenwert - rest > n)
			for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
			{
				Modulo.push_back(unsinn);
				i++;
			}
			Modulo.push_back(rest);
		}

		for (size_t i = 1; i <= wechsel; i++)
		{
			if (Modulo[i] < testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
				for (size_t j = Modulo[i]; j < testLampen; j += i)
				{
					Lampen[j] = !Lampen[j];
				}

				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(i);
				}
			}
		}

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Lampejetzt = Modulo[AnzRunden];

		if (Lampejetzt < testLampen)
		{
			Lampen[Lampejetzt] = !Lampen[Lampejetzt];
		}

		while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn||Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			Lampejetzt += AnzRunden;
			AnzRunden += Lampejetzt / n;
			Lampejetzt %= n;

			if (Lampejetzt < testLampen)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
			}
		}

		return PositiveRunden;
	}
}

vector<unsigned long long> OptimierteForm5(unsigned long long n, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN /*double wechselfaktorK*/)	// maxK >= n
{
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
		vector<bool>				Lampen(testLampen, true);
		unsigned long long			rest = 0;
		double						wechselD = n * wechselfaktorN;
		unsigned long long			wechsel;
		unsigned long long			unsinn = testLampen + 3;

		vector<bool>				AlleLampenAn(testLampen, true);
		vector<bool>				AlleLampenAus(testLampen, false);

		if (wechselD > maxK)			//wechsel anpassen
		{
			wechsel = maxK;
		}
		else
		{
			wechsel = (unsigned long long)wechselD;
		}

		Modulo.reserve(wechsel + 7);
		Modulo.push_back(unsinn);						// 0-ten Wert erstellen
		Modulo.push_back(unsinn);						// 1-ten Wert erstellen

		for (size_t i = 1; i < wechsel + 1; i++)			// 2. bis maxK+1-ten Wert erstellen
		{
			unsigned long long zwischenwert = i * ((n - rest - 1) / i) + rest + i;
			rest = (zwischenwert) % n;
			for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
			{
				Modulo.push_back(unsinn);
				i++;
			}
			Modulo.push_back(rest);
		}

		for (size_t i = 1; i <= wechsel; i++)
		{
			if (Modulo[i] < testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
				for (size_t j = Modulo[i]; j < testLampen; j += i)
				{
					Lampen[j] = !Lampen[j];
				}

				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(i);
				}
			}
		}

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Lampejetzt = Modulo[AnzRunden];

		if (Lampejetzt < testLampen)
		{
			Lampen[Lampejetzt] = !Lampen[Lampejetzt];
		}

		while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
			}

			Lampejetzt += AnzRunden;
			AnzRunden += Lampejetzt / n;
			Lampejetzt %= n;

			if (Lampejetzt < testLampen)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
			}
		}

		return PositiveRunden;
	}
}

//vector<vector<unsigned long long>> OptimierteForm2Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
string OptimierteForm2Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm2(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm3Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm3(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm4Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm4(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm5Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN)
{
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm5(i, maxK, testLampen, wechselfaktorN));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm6Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN)
{
	vector<vector<unsigned long long>>	output;

	vector<bool>				AlleLampenAn(testLampen, true);
	vector<bool>				AlleLampenAus(testLampen, false);
	vector<unsigned long long>	Modulo;					// Modulo-werte für einzelne Runden
	vector<bool>				Lampen(testLampen, true);
	unsigned long long			rest;
	unsigned long long			unsinn = testLampen + 3;
	unsigned long long			wechsel;
	unsigned long long			wechselMax;
	double						wechselD;

	unsigned long long		AnzRunden;							// Aktuelle Runde
	unsigned long long		Lampejetzt;

	if (maxN * wechselfaktorN > maxK)			//wechsel anpassen
	{
		wechselMax = maxK;
	}
	else
	{
		wechselMax = (unsigned long long)(maxN * wechselfaktorN);
	}

	Modulo.reserve(wechselMax+7);

	for (size_t n = minN; n <= maxN; n++)
	{
		vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
		Modulo.clear();
		fill(Lampen.begin(), Lampen.end(), true);

		{
			rest = 0;			
			wechselD = n * wechselfaktorN;


			if (wechselD > maxK)			//wechsel anpassen
			{
				wechsel = maxK;
			}
			else
			{
				wechsel = (unsigned long long)wechselD;
			}

			Modulo.push_back(unsinn);						// 0-ten Wert erstellen
			Modulo.push_back(unsinn);						// 1-ten Wert erstellen

			for (size_t i = 1; i < wechsel + 1; i++)			// 2. bis maxK+1-ten Wert erstellen
			{
				unsigned long long zwischenwert = i * ((n - rest - 1) / i) + rest + i;
				rest = (zwischenwert) % n;
				for (size_t j = 1; j < (zwischenwert - rest) / n; j++)
				{
					Modulo.push_back(unsinn);
					i++;
				}
				Modulo.push_back(rest);
			}

			for (size_t i = 1; i < testLampen; i++)
			{
				if (Modulo[i] < testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
				{
					for (size_t j = Modulo[i]; j < testLampen; j += i)
					{
						Lampen[j] = !Lampen[j];
					}

					if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
					{
						PositiveRunden.push_back(i);
					}
				}
			}

			for (size_t i = testLampen; i <= wechsel; i++)
			{
				auto j = Modulo[i];
				if (j < testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
				{
					Lampen[j] = !Lampen[j];

					if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
					{
						PositiveRunden.push_back(i);
					}
				}
			}

			// Berechnungswechsel

			AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
			Lampejetzt = Modulo[AnzRunden];
			
			if (Lampejetzt < testLampen)
			{
				Lampen[Lampejetzt] = !Lampen[Lampejetzt];
			}

			while (AnzRunden <= maxK)								// bis Rundenanzahl erreicht
			{

				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden);
				}

				Lampejetzt += AnzRunden;
				AnzRunden += Lampejetzt / n;
				Lampejetzt %= n;

				if (Lampejetzt < testLampen)
				{
					Lampen[Lampejetzt] = !Lampen[Lampejetzt];	// Lampe umschalten
				}
			}

//			return PositiveRunden;
		}
		output.push_back(PositiveRunden);
	}
	return VectorenZuString(minN, output);
}

#ifdef _ENABLEBIGINTS_

vector<uint1024_t> LampenSimulieren1024(unsigned long long n, uint1024_t k, bool einsenAnzeigen)
{
	uint1024_t				AnzRunden = 2;				// Aktuelle Runde
	vector<bool>			Lampen;
	vector<uint1024_t>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	uint1024_t				Schritte = n;				// Anzahl der Schritte, die schon gelaufen sind
	unsigned long long		Lampejetzt;

	vector<bool>	AlleLampenAn;
	vector<bool>	AlleLampenAus;

	//ostringstream			osa;
	//ostringstream			osk; osk << k;
	//ostringstream			oss;

	for (size_t i = 0; i < n; i++)						// n Lampen, die aus sind erstellen
	{
		Lampen.push_back(true);
		AlleLampenAn.push_back(true);
		AlleLampenAus.push_back(false);
	}
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
	{
		Schritte += AnzRunden;
		if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
		{
			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);	// Runde vermerken
			}

			AnzRunden = 1 + Schritte / n;
			//osa.str(""); osa.clear(); oss.str(""); oss.clear();
			//osa << AnzRunden; oss << Schritte;
			bool nonsense = Lampen[1];
		}
		Lampejetzt = (unsigned long long)(Schritte % n);
		Lampen[Lampejetzt] = !Lampen[Lampejetzt];		// Lampe umschalten
	}

	return PositiveRunden;
}

vector<uint1KB_t> LampenSimulieren1KB(unsigned long long n, uint1KB_t k, bool einsenAnzeigen)
{
	uint1KB_t				AnzRunden = 2;				// Aktuelle Runde
	vector<bool>			Lampen;
	vector<uint1KB_t>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	uint1KB_t				Schritte = n;				// Anzahl der Schritte, die schon gelaufen sind
	unsigned long long		Lampejetzt;

	vector<bool>	AlleLampenAn;
	vector<bool>	AlleLampenAus;

	for (size_t i = 0; i < n; i++)						// n Lampen, die aus sind erstellen
	{
		Lampen.push_back(true);
		AlleLampenAn.push_back(true);
		AlleLampenAus.push_back(false);
	}
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
	{
		Schritte += AnzRunden;
		if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
		{
			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);	// Runde vermerken
			}

			AnzRunden = 1 + Schritte / n;
			bool nonsense = Lampen[1];
		}
		Lampejetzt = (unsigned long long)(Schritte % n);
		Lampen[Lampejetzt] = !Lampen[Lampejetzt];		// Lampe umschalten
	}

	return PositiveRunden;
}

vector<uint1KB_t> OP7LampenSimulieren1KB(unsigned long long n, uint1KB_t maxK, bool einsenAnzeigen)
{
	/*unsigned long long		max64 = 18'446'744'073'709'551'615;
	uint256_t				max256 = (uint256_t)"115792089237316195423570985008687907853269984665640564039457584007913129639935";
	uint1024_t				max1024 = (uint1024_t)max256 * max256; max1024 *= max1024;
	uint1KB_t				max1KB = (uint1KB_t)max1024 * max1024; max1KB *= max1KB; max1KB *= max1KB;*/

	double					Doppel = log(2) / (log(n + 1) - log(n));
	vector<unsigned int>	Wechsel(3);

	Wechsel[0] = (unsigned int)(63 * Doppel);
	Wechsel[1] = (unsigned int)(255 * Doppel);
	Wechsel[2] = (unsigned int)(1023 * Doppel);

	unsigned long long		AnzRunden64 = 2;			// Aktuelle Runde
	checked_uint256_t		AnzRunden256;
	checked_uint1024_t		AnzRunden1024;
	checked_uint1KB_t		AnzRunden1KB;
	vector<bool>			Lampen(n, true);
	vector<uint1KB_t>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	unsigned long long		Lampejetzt64 = 0;			//Schritte64 = n;				// Anzahl der Schritte, die schon gelaufen sind
	checked_uint256_t		Lampejetzt256;				//Schritte256;
	checked_uint1024_t		Lampejetzt1024;				//Schritte1024;
	checked_uint1KB_t		Lampejetzt1KB;				//Schritte1KB;
	//	unsigned long long		Lampejetzt = 0;
	unsigned int			Betaetigen = 0;


	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	/*	while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
		{
			Schritte += AnzRunden;
			if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
			{
				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden);	// Runde vermerken
				}

				AnzRunden = 1 + Schritte / n;
			}
			Lampejetzt = (unsigned long long)(Schritte % n);
			Lampen[Lampejetzt] = !Lampen[Lampejetzt];		// Lampe umschalten
		}*/

		//	try
	{
		while (AnzRunden64 <= n + 1)
		{
			Lampejetzt64 += AnzRunden64;
			if (Lampejetzt64 >= n)
			{
				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden64);
				}
			}
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		while (Betaetigen < Wechsel[0])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden64);
			}

			Lampejetzt64 += AnzRunden64;
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt256 = Lampejetzt64;
		AnzRunden256 = AnzRunden64;

		while (Betaetigen < Wechsel[1])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden256);
			}

			Lampejetzt256 += AnzRunden256;
			AnzRunden256 += Lampejetzt256 / n;
			Lampejetzt256 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt256;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1024 = Lampejetzt256;
		AnzRunden1024 = AnzRunden256;

		while (Betaetigen < Wechsel[2])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1024);
			}

			Lampejetzt1024 += AnzRunden1024;
			AnzRunden1024 += Lampejetzt1024 / n;
			Lampejetzt1024 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1024;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1KB = Lampejetzt1024;
		AnzRunden1KB = AnzRunden1024;

		while (AnzRunden1KB <= maxK)								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1KB);
			}

			Lampejetzt1KB += AnzRunden1KB;
			AnzRunden1KB += Lampejetzt1KB / n;
			Lampejetzt1KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			//		Betätigen++;
		}

	}
	/*	catch (const std::exception& e)
		{
			cout << "Fehler:\n" << e.what() << endl;
	//		PositiveRunden.clear();
		}*/

	return PositiveRunden;
}

vector<uint16KB_t> OP8LampenSimulieren16KB(unsigned long long n, uint16KB_t maxK, bool einsenAnzeigen)
{
	double					Doppel = log(2) / (log(n + 1) - log(n));
	vector<unsigned int>	Wechsel(5);

	Wechsel[0] = (unsigned int)(63 * Doppel);
	Wechsel[1] = (unsigned int)(255 * Doppel);
	Wechsel[2] = (unsigned int)(1023 * Doppel);
	Wechsel[3] = (unsigned int)(8191 * Doppel);
	Wechsel[4] = (unsigned int)(32767 * Doppel);
	//	Wechsel[5] = (unsigned int)(131071 * Doppel);

	unsigned long long		AnzRunden64 = 2;			// Aktuelle Runde
	checked_uint256_t		AnzRunden256;
	checked_uint1024_t		AnzRunden1024;
	checked_uint1KB_t		AnzRunden1KB;
	checked_uint4KB_t		AnzRunden4KB;
	checked_uint16KB_t		AnzRunden16KB;
	vector<bool>			Lampen(n, true);
	vector<uint16KB_t>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	unsigned long long		Lampejetzt64 = 0;			//Schritte64 = n;				// Anzahl der Schritte, die schon gelaufen sind
	checked_uint256_t		Lampejetzt256;				//Schritte256;
	checked_uint1024_t		Lampejetzt1024;				//Schritte1024;
	checked_uint1KB_t		Lampejetzt1KB;				//Schritte1KB;
	checked_uint4KB_t		Lampejetzt4KB;				//Schritte1KB;
	checked_uint16KB_t		Lampejetzt16KB;				//Schritte1KB;
	//	unsigned long long		Lampejetzt = 0;
	unsigned int			Betaetigen = 0;


	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	/*	while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
		{
			Schritte += AnzRunden;
			if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
			{
				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden);	// Runde vermerken
				}

				AnzRunden = 1 + Schritte / n;
			}
			Lampejetzt = (unsigned long long)(Schritte % n);
			Lampen[Lampejetzt] = !Lampen[Lampejetzt];		// Lampe umschalten
		}*/

		//	try
	{
		while (AnzRunden64 <= n + 1)
		{
			Lampejetzt64 += AnzRunden64;
			if (Lampejetzt64 >= n)
			{
				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden64);
				}
			}
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		while (Betaetigen < Wechsel[0])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden64);
			}

			Lampejetzt64 += AnzRunden64;
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt256 = Lampejetzt64;
		AnzRunden256 = AnzRunden64;

		while (Betaetigen < Wechsel[1])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden256);
			}

			Lampejetzt256 += AnzRunden256;
			AnzRunden256 += Lampejetzt256 / n;
			Lampejetzt256 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt256;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1024 = Lampejetzt256;
		AnzRunden1024 = AnzRunden256;

		while (Betaetigen < Wechsel[2])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1024);
			}

			Lampejetzt1024 += AnzRunden1024;
			AnzRunden1024 += Lampejetzt1024 / n;
			Lampejetzt1024 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1024;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1KB = Lampejetzt1024;
		AnzRunden1KB = AnzRunden1024;


		while (Betaetigen < Wechsel[3])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1KB);
			}

			Lampejetzt1KB += AnzRunden1KB;
			AnzRunden1KB += Lampejetzt1KB / n;
			Lampejetzt1KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt4KB = Lampejetzt1KB;
		AnzRunden4KB = AnzRunden1KB;

		while (Betaetigen < Wechsel[4])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden4KB);
			}

			Lampejetzt4KB += AnzRunden4KB;
			AnzRunden4KB += Lampejetzt4KB / n;
			Lampejetzt4KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt4KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt16KB = Lampejetzt4KB;
		AnzRunden16KB = AnzRunden4KB;

		while (AnzRunden16KB <= maxK)								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden16KB);
			}

			Lampejetzt16KB += AnzRunden16KB;
			AnzRunden16KB += Lampejetzt16KB / n;
			Lampejetzt16KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt16KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			//		Betätigen++;
		}

	}
	/*	catch (const std::exception& e)
		{
			cout << "Fehler:\n" << e.what() << endl;
	//		PositiveRunden.clear();
		}*/

	return PositiveRunden;
}

vector<uint16KB_t> OP9LampenSimulieren16KB(unsigned long long n, bool einsenAnzeigen)
{
	double					Doppel = log(2) / (log(n + 1) - log(n));
	vector<unsigned int>	Wechsel(5);

	Wechsel[0] = (unsigned int)(63 * Doppel);
	Wechsel[1] = (unsigned int)(255 * Doppel);
	Wechsel[2] = (unsigned int)(1023 * Doppel);
	Wechsel[3] = (unsigned int)(8191 * Doppel);
	Wechsel[4] = (unsigned int)(32767 * Doppel);
	Wechsel[5] = (unsigned int)(131071 * Doppel);

	unsigned long long		AnzRunden64 = 2;			// Aktuelle Runde
	checked_uint256_t		AnzRunden256;
	checked_uint1024_t		AnzRunden1024;
	checked_uint1KB_t		AnzRunden1KB;
	checked_uint4KB_t		AnzRunden4KB;
	checked_uint16KB_t		AnzRunden16KB;
	vector<bool>			Lampen(n, true);
	vector<uint16KB_t>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	unsigned long long		Lampejetzt64 = 0;			//Schritte64 = n;				// Anzahl der Schritte, die schon gelaufen sind
	checked_uint256_t		Lampejetzt256;				//Schritte256;
	checked_uint1024_t		Lampejetzt1024;				//Schritte1024;
	checked_uint1KB_t		Lampejetzt1KB;				//Schritte1KB;
	checked_uint4KB_t		Lampejetzt4KB;				//Schritte1KB;
	checked_uint16KB_t		Lampejetzt16KB;				//Schritte1KB;
	//	unsigned long long		Lampejetzt = 0;
	unsigned int			Betaetigen = 0;


	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = false;									// erste Lampe der 2. Runde wird umgeschalten

	//	try
	{
		while (AnzRunden64 <= n + 1)
		{
			Lampejetzt64 += AnzRunden64;
			if (Lampejetzt64 >= n)
			{
				if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
				{
					PositiveRunden.push_back(AnzRunden64);
				}
			}
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		while (Betaetigen < Wechsel[0])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden64);
			}

			Lampejetzt64 += AnzRunden64;
			AnzRunden64 += Lampejetzt64 / n;
			Lampejetzt64 %= n;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt256 = Lampejetzt64;
		AnzRunden256 = AnzRunden64;

		while (Betaetigen < Wechsel[1])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden256);
			}

			Lampejetzt256 += AnzRunden256;
			AnzRunden256 += Lampejetzt256 / n;
			Lampejetzt256 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt256;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1024 = Lampejetzt256;
		AnzRunden1024 = AnzRunden256;

		while (Betaetigen < Wechsel[2])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1024);
			}

			Lampejetzt1024 += AnzRunden1024;
			AnzRunden1024 += Lampejetzt1024 / n;
			Lampejetzt1024 %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1024;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt1KB = Lampejetzt1024;
		AnzRunden1KB = AnzRunden1024;


		while (Betaetigen < Wechsel[3])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden1KB);
			}

			Lampejetzt1KB += AnzRunden1KB;
			AnzRunden1KB += Lampejetzt1KB / n;
			Lampejetzt1KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt1KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt4KB = Lampejetzt1KB;
		AnzRunden4KB = AnzRunden1KB;

		while (Betaetigen < Wechsel[4])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden4KB);
			}

			Lampejetzt4KB += AnzRunden4KB;
			AnzRunden4KB += Lampejetzt4KB / n;
			Lampejetzt4KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt4KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

		Lampejetzt16KB = Lampejetzt4KB;
		AnzRunden16KB = AnzRunden4KB;

		while (Betaetigen < Wechsel[5])								// bis Rundenanzahl erreicht
		{

			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden16KB);
			}

			Lampejetzt16KB += AnzRunden16KB;
			AnzRunden16KB += Lampejetzt16KB / n;
			Lampejetzt16KB %= n;
			Lampejetzt64 = (unsigned long long)Lampejetzt16KB;

			Lampen[Lampejetzt64] = !Lampen[Lampejetzt64];	// Lampe umschalten
			Betaetigen++;
		}

	}
	/*	catch (const std::exception& e)
		{
			cout << "Fehler:\n" << e.what() << endl;
	//		PositiveRunden.clear();
		}*/

	return PositiveRunden;
}

#endif

vector<mpz_class> LampenSimulierenGMPLIB(unsigned long long n, mpz_class k, bool einsenAnzeigen)
{
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				Schritte(tmp_n_gmplib);
	mpz_class				n_gmplib(tmp_n_gmplib);
    unsigned long long		Lampejetzt;

//	mpz_init(Schritte);
//	mpz_set(Schritte.get_mpz_t(), n_gmplib);

	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

    if (einsenAnzeigen)
    {
        PositiveRunden.push_back(1);
    }

    Lampen[0] = false;

    while (1 + Schritte / n_gmplib <= k)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
            }

            AnzRunden = 1 + Schritte / n_gmplib;
            bool nonsense = Lampen[1];
        }
		Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];
    }

    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv2(unsigned long long n, uint64_t anz, bool einsenAnzeigen)
{
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				Schritte(tmp_n_gmplib);
	mpz_class				n_gmplib(tmp_n_gmplib);
    unsigned long long		Lampejetzt;
	int						print = 0;
	auto					berechnungsStartHR = std::chrono::high_resolution_clock::now();
	auto					berechnungsEndeHR  = berechnungsStartHR;
	auto					berechnungsZwCP_HR = berechnungsStartHR;
	string					durHR;
	string					CP_HR;
	string					CPdHR;

	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

	PositiveRunden.reserve(anz);
    if (einsenAnzeigen)
    {
        PositiveRunden.push_back(1);
    }

    Lampen[0] = false;


    while (PositiveRunden.size() < anz)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
            }

            AnzRunden = 1 + Schritte / n_gmplib;
            bool nonsense = Lampen[1];
        }
		Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

		print++;
//		if (print % 65536 == 0)
		if (print % 1048576 == 0)
//			pnarc("Mem:" + giveRAM('k') + "\tIteration: " + to_string(print));
		{
			berechnungsZwCP_HR = berechnungsEndeHR;
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wformat"
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);
		    #pragma GCC diagnostic pop
			cout << "    \r \033[91mRAM: " << giveRAM('k') << "  \033[96mIteration: " << to_string(print) << "  \033[95mSchritte: " << mpz_sizeinbase(Schritte.get_mpz_t(), 265) << " Bytes  \033[93mZeit: " << durHR << "\033[0m  \033[2;93mΔt: " << CP_HR << "  \033[3;93mdt/dn: " << CPdHR << "\033[0m             \r" << flush;
		}
    }
	cout << endl;
    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv3(string Session)
{
	unsigned long long		n;
	uint64_t				anz;
	bool					einsenAnzeigen;
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				Schritte(tmp_n_gmplib);
	mpz_class				n_gmplib(tmp_n_gmplib);
    unsigned long long		Lampejetzt;
	int						print = 0;

	CheckpointLSGv3(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, tmp_n_gmplib, Schritte, n_gmplib, Lampejetzt, print);

	auto					berechnungsStartHR = std::chrono::high_resolution_clock::now();
	auto					berechnungsEndeHR  = berechnungsStartHR;
	auto					berechnungsZwCP_HR = berechnungsStartHR;
	string					durHR;
	string					CP_HR;
	string					CPdHR;

	vector<bool>	AlleLampenAn(n, true);
	vector<bool>	AlleLampenAus(n, false);

	PositiveRunden.reserve(anz);
    if (einsenAnzeigen)
    {
        PositiveRunden.push_back(1);
    }

    Lampen[0] = false;


    while (PositiveRunden.size() < anz)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
            }

            AnzRunden = 1 + Schritte / n_gmplib;
            bool nonsense = Lampen[1];
        }
		Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

		print++;
//		if (print % 65536 == 0)
		if (print % 1048576 == 0)
//			pnarc("Mem:" + giveRAM('k') + "\tIteration: " + to_string(print));
		{
			berechnungsZwCP_HR = berechnungsEndeHR;
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wformat"
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);
		    #pragma GCC diagnostic pop
			cout << "    \r \033[91mRAM: " << giveRAM('k') << "  \033[96mIteration: " << to_string(print) << "  \033[95mSchritte: " << mpz_sizeinbase(Schritte.get_mpz_t(), 265) << " Bytes  \033[93mZeit: " << durHR << "\033[0m  \033[2;93mΔt: " << CP_HR << "  \033[3;93mdt/dn: " << CPdHR << "\033[0m             \r" << flush;
		}
    }
	cout << endl;
    return PositiveRunden;
}

int main()
{
	ostringstream						Dateiausgabe;

	auto								AnzThreadsUnterstützt = thread::hardware_concurrency;		// soviele threads wie CPU-Kerne
	long long							AnzThreads3;
//	unsigned long long					AnzThreads4;												// Anzahl der Threads für case 4+
	unsigned int						AnzThreads4;												// Anzahl der Threads für case 4+

	cout << "Version " << _V << " Compiled on: " << __DATE__ << ' ' << __TIME__ << '\n' << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads für Prüfmethode Nr.3 eingeben: ";
	cin >> AnzThreads3;

	if (AnzThreads3 == -1)
	{
		AnzThreads3++;

		cout << "\n\aDu befindest dich jetzt im Debug-Modus.\n\n";

		unsigned long long					minN;
		unsigned long long					maxN;
		unsigned long long					maxNecht;
		unsigned long long					delN;
		unsigned long long					maxK;
#ifdef _ENABLEBIGINTS_
		//	uint1024_t							minN1024;
		//	uint1024_t							maxN1024;
		uint1024_t							maxK1024;
		uint1KB_t							maxK1KB;
		uint16KB_t							maxK16KB;
		//	uint1MB_t							maxK1MB;
#endif
	    mpz_class							maxK_GMPLIB;			// Verwende mpz_class für GMP-Bibliothek
	    uint64_t							mkb, mke;				// Basis und Exponent von max k
		uint64_t							anz;					// Anzahl der gesuchten PR je Lampenanzahl

		vector<vector<unsigned long long>>	vPositiveRunden(AnzThreads3);
		int									prüfart;
		double								wechselfaktorN;
		double								wechselfaktorK;
		unsigned int						minG;
		unsigned int						maxG;
		unsigned int						MaxAbweichung;

		auto								berechnungsStart   = steady_clock::now();
		auto								berechnungsEnde    = steady_clock::now();
		auto								berechnungsStartHR = std::chrono::high_resolution_clock::now();
		auto								berechnungsEndeHR  = std::chrono::high_resolution_clock::now();
		string								durHR;

		double								diffN;



		//	vector<vector<unsigned long long>>	Vectorliste;

		/*	for (size_t i = 0; i < AnzThreads; i++)
			{
				vector<unsigned long long>	defaultVector;
				Vectorliste.push_back(defaultVector);
			}*/

		while (true)
		{
			string filename;
			fstream output_fstream;

			try
			{
				cout << "Programm von Lorenz Taschner & Lars Krabbenhöft\nLampen prüfen bis (n,k)\nWelche Prüfmethode?\n0  = Beenden\t\t\t1  = Simulation\t\t\t\t\t2  = einzelne Lampen Testen\n3  = optimierte Version Nr.1\t4  = optimierte Version Nr.2\t\t\t5  = optimierte & erweiterte Simulation Nr.1\n6  = optimierte Version Nr.3\t7  = optimierte & erweiterte Simulation Nr.2\t8  = optimierte Version Nr.4\n9  = optimierte Version Nr.5\t10 = optimierte Version Nr.6\t\t\t11 = optimierte & erweiterte Simulation Nr.3\n12 = optimierte Version Nr.6.2\t13 = optimierte & erweiterte Simulation Nr.4\t14 = optimierte & erweiterte Simulation Nr.5\n15 = optimierte & erweiterte Simulation mit GMPLIB\t\t16 = optimierte & erweiterte Simulation mit GMPLIB V2\n";
				cin >> prüfart;

				unsigned long long testLampen;
				Dateiausgabe.clear();


				switch (prüfart)
				{
				case 0:
					return 0;
					break;
				case 1:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<unsigned long long> PositiveRunden = LampenSimulieren(i, maxK, false);

						ostringstream oss;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<unsigned long long>(oss, ","));

							// Now add the last element with no delimiter
							oss << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
						}

					}

					berechnungsEnde = steady_clock::now();
					//			cout << "Laufzeit: " << nanoseconds{ berechnungsEnde - berechnungsStart }.count() << "ns\n";
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 2:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;

						vector<unsigned long long> PositiveRunden = LampenEinzelnPrüfen(i, maxK, testLampen);		//lampen prüfen variabel!!!!!!!!

						ostringstream oss;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<unsigned long long>(oss, ","));

							// Now add the last element with no delimiter
							oss << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
						}

					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 3:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;

					//			delN = maxN - minN;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i += AnzThreads3)
					{
						vector<future<vector<unsigned long long>>>	t(AnzThreads3);
						for (size_t j = 0; j < AnzThreads3; j++)
						{
							t[j] = async(launch::async, OptimierteForm1, i + j, maxK, testLampen, wechselfaktorN);
						}
						for (size_t j = 0; j < AnzThreads3; j++)
						{
							try
							{
								vPositiveRunden[j] = t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}

						/*//				thread								vt[AnzThreads];
										vector<thread>						vt;
										vector<vector<unsigned long long>>	PositiveRunden;

										vt.resize(AnzThreads);

										for (size_t j = 0; j < AnzThreads; j++)
										{
											unsigned long long n = i + j;
						//					vt.push_back(NULL);
											vt[j]=thread([n, maxK, testLampen, wechselfaktorN, &PositiveRunden] {PositiveRunden.push_back(OptimierteForm1(n, maxK, testLampen, wechselfaktorN)); });
										}
										for (auto& j : vt)
										{
											j.join();
										}*/
						cout << VectorenZuString(i, vPositiveRunden);
					}

					/*			for (size_t i = minN; i <= maxN; i += 4)
					//			{
					//				string		Ausgabe;
					//
					//				thread t1([&o10] {o10 = OptimierteForm1(i, maxK, testLampen, wechselfaktorN); });
						//			thread t2([&o11] {o11 = OptimierteForm1(i + 1, maxK, testLampen, wechselfaktorN); });
							//		thread t3([&o12] {o12 = OptimierteForm1(i + 2, maxK, testLampen, wechselfaktorN); });
								//	thread t4([&o13] {o13 = OptimierteForm1(i + 3, maxK, testLampen, wechselfaktorN); });
					//
					/*				vector<unsigned long long> PositiveRunden = OptimierteForm1(i, maxK, testLampen, wechselfaktorN);

									ostringstream oss;

									if (!PositiveRunden.empty())					//vetor to string
									{
										// Convert all but the last element to avoid a trailing ","
										copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<unsigned long long>(oss, ","));

										// Now add the last element with no delimiter
										oss << PositiveRunden.back();

										cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
									}

					//				t1.join(); t2.join(); t3.join(); t4.join();
						//			Vectorliste[0] = o10;
							//		Vectorliste[1] = o11;
								//	Vectorliste[2] = o12;
								//	Vectorliste[3] = o13;
								//	cout << VectorenZuString(i, Vectorliste);
								}*/

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 4:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();

					/*			for (size_t i = minN; i <= maxN; i++)
								{
									string		Ausgabe;

									vector<unsigned long long> PositiveRunden = OptimierteForm2(i, maxK, testLampen, wechselfaktorN, wechselfaktorK);		//lampen prüfen variabel!!!!!!!!

									ostringstream oss;

									if (!PositiveRunden.empty())					//vetor to string
									{
										// Convert all but the last element to avoid a trailing ","
										copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<unsigned long long>(oss, ","));

										// Now add the last element with no delimiter
										oss << PositiveRunden.back();

										cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
									}

								}*/

					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm2Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN, wechselfaktorK);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								//						vPositiveRunden[j] = t[j].get();
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}
					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 5:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK1024;
					cout << maxK1024 << '\n';

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<uint1024_t> PositiveRunden = LampenSimulieren1024(i, maxK1024, false);

						ostringstream oss;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1024_t>(oss, ","));

							// Now add the last element with no delimiter
							oss << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
						}

					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 6:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();

					/*for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;

						vector<unsigned long long> PositiveRunden = OptimierteForm3(i, maxK, testLampen, wechselfaktorN, wechselfaktorK);		//lampen prüfen variabel!!!!!!!!

						ostringstream oss;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<unsigned long long>(oss, ","));

							// Now add the last element with no delimiter
							oss << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
						}

					}*/

					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm3Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN, wechselfaktorK);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								//						vPositiveRunden[j] = t[j].get();
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 7:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK1KB;
					cout << maxK1KB << '\n';

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<uint1KB_t> PositiveRunden = LampenSimulieren1KB(i, maxK1KB, false);

						ostringstream oss;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1KB_t>(oss, ","));

							// Now add the last element with no delimiter
							oss << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss.str() << "\n";
						}

					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 8:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();


					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm4Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN, wechselfaktorK);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 9:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();


					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm5Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 10:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;
					cout << "wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();


					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm6Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;
				case 11:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK1KB;
					cout << "Datei speichern unter: ";
					cin >> filename;
					//				cout << maxK1KB << '\n';

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<uint1KB_t> PositiveRunden = OP7LampenSimulieren1KB(i, maxK1KB, false);

						ostringstream oss1;
						ostringstream oss2;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1KB_t>(oss1, ","));
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1KB_t>(oss2, "\n"));

							// Now add the last element with no delimiter
							oss1 << PositiveRunden.back();
							oss2 << PositiveRunden.back();

							cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss1.str() << "\n";
							Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
						}

					}

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Failed to open " << filename << '\n';
					}
					else {
						output_fstream << Dateiausgabe.str() << endl;
						cout << "Datei gespeichert!" << endl;
					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 12:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;
					cout << "Wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;
					cout << "minimale Threadgröße eingeben (empfohlen 100): ";
					cin >> minG;
					cout << "maximale Threadgröße eingeben (empfohlen 500): ";
					cin >> maxG;
					cout << "maximale Abweichung eingeben: ";
					cin >> MaxAbweichung;
					cout << GeeigneteThreadgroeszenBerechnen(maxN - minN + 1, AnzThreads4, minG, maxG, MaxAbweichung);
					cout << "\n\nThreadgröße (Anzahl der n je Thread): ";
					cin >> delN;

					berechnungsStart = steady_clock::now();

					diffN = maxN - minN;
					for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
					{
						vector<future<string>>	t(AnzThreads4);
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							t[j] = async(launch::async, OptimierteForm6Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN);
						}
						for (size_t j = 0; j < AnzThreads4; j++)
						{
							try
							{
								cout << t[j].get();
							}
							catch (const std::exception& e)
							{
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
						PrintProgressBar((i - minN) / diffN, getConsoleWidth());
					}
					PrintProgressBar(1, getConsoleWidth()); cout << endl;
					berechnungsEnde = steady_clock::now();
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
					break;

				case 13:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "max k eingeben: ";
					cin >> maxK16KB;
					cout << "Datei speichern unter: ";
					cin >> filename;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<uint16KB_t> PositiveRunden = OP8LampenSimulieren16KB(i, maxK16KB, false);

						//					ostringstream oss1;
						ostringstream oss2;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
	//						copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint16KB_t>(oss1, ","));
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint16KB_t>(oss2, "\n"));

							// Now add the last element with no delimiter
	//						oss1 << PositiveRunden.back();
							oss2 << PositiveRunden.back();

							//						cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss1.str() << "\n";
							Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
						}

					}

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Failed to open " << filename << '\n';
					}
					else {
						output_fstream << Dateiausgabe.str() << endl;
						cout << "Datei gespeichert!" << endl;
					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;

				case 14:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
					cout << "Datei speichern unter: ";
					cin >> filename;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string		Ausgabe;
						vector<uint16KB_t> PositiveRunden = OP9LampenSimulieren16KB(i, false);

						ostringstream oss2;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint16KB_t>(oss2, "\n"));

							// Now add the last element with no delimiter
							oss2 << PositiveRunden.back();

							Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
						}

					}

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Failed to open " << filename << '\n';
					}
					else {
						output_fstream << Dateiausgabe.str() << endl;
						cout << "Datei gespeichert!" << endl;
					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 15:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
				    cout << "Basis von max k eingeben: ";
					cin >> mkb;
				    cout << "Exponent von max k eingeben (" << mkb << "te Potenz): ";
					cin >> mke;
//				    cout << "max k eingeben: ";
//					cin >> maxK_GMPLIB;
					maxK_GMPLIB = mkb;
					mpz_ui_pow_ui(maxK_GMPLIB.get_mpz_t(), mkb, mke);

					cout << "max k = " << maxK_GMPLIB << '\n';
					cout << "Datei speichern unter: ";
					cin >> filename;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						string Ausgabe;
						vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIB(i, maxK_GMPLIB, false);

						ostringstream oss2;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<mpz_class>(oss2, "\n"));

							// Now add the last element with no delimiter
							oss2 << PositiveRunden.back();

							Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
						}
					}
					
					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Failed to open " << filename << '\n';
					}
					else {
						output_fstream << Dateiausgabe.str() << endl;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}

					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{berechnungsEnde - berechnungsStart}.count() << "s\n\n";
					break;
				case 16:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;

					cout << "Datei speichern unter: ";
					cin >> filename;

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					delN = maxN - minN + 1;

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Fehler: Failed to open " << filename << '\n';
					}
					else {
						for (size_t i = minN; i <= maxN; i++)
						{
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(minN, i, delN, delN, elapsed, 'k');

							string Ausgabe;
							vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv2(i, anz, false);

							ostringstream oss2;

							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<mpz_class>(oss2, "\n"));

							// Now add the last element with no delimiter
							oss2 << PositiveRunden.back();

							output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << endl;
						}
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					#pragma GCC diagnostic push
					#pragma GCC diagnostic ignored "-Wformat"
					dt(berechnungsStartHR, durHR);
				    #pragma GCC diagnostic pop
					cout << "Laufzeit: " << durHR << "\n\n";
					break;
				case -16:
					cout << "min n eingeben: ";
					cin >> minN;
					cout << "max n eingeben: ";
					cin >> maxN;
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv2(i, anz, false);

						ostringstream oss2;

						if (!PositiveRunden.empty())					//vetor to string
						{
							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<mpz_class>(oss2, "\n"));
							// Now add the last element with no delimiter
							oss2 << PositiveRunden.back();

							cout << "\rLampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << endl << "size: ";
						}
					}
					
					berechnungsEnde = steady_clock::now();
					cout << "Laufzeit: " << duration<double>{berechnungsEnde - berechnungsStart}.count() << "s\n\n";
					break;
				default:
					cout << "\aFehlerhafte Eingabe!\n\n";
					break;
				}
			}
			catch (const std::exception& e)
			{
				cout << "\aFehler:\n" << e.what() << endl;
			}
		}

	}
	else
	{

	unsigned long long					minN;
	unsigned long long					maxN;
	unsigned long long					maxNecht;
	unsigned long long					delN;
	unsigned long long					maxK;
#ifdef _ENABLEBIGINTS_
	//	uint1024_t							minN1024;
	//	uint1024_t							maxN1024;
	uint1024_t							maxK1024;
	uint1KB_t							maxK1KB;
	uint16KB_t							maxK16KB;
	//	uint1MB_t							maxK1MB;
#endif
	vector<vector<unsigned long long>>	vPositiveRunden(AnzThreads3);
	int									prüfart;
	double								wechselfaktorN;
	double								wechselfaktorK;
	unsigned int						minG;
	unsigned int						maxG;
	unsigned int						MaxAbweichung;

	auto								berechnungsStart = steady_clock::now();
	auto								berechnungsEnde = steady_clock::now();



	//	vector<vector<unsigned long long>>	Vectorliste;

	/*	for (size_t i = 0; i < AnzThreads; i++)
		{
			vector<unsigned long long>	defaultVector;
			Vectorliste.push_back(defaultVector);
		}*/

	while (true)
	{
		string filename;
		fstream output_fstream;

		try
		{
			cout << "Programm von Lorenz Taschner & Lars Krabbenhöft\nLampen prüfen bis (n,k)\nWelche Prüfmethode?\n0 = Beenden\n1 = Für eine große Lampenanzahl\n2 = Für eine große Rundenanzahl\n3 = wenn du deinen PC sprengen willst (ca. 2^(2^17) Runden)\n";
			cin >> prüfart;

			unsigned long long testLampen;
			Dateiausgabe.clear();


			switch (prüfart)
			{
			case 0:
				return 0;
				break;
			case 1:
				cout << "min n eingeben: ";
				cin >> minN;
				cout << "max n eingeben: ";
				cin >> maxN;
				cout << "max k eingeben (>= max n): ";
				cin >> maxK;
				cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
				cin >> testLampen;
				cout << "Wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
				cin >> wechselfaktorN;
				cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
				cin >> AnzThreads4;
				cout << "minimale Threadgröße eingeben (empfohlen 100): ";
				cin >> minG;
				cout << "maximale Threadgröße eingeben (empfohlen 500): ";
				cin >> maxG;
				cout << "maximale Abweichung eingeben: ";
				cin >> MaxAbweichung;
				cout << GeeigneteThreadgroeszenBerechnen(maxN - minN + 1, AnzThreads4, minG, maxG, MaxAbweichung);
				cout << "\n\nThreadgröße (Anzahl der n je Thread): ";
				cin >> delN;

				berechnungsStart = steady_clock::now();


				for (size_t i = minN; i <= maxN; i += AnzThreads4 * delN)
				{
					vector<future<string>>	t(AnzThreads4);
					for (size_t j = 0; j < AnzThreads4; j++)
					{
						t[j] = async(launch::async, OptimierteForm6Pack, i + j * delN, i + (j + 1) * delN - 1, maxK, testLampen, wechselfaktorN);
					}
					for (size_t j = 0; j < AnzThreads4; j++)
					{
						try
						{
							cout << t[j].get();
						}
						catch (const std::exception& e)
						{
							cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
						}
					}
					maxNecht = i + AnzThreads4 * delN - 1;
				}

				berechnungsEnde = steady_clock::now();
				cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
				break;
			case 2:
#ifdef _ENABLEBIGINTS_
				cout << "min n eingeben: ";
				cin >> minN;
				cout << "max n eingeben: ";
				cin >> maxN;
				cout << "max k eingeben: ";
				cin >> maxK1KB;
				cout << "Datei speichern unter: ";
				cin >> filename;
				//				cout << maxK1KB << '\n';

				berechnungsStart = steady_clock::now();

				for (size_t i = minN; i <= maxN; i++)
				{
					string		Ausgabe;
					vector<uint1KB_t> PositiveRunden = OP7LampenSimulieren1KB(i, maxK1KB, false);

					ostringstream oss1;
					ostringstream oss2;

					if (!PositiveRunden.empty())					//vetor to string
					{
						// Convert all but the last element to avoid a trailing ","
						copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1KB_t>(oss1, ","));
						copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint1KB_t>(oss2, "\n"));

						// Now add the last element with no delimiter
						oss1 << PositiveRunden.back();
						oss2 << PositiveRunden.back();

						cout << "Lampenanzahl: " << i << "; positive Runde(n) :" << oss1.str() << "\n";
						Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
					}

				}

				output_fstream.open(filename, ios_base::out);
				if (!output_fstream.is_open()) {
					cout << "Failed to open " << filename << '\n';
				}
				else {
					output_fstream << Dateiausgabe.str() << endl;
					cout << "Datei gespeichert!" << endl;
				}

				berechnungsEnde = steady_clock::now();
				cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
				cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
				break;

			case 3:
#ifdef _ENABLEBIGINTS_
				cout << "min n eingeben: ";
				cin >> minN;
				cout << "max n eingeben: ";
				cin >> maxN;
				cout << "Datei speichern unter: ";
				cin >> filename;

				berechnungsStart = steady_clock::now();

				for (size_t i = minN; i <= maxN; i++)
				{
					string		Ausgabe;
					vector<uint16KB_t> PositiveRunden = OP9LampenSimulieren16KB(i, false);

					ostringstream oss2;

					if (!PositiveRunden.empty())					//vetor to string
					{
						// Convert all but the last element to avoid a trailing ","
						copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<uint16KB_t>(oss2, "\n"));

						// Now add the last element with no delimiter
						oss2 << PositiveRunden.back();

						Dateiausgabe << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n";
					}

				}

				output_fstream.open(filename, ios_base::out);
				if (!output_fstream.is_open()) {
					cout << "Failed to open " << filename << '\n';
				}
				else {
					output_fstream << Dateiausgabe.str() << endl;
					cout << "Datei gespeichert!" << endl;
				}

				berechnungsEnde = steady_clock::now();
				cout << "Laufzeit: " << duration<double>{ berechnungsEnde - berechnungsStart }.count() << "s\n\n";
#else
				cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
				break;


			default:
				cout << "\aFehlerhafte Eingabe!\n\n";
				break;
			}
		}
		catch (const std::exception& e)
		{
			cout << "Fehler:\n" << e.what() << endl;
		}
	}

	}
}


// Programm ausführen: STRG+F5 oder Menüeunsigned long longrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.








