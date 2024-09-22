// Lampenproblem.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
// TODO 1kb
// TODO Terminal width
// TODO Benchmark-Tests um zu entscheiden, ob LampenSimulierenFLINT_tdiv_qr_ist_schneller
// um zu switchen, das Makro unkommentieren
#define __LS_division_variant 1
//

// Programmversion:
#define _V "0.1.27"

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

// check if flint is installed correctly
#ifndef _DISABLELIBFLINT_
	#if __has_include(<flint/fmpzxx.h>)
		#include <flint/fmpzxx.h>
	#elif __has_include(<flint/flint.h>)
		#define _DISABLELIBFLINTXX_
		#include <flint/flint.h>
	#else 
		#define _DISABLELIBFLINT_
		#define _DISABLELIBFLINTXX_
	#endif
#else
	#ifndef _DISABLELIBFLINTXX_
		#define _DISABLELIBFLINTXX_
	#endif
#endif

#include <algorithm>
#include <chrono>
#include <cinttypes>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <future>
#include <fstream>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

//#include <ncurses.h>
//#include <stdexcept>
//#include <stdio.h>
//#include <sys/ioctl.h>

#ifdef DNCURSES_WIDECHAR
	#include <ncursesw/ncurses.h>
#else
	#include <ncurses.h>
#endif

// für globale Variablen
#include "globalVars.hh"

// Für CPU-Profiler
#include "cpu.h"

// Für Veriablenspeicherung
#include "zwischenVar.h"

// für die unterschiedlichen Divisionsverfahren beim Schrittegehen an den Lampen
#ifndef _DISABLELIBFLINTXX_
	#include "LS_divionsVarianten.hh"
#endif // !ifndef _DISABLELIBFLINTXX_

// if using a progressBar
#include "progBar.h"

// für die Logdatei
#include "logfile.hh"

using namespace std;
using namespace chrono;
using namespace chrono_literals;

#ifndef _DISABLELIBFLINTXX_
	using flint::fmpzxx;
#endif // !ifndef _DISABLELIBFLINTXX_

#ifdef _ENABLEBIGINTS_
	using namespace boost::multiprecision;
#endif

#include "makrocheks.h"

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

std::string PrintLampenStatus(vector<bool> Lampen)
{
	_PRINTINPUT_3_("Funktionsaufruf: PrintLampenStatus")
	string Ausgabe = " ";
	for (bool Lampe: Lampen)
	{
		if (Lampe)	Ausgabe += "☒ ";
		else		Ausgabe += "☐ ";
	}
	_PRINTVAR_4_(Ausgabe)
	return Ausgabe;
}

string VectorenZuString(unsigned long long n, vector<vector<unsigned long long>> v)
{
	_PRINTINPUT_3_("Funktionsaufruf: VectorenZuString")
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
	_PRINTINPUT_3_("Funktionsaufruf: GeeigneteThreadgroeszenBerechnen")
	string			GuteThreadgroeszen;
	unsigned int	mod;
	unsigned int	Abweichung;

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
		}
	}

	return GuteThreadgroeszen;
}

bool isTTY(std::string TERM)
{
	_PRINTINPUT_3_("Funktionsaufruf: isTTY")
	if (TERM == "linux") return true;				// Normales TTY Terminal
	if (TERM.rfind("xterm", 0) == 0) return false;	// checks if it starts with xterm; xterm unterstützt de Unicode Block U+2500 und weitere nützliche sachen

	// check other terinals

	// if unknown, assume it is not a TTY
	return false;
}

// Funktion zur Überwachung der Nutzereingabe
void input_listener() {
    int ch;
	EingabeAktiviert = true;
    while (ch = getch())// != 'q') {
    {
        if (ch == 82) // 82 = R (Groß)
        { // 18 ist der ASCII-Code für Strg+R
            refreshScreen();
        }

		if (!EingabeAktiviert) return;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Beende ncurses-Modus wenn 'q' gedrückt wird
    endwin();
    exit(0); // Beende das Programm
}

void signalHandler(int signum)
{
	AnzInterrupts++;
	_PRINTINPUT_1_("INTERRUPT-SIGNAL RECEIVED: " << signum << "; #interrupts = " << AnzInterrupts)
	if (NachrichtenAktiviert)
	{
		// Get the current time
		time_t now = time(0);
		
		// Convert it to tm struct for local timezone
		tm* localtm = localtime(&now);

		uint64_t AnzI = AnzInterrupts.load();

		lock_cout;
		wattron(NachrichtenFenster, COLOR_PAIR(1));
		mvwprintw(NachrichtenFenster, 1, 2, "Signal %i erhalten; #interrupts = %" PRIu64 "; Zeit: %04d-%02d-%02d %02d:%02d:%02d\n",
		   signum,
		   AnzI,
           localtm->tm_year + 1900, // tm_year is years since 1900
           localtm->tm_mon + 1, // tm_mon is months since January (0-11)
           localtm->tm_mday,
           localtm->tm_hour,
           localtm->tm_min,
           localtm->tm_sec);
		wattroff(NachrichtenFenster, COLOR_PAIR(1));

		wrefresh(NachrichtenFenster);
	}

	if (signum == SIGSEGV) // = 11; jedes mal wenn auf illegalem Speicher zugegriffen wurde
	{
		// do something!
	}
	if (signum == SIGWINCH) // = 28; soll jedes mal passend den Bildschirm aktualisieren
	{
		//refreshScreen();
	} else if (AnzInterrupts > MaxInterrupts)	// Killswitch
	{
		endwin();	// end ncurses
	    std::cerr << "\033[0m\n\033[1;4;31mFehler: zu viele Interrupts erhalten.\nLetztes Signal:    " << signum << "\nVorletztes Signal: " << UserInterrupt << "\nProgramm läuft nicht weiter.\033[0m" << std::endl;
		_PRINTINPUT_1_("Fehler: zu viele Interrupts erhalten.\n\nLetztes Signal:    " << signum << "\nVorletztes Signal: " << UserInterrupt << "\nProgramm läuft nicht weiter.\n")
		exit(signum);
	} else if (signum == SIGINT && UserInterrupt != 0) {
		endwin();	// end ncurses
	    std::cerr << gotoZeileDrunter << "Signal " << UserInterrupt << " erhalten und Exekution vom Nutzer befohlen. Programm läuft nicht weiter." << std::endl;
		exit(UserInterrupt);
	} else {
		if (InterruptRequiredByApp)
		{
			std::cout << "Signal received! Stopping soon :)\r" << flush;
		} else {		
	    	if(!NachrichtenAktiviert)
			{
				std::cerr << "\nSignal " << signum << " erhalten. Programm läuft noch weiter.\nZum Anhalten bitte Strg+C drücken! (SIGINT wird benötigt)" << std::endl;
			}
		}
		UserInterrupt = signum;
		// mach etwas
	}

    // std::cerr << "\nSignal " << signum << " erhalten. Programm läuft nicht weiter." /*<< SIGRTMAX << '\t' << SIGRTMIN*/ << std::endl;
}

void CheckpointLSGv3(const std::string& ordner, const bool retrieve, unsigned long long& n_ULL, uint64_t& anz, bool& einsenAnzeigen, mpz_class& AnzRunden, vector<bool>& Lampen, vector<mpz_class>& PositiveRunden, mpz_class& Schritte, unsigned long long& Lampejetzt_ULL, int& print)
{
	_PRINTINPUT_3_("Funktionsaufruf: CheckpointLSGv3")
	if (retrieve)				// wenn true, dann wird die datei gelesen, sonst geschrieben
	{
		uint64_t n;
		uint64_t Lampejetzt;
		readVar(n);
		readVar(anz);
		readVar(einsenAnzeigen);
		readVar(AnzRunden);
		readVar(Lampen);
		readVar(PositiveRunden);
		readVar(Schritte);
		readVar(Lampejetzt);
		readVar(print);
		n_ULL = (unsigned long long)n;
		Lampejetzt_ULL = (unsigned long long)Lampejetzt;
	}
	else
	{
		erstelleVerzeichnis(ordner.c_str());
		uint64_t n = (uint64_t)n_ULL;
		uint64_t Lampejetzt = (uint64_t)Lampejetzt_ULL;
		saveVar(n);
		saveVar(anz);
		saveVar(einsenAnzeigen);
		saveVar(AnzRunden);
		saveVar(Lampen);
		saveVar(PositiveRunden);
		saveVar(Schritte);
		saveVar(Lampejetzt);
		saveVar(print);
	}
}

void CheckpointLSGv4(const std::string& ordner, const bool retrieve, unsigned long long& n_ULL, uint64_t& anz, bool& einsenAnzeigen, mpz_class& AnzRunden, vector<bool>& Lampen, vector<mpz_class>& PositiveRunden, mpz_class& Schritte, unsigned long long& Lampejetzt_ULL, unsigned long long& print_ULL)
{
	_PRINTINPUT_3_("Funktionsaufruf: CheckpointLSGv4")
	if (retrieve)				// wenn true, dann wird die datei gelesen, sonst geschrieben
	{
		uint64_t n;
		uint64_t Lampejetzt;
		uint64_t print;
		readVar(n);
		readVar(anz);
		readVar(einsenAnzeigen);
		readVar(AnzRunden);
		readVar(Lampen);
		readVar(PositiveRunden);
		readVar(Schritte);
		readVar(Lampejetzt);
		readVar(print);
		n_ULL = (unsigned long long)n;
		Lampejetzt_ULL = (unsigned long long)Lampejetzt;
		print_ULL = (unsigned long long)print;
	}
	else
	{
		erstelleVerzeichnis(ordner.c_str());
		uint64_t n = (uint64_t)n_ULL;
		uint64_t Lampejetzt = (uint64_t)Lampejetzt_ULL;
		uint64_t print = (uint64_t)print_ULL;
		saveVar(n);
		saveVar(anz);
		saveVar(einsenAnzeigen);
		saveVar(AnzRunden);
		saveVar(Lampen);
		saveVar(PositiveRunden);
		saveVar(Schritte);
		saveVar(Lampejetzt);
		saveVar(print);
	}
}

void CheckpointLSGv6(const std::string& ordner, const bool retrieve, unsigned long long& n_ULL, uint64_t& anz, bool& einsenAnzeigen, mpz_class& AnzRunden, vector<bool>& Lampen, vector<mpz_class>& PositiveRunden, mpz_class& Schritte, unsigned long long& Lampejetzt_ULL, unsigned long long& print_ULL, unsigned long long& cPrint_ULL, unsigned long long& dPrint_ULL, std::chrono::nanoseconds Laufzeit)
{
	_PRINTINPUT_3_("Funktionsaufruf: CheckpointLSGv6")
	if (retrieve)				// wenn true, dann wird die datei gelesen, sonst geschrieben
	{
		uint64_t n;
		uint64_t Lampejetzt;
		uint64_t print;
		uint64_t cPrint;
		uint64_t dPrint;
		readVar(n);
		readVar(anz);
		readVar(einsenAnzeigen);
		readVar(AnzRunden);
		readVar(Lampen);
		readVar(PositiveRunden);
		readVar(Schritte);
		readVar(Lampejetzt);
		readVar(print);
		readVar(cPrint);
		readVar(dPrint);
		readVar(Laufzeit);
		n_ULL = (unsigned long long)n;
		Lampejetzt_ULL = (unsigned long long)Lampejetzt;
		print_ULL = (unsigned long long)print;
	} else {
		erstelleVerzeichnis(ordner.c_str());
		uint64_t n = (uint64_t)n_ULL;
		uint64_t Lampejetzt = (uint64_t)Lampejetzt_ULL;
		uint64_t print = (uint64_t)print_ULL;
		uint64_t cPrint = (uint64_t)cPrint_ULL;
		uint64_t dPrint = (uint64_t)dPrint_ULL;
		saveVar(n);
		saveVar(anz);
		saveVar(einsenAnzeigen);
		saveVar(AnzRunden);
		saveVar(Lampen);
		saveVar(PositiveRunden);
		saveVar(Schritte);
		saveVar(Lampejetzt);
		saveVar(print);
		saveVar(cPrint);
		saveVar(dPrint);
		saveVar(Laufzeit);
	}
}

#ifndef _DISABLELIBFLINTXX_

void CheckpointLSF(const std::string& ordner, const bool retrieve, unsigned long long& n_ULL, uint64_t& anz, bool& einsenAnzeigen, fmpzxx& AnzRunden, vector<bool>& Lampen, vector<fmpzxx>& PositiveRunden, fmpzxx& Schritte, unsigned long long& Lampejetzt_ULL, unsigned long long& print_ULL, unsigned long long& cPrint_ULL, unsigned long long& dPrint_ULL, std::chrono::nanoseconds Laufzeit)
{
	_PRINTINPUT_3_("Funktionsaufruf: CheckpointLSF")
	if (retrieve)				// wenn true, dann wird die datei gelesen, sonst geschrieben
	{
		uint64_t n;
		uint64_t Lampejetzt;
		uint64_t print;
		uint64_t cPrint;
		uint64_t dPrint;
		readVar(n);
		readVar(anz);
		readVar(einsenAnzeigen);
		readVar(AnzRunden);
		readVar(Lampen);
		readVar(PositiveRunden);
		readVar(Schritte);
		readVar(Lampejetzt);
		readVar(print);
		readVar(cPrint);
		readVar(dPrint);
		readVar(Laufzeit);
		n_ULL = (unsigned long long)n;
		Lampejetzt_ULL = (unsigned long long)Lampejetzt;
		print_ULL = (unsigned long long)print;
	} else {
		erstelleVerzeichnis(ordner.c_str());
		uint64_t n = (uint64_t)n_ULL;
		uint64_t Lampejetzt = (uint64_t)Lampejetzt_ULL;
		uint64_t print = (uint64_t)print_ULL;
		uint64_t cPrint = (uint64_t)cPrint_ULL;
		uint64_t dPrint = (uint64_t)dPrint_ULL;
		saveVar(n);
		saveVar(anz);
		saveVar(einsenAnzeigen);
		saveVar(AnzRunden);
		saveVar(Lampen);
		saveVar(PositiveRunden);
		saveVar(Schritte);
		saveVar(Lampejetzt);
		saveVar(print);
		saveVar(cPrint);
		saveVar(dPrint);
		saveVar(Laufzeit);
	}
}

#endif // !ifndef _DISABLELIBFLINTXX_

vector<unsigned long long> LampenSimulieren(unsigned long long n, unsigned long long k, bool einsenAnzeigen)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulieren")
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

std::string LampenVerbosSimulieren(unsigned long long n, unsigned long long Anz, bool einsenAnzeigen)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenVerbosSimulieren")
	unsigned long long				AnzRunden = 1;				// Aktuelle Runde
    vector<bool>					Lampen;
	vector<unsigned long long>		PositiveRunden;				// Liste der Runden nachdem alle Lampen an/aus sind
	unsigned long long				Schritte = 0;				// Anzahl der Schritte, die schon gelaufen sind

	vector<bool>	AlleLampenAn;
	vector<bool>	AlleLampenAus;

	std::string		Ausgabe = "";

	for (size_t i = 0; i < n; i++)						// n Lampen, die aus sind erstellen
	{
		Lampen.push_back(false);
		AlleLampenAn.push_back(true);
		AlleLampenAus.push_back(false);
	}
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(1);					// 1. Runde ist immer positiv
	}

	Lampen[0] = true;									// erste Lampe der 2. Runde wird umgeschalten

	//while (1 + Schritte / n <= k)						// bis Rundenanzahl erreicht
	for (size_t i = 1; i <= Anz; i++)
	{
		Ausgabe += std::string("\nIteration: ") + to_string(i) + "\tSchritte: " + to_string(Schritte) + "\tAnzRunden: " + to_string(AnzRunden) + "\tLampen: " + PrintLampenStatus(Lampen);
		Schritte += AnzRunden;
		if (AnzRunden > n || AnzRunden < 1 + Schritte / n)
		{
			if (Lampen==AlleLampenAn||Lampen==AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);	// Runde vermerken
				Ausgabe += "\t OMG Positive Runde gefunden!!!!!!";
			}

			AnzRunden = 1 + Schritte / n;
		}
		Lampen[Schritte%n] = !Lampen[Schritte%n];		// Lampe umschalten
	}

	return Ausgabe;
}

vector<unsigned long long> LampenEinzelnPrüfen(unsigned long long n, unsigned long long maxK, unsigned long long testLampen/*, bool nurNNprüfen*/)		//testLampen <= n
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenEinzelnPrüfen")
	vector<unsigned long long>	PositiveRunden;			// Liste der Runden nachdem alle Lampen an/aus sind
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
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm1")
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
	_PRINTINPUT_5_("Funktionsaufruf: OptimierteForm2")
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
			if (Modulo[i] <= testLampen)								// Wenn keine Lampen betätigt werden, wird es übersprungen
			{
				for (size_t j = Modulo[i]; j <= testLampen; j+=i)
				{
					Lampen[j] = !Lampen[j];
				}

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

		// Berechnungswechsel

		unsigned long long		AnzRunden = Modulo.size() - 1;							// Aktuelle Runde
		unsigned long long		Schritte = n * (AnzRunden - 1) + Modulo[AnzRunden] - 1;	// Anzahl der Schritte, die schon gelaufen sind

		if (Modulo[AnzRunden]<=testLampen)
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
	_PRINTINPUT_5_("Funktionsaufruf: OptimierteForm3")
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
	_PRINTINPUT_5_("Funktionsaufruf: OptimierteForm4")
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
	_PRINTINPUT_5_("Funktionsaufruf: OptimierteForm5")
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

string OptimierteForm2Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm2Pack")
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm2(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm3Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm3Pack")
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm3(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm4Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN, double wechselfaktorK)
{
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm4Pack")
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm4(i, maxK, testLampen, wechselfaktorN, wechselfaktorK));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm5Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN)
{
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm5Pack")
	vector<vector<unsigned long long>>	output;
	for (size_t i = minN; i <= maxN; i++)
	{
		output.push_back(OptimierteForm5(i, maxK, testLampen, wechselfaktorN));
	}
	return VectorenZuString(minN, output);
}

string OptimierteForm6Pack(unsigned long long minN, unsigned long long maxN, unsigned long long maxK, unsigned long long testLampen, double wechselfaktorN)
{
	_PRINTINPUT_3_("Funktionsaufruf: OptimierteForm6Pack")
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

		}
		output.push_back(PositiveRunden);
	}
	return VectorenZuString(minN, output);
}

#ifdef _ENABLEBIGINTS_

vector<uint1024_t> LampenSimulieren1024(unsigned long long n, uint1024_t k, bool einsenAnzeigen)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulieren1024")
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
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulieren1KB")
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
	_PRINTINPUT_3_("Funktionsaufruf: OP7LampenSimulieren1KB")
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
	_PRINTINPUT_3_("Funktionsaufruf: OP8LampenSimulieren16KB")
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
	_PRINTINPUT_3_("Funktionsaufruf: OP9LampenSimulieren16KB")
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

#endif // !ifdef _ENABLEBIGINTS_

vector<mpz_class> LampenSimulierenGMPLIB(unsigned long long n, mpz_class k, bool einsenAnzeigen)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIB")
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				Schritte(tmp_n_gmplib);
	mpz_class				n_gmplib(tmp_n_gmplib);
    unsigned long long		Lampejetzt;


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

vector<mpz_class> LampenSimulierenGMPLIBv2(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv2")
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				Schritte(tmp_n_gmplib);
	const mpz_class			n_gmplib(tmp_n_gmplib);
    unsigned long long		Lampejetzt;
	unsigned long long		print = 0;
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
		if (print % 1048576 == 0)
		{
			CheckpointLSGv4(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print);
			berechnungsZwCP_HR = berechnungsEndeHR;
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);
			cout << "    \r \033[91mRAM: " << giveRAM('k') << "  \033[96mIteration: " << to_string(print) << "  \033[95mSchritte: " << mpz_sizeinbase(Schritte.get_mpz_t(), 265) << " Bytes  \033[93mZeit: " << durHR << "\033[0m  \033[2;93mΔt: " << CP_HR << "  \033[3;93mdt/dn: " << CPdHR << "\033[0m             \r" << flush;
		}
    }
	cout << endl;
    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv3(string Session)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv3")
	unsigned long long		n;
	uint64_t				anz;
	bool					einsenAnzeigen;
    mpz_class				AnzRunden = 2;
	vector<bool>			Lampen(n, true);
    vector<mpz_class>		PositiveRunden;
	mpz_class				Schritte(1729);
    unsigned long long		Lampejetzt;
	unsigned long long		print = 0;

	CheckpointLSGv4(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print);

	mpz_t					tmp_n_gmplib;
	mpz_init_set_ui			(tmp_n_gmplib, n);
	mpz_class				n_gmplib(tmp_n_gmplib);

	//debugausgaben
	//std::cout << Session << '\t' << n << '\t' << anz << '\t' << einsenAnzeigen << '\t' << AnzRunden << '\t' << Schritte << '\t' << n_gmplib << '\t' << Lampejetzt << '\t' << print << std::endl;

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
		if (print % 1048576 == 0)
		{
			CheckpointLSGv4(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print);
			berechnungsZwCP_HR = berechnungsEndeHR;
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);
			cout << "    \r \033[91mRAM: " << giveRAM('k') << "  \033[96mIteration: " << to_string(print) << "  \033[95mSchritte: " << mpz_sizeinbase(Schritte.get_mpz_t(), 265) << " Bytes  \033[93mZeit: " << durHR << "\033[0m  \033[2;93mΔt: " << CP_HR << "  \033[3;93mdt/dn: " << CPdHR << "\033[0m             \r" << flush;
		}
    }
	cout << endl;
    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv4(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv4")
    mpz_class AnzRunden = 2;
    vector<bool> Lampen(n, true);
    vector<mpz_class> PositiveRunden;
    mpz_t tmp_n_gmplib;
    mpz_init_set_ui(tmp_n_gmplib, n);
    mpz_class Schritte(tmp_n_gmplib);
    const mpz_class n_gmplib(tmp_n_gmplib);
    unsigned long long Lampejetzt;
    unsigned long long print = 0;
    auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
    auto berechnungsEndeHR = berechnungsStartHR;
    auto berechnungsZwCP_HR = berechnungsStartHR;
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

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

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

    Lampen[0] = false;

    while (AnzPR < anz)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
            }

            AnzRunden = 1 + Schritte / n_gmplib;
            bool nonsense = Lampen[1];
        }
        Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

        print++;

        if (print % 1048576 == 0)
        {
            CheckpointLSGv4(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print);
            berechnungsZwCP_HR = berechnungsEndeHR;
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);

            // Redirect output to the ncurses window
			
			{
				lock_cout;

				wattron(outputWin, A_BOLD);  		// Fett

				wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
				mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
				wattron(outputWin, A_DIM);          // Halbdurchsichtig
				mvwprintw(outputWin, 2, 30, "dt: %s       ", CP_HR.c_str());
				wattron(outputWin, A_ITALIC);       // Kursiv
				mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
				wattroff(outputWin, A_DIM);
				wattroff(outputWin, A_ITALIC);
				wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
				mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
				mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
				wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
				mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
				wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
				mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
				wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

				wattroff(outputWin, A_BOLD);

				wrefresh(outputWin);
			}
		}
    }

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	dt(berechnungsZwCP_HR, CP_HR);
	pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);
	}

    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv5(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv5")
    mpz_class AnzRunden = 2;
    vector<bool> Lampen(n, true);
    vector<mpz_class> PositiveRunden;
    mpz_t tmp_n_gmplib;
    mpz_init_set_ui(tmp_n_gmplib, n);
    mpz_class Schritte(tmp_n_gmplib);
    const mpz_class n_gmplib(tmp_n_gmplib);
    unsigned long long Lampejetzt;
    unsigned long long print = 0;
    auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
    auto berechnungsEndeHR = berechnungsStartHR;
    auto berechnungsZwCP_HR = berechnungsStartHR;
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

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

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

    Lampen[0] = false;

    while (AnzPR < anz)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
            }

            AnzRunden = 1 + Schritte / n_gmplib;
            bool nonsense = Lampen[1];
        }
        Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

        print++;

        if (print % 1048576 == 0)
        {
            CheckpointLSGv4(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print);
            berechnungsZwCP_HR = berechnungsEndeHR;
			dt(berechnungsStartHR, durHR);
			dt(berechnungsZwCP_HR, CP_HR);
			ddt(berechnungsZwCP_HR, CPdHR);

            // Redirect output to the ncurses window
			
			{
				lock_cout;

				wattron(titelWin, A_BOLD);  		// Fett
				wattron(outputWin, A_BOLD);  		// Fett

				wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
				mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
				wattron(outputWin, A_DIM);          // Halbdurchsichtig
				if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
				else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
				wattron(outputWin, A_ITALIC);       // Kursiv
				mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
				wattroff(outputWin, A_DIM);
				wattroff(outputWin, A_ITALIC);
				wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
				mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
				mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
				wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
				mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
				wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

				wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
				mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
				wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

				wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
				printCurrentTime(titelWin, timerOrty, timerOrtx);
				wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

				wattroff(titelWin, A_BOLD);
				wattroff(outputWin, A_BOLD);

				wrefresh(titelWin);
				wrefresh(outputWin);
			}
		}
    }

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	dt(berechnungsZwCP_HR, CP_HR);
	pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv6(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv6")
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
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSGv6(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
		berechnungsStartHR -= Laufzeit;
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

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

    Lampen[0] = false;

    while (AnzPR < anz)
    {
        Schritte += AnzRunden;
        if (AnzRunden > n_gmplib || AnzRunden < 1 + Schritte / n_gmplib)
        {
            if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
            {
                PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
            }

            AnzRunden = 1 + Schritte / n_gmplib;
        }
        Lampejetzt = mpz_fdiv_ui(Schritte.get_mpz_t(), n);
        Lampen[Lampejetzt] = !Lampen[Lampejetzt];

        print++;

		if(print % 32768 == 0)
		{
			if (increasedBackupFrequency || print % 1048576 == 0)
			{
				dPrint = print - cPrint;
				cPrint = print;
				Laufzeit = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
				if(!increasedBackupFrequency) increasedBackupFrequency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsEndeHR).count() >= 7200'000'000'000; // wenn die Zwischenzeit länger als 2 Stunden sind
				CheckpointLSGv6(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
				berechnungsZwCP_HR = berechnungsEndeHR;
				dt(berechnungsStartHR, durHR);
				dt(berechnungsZwCP_HR, CP_HR);
				Pdt(berechnungsZwCP_HR, CPdHR);

				// Redirect output to the ncurses window
				
				{
					lock_cout;

					wattron(titelWin, A_BOLD);  		// Fett
					wattron(outputWin, A_BOLD);  		// Fett

					wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
					mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
					wattron(outputWin, A_DIM);          // Halbdurchsichtig
					if(tty) mvwprintw(outputWin, 2, 30, "dt: %s    ", CP_HR.c_str());				// tty unterstützt nicht so viele unicode zeichen
					else    mvwprintw(outputWin, 2, 30, "Δt: %s    ", CP_HR.c_str());
					// if(tty) mvwprintw(outputWin, 2, 30, "dn: %llu dt: %s", dPrint, CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
					// else    mvwprintw(outputWin, 2, 30, "Δn: %llu Δt: %s", dPrint, CP_HR.c_str());
					wattron(outputWin, A_ITALIC);       // Kursiv
					mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
					wattroff(outputWin, A_DIM);
					wattroff(outputWin, A_ITALIC);
					wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
					mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
					mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
					wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
					mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
					mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
					wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

					wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
					printCurrentTime(titelWin, timerOrty, timerOrtx);
					wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

					wattroff(titelWin, A_BOLD);
					wattroff(outputWin, A_BOLD);

					wrefresh(titelWin);
					wrefresh(outputWin);
				}
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	Pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		// if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		// else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s      ", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
		mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

    return PositiveRunden;
}

vector<mpz_class> LampenSimulierenGMPLIBv7(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenGMPLIBv7")
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
    string durHR;
    string CP_HR;
    string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSGv6(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
		berechnungsStartHR -= Laufzeit;
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

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

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
				Laufzeit = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
				if(!increasedBackupFrequency) increasedBackupFrequency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsEndeHR).count() >= 7200'000'000'000; // wenn die Zwischenzeit länger als 2 Stunden sind
				CheckpointLSGv6(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
				berechnungsZwCP_HR = berechnungsEndeHR;
				dt(berechnungsStartHR, durHR);
				dt(berechnungsZwCP_HR, CP_HR);
				Pdt(berechnungsZwCP_HR, CPdHR);

				// Redirect output to the ncurses window
				
				{
					lock_cout;

					wattron(titelWin, A_BOLD);  		// Fett
					wattron(outputWin, A_BOLD);  		// Fett

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titel
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
					mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
					wattron(outputWin, A_DIM);          // Halbdurchsichtig
					if(tty) mvwprintw(outputWin, 2, 30, "dt: %s    ", CP_HR.c_str());				// tty unterstützt nicht so viele unicode zeichen
					else    mvwprintw(outputWin, 2, 30, "Δt: %s    ", CP_HR.c_str());
					// if(tty) mvwprintw(outputWin, 2, 30, "dn: %llu dt: %s", dPrint, CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
					// else    mvwprintw(outputWin, 2, 30, "Δn: %llu Δt: %s", dPrint, CP_HR.c_str());
					wattron(outputWin, A_ITALIC);       // Kursiv
					mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
					wattroff(outputWin, A_DIM);
					wattroff(outputWin, A_ITALIC);
					wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
					mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
					mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
					wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
					mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
					mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
					wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

					wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
					printCurrentTime(titelWin, timerOrty, timerOrtx);
					wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

					wattroff(titelWin, A_BOLD);
					wattroff(outputWin, A_BOLD);

					wrefresh(titelWin);
					wrefresh(outputWin);
				}
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	Pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		// if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		// else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s      ", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
		mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

    return PositiveRunden;
}

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

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

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
				
				{
					lock_cout;

					wattron(titelWin, A_BOLD);  		// Fett
					wattron(outputWin, A_BOLD);  		// Fett

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titel
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
					mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
					wattron(outputWin, A_DIM);          // Halbdurchsichtig
					if(tty) mvwprintw(outputWin, 2, 30, "dt: %s    ", CP_HR.c_str());				// tty unterstützt nicht so viele unicode zeichen
					else    mvwprintw(outputWin, 2, 30, "Δt: %s    ", CP_HR.c_str());
					// if(tty) mvwprintw(outputWin, 2, 30, "dn: %llu dt: %s", dPrint, CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
					// else    mvwprintw(outputWin, 2, 30, "Δn: %llu Δt: %s", dPrint, CP_HR.c_str());
					wattron(outputWin, A_ITALIC);       // Kursiv
					mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
					wattroff(outputWin, A_DIM);
					wattroff(outputWin, A_ITALIC);
					wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
					mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
					mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
					wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
					mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
					mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
					wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

					wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
					printCurrentTime(titelWin, timerOrty, timerOrtx);
					wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

					wattroff(titelWin, A_BOLD);
					wattroff(outputWin, A_BOLD);

					wrefresh(titelWin);
					wrefresh(outputWin);
				}
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	zdt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	zPdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		// if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		// else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s      ", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", mpz_sizeinbase(Schritte.get_mpz_t(), 265));
		mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

    return PositiveRunden;
}

#ifndef _DISABLELIBFLINTXX_

vector<fmpzxx> LampenSimulierenFLINT(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenFLINT")
	fmpzxx AnzRunden(2);
	vector<bool> Lampen(n, true);
	vector<fmpzxx> PositiveRunden;
	fmpzxx Schritte(n);
	const fmpzxx n_flintlib(n);
	unsigned long long Lampejetzt;
	unsigned long long print = 0;		// Anz bereits durchgeführter Iterationen
	unsigned long long cPrint = 0;		// Checkpoint für print
	unsigned long long dPrint = 0;		// Anz Iterationen zwischen den 2 letzten Sicherungen
	bool increasedBackupFrequency = false;
	auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
	auto berechnungsEndeHR = berechnungsStartHR;
	auto berechnungsZwCP_HR = berechnungsStartHR;
	std::chrono::nanoseconds Laufzeit;
	string durHR;
	string CP_HR;
	string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSF(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
		berechnungsStartHR -= Laufzeit;
	}

	vector<bool> AlleLampenAn(n, true);
	vector<bool> AlleLampenAus(n, false);

	PositiveRunden.reserve(anz);
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(fmpzxx{1});
	}

	start_color();  // Aktiviert die Farbunterstützung
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_BLUE, COLOR_BLACK);

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

	Lampen[0] = false;

	while (AnzPR < anz)
	{
		Schritte += AnzRunden;
		if (AnzRunden > n_flintlib || AnzRunden < 1 + Schritte / n_flintlib)
		{
			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
			}

			AnzRunden = 1 + Schritte / n_flintlib;
		}
		Lampejetzt = fmpz_fdiv_ui(Schritte._fmpz(), n);
		Lampen[Lampejetzt] = !Lampen[Lampejetzt];

		print++;

		if(print % 32768 == 0)
		{
			if (increasedBackupFrequency || print % 1048576 == 0)
			{
				dPrint = print - cPrint;
				cPrint = print;
				Laufzeit = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
				if(!increasedBackupFrequency) increasedBackupFrequency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsEndeHR).count() >= 7200'000'000'000; // wenn die Zwischenzeit länger als 2 Stunden sind
				CheckpointLSF(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
				berechnungsZwCP_HR = berechnungsEndeHR;
				dt(berechnungsStartHR, durHR);
				dt(berechnungsZwCP_HR, CP_HR);
				Pdt(berechnungsZwCP_HR, CPdHR);

				// Redirect output to the ncurses window
				
				{
					lock_cout;

					wattron(titelWin, A_BOLD);  		// Fett
					wattron(outputWin, A_BOLD);  		// Fett

					wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
					mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
					wattron(outputWin, A_DIM);          // Halbdurchsichtig
					if(tty) mvwprintw(outputWin, 2, 30, "dt: %s    ", CP_HR.c_str());				// tty unterstützt nicht so viele unicode zeichen
					else    mvwprintw(outputWin, 2, 30, "Δt: %s    ", CP_HR.c_str());
					// if(tty) mvwprintw(outputWin, 2, 30, "dn: %llu dt: %s", dPrint, CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
					// else    mvwprintw(outputWin, 2, 30, "Δn: %llu Δt: %s", dPrint, CP_HR.c_str());
					wattron(outputWin, A_ITALIC);       // Kursiv
					mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
					wattroff(outputWin, A_DIM);
					wattroff(outputWin, A_ITALIC);
					wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
					mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
					mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
					wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
					mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", fmpz_sizeinbase(Schritte._fmpz(), 265));
					mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
					wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

					wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
					printCurrentTime(titelWin, timerOrty, timerOrtx);
					wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

					wattroff(titelWin, A_BOLD);
					wattroff(outputWin, A_BOLD);

					wrefresh(titelWin);
					wrefresh(outputWin);
				}
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	Pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		// if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		// else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s      ", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", fmpz_sizeinbase(Schritte._fmpz(), 265));
		mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

	return PositiveRunden;
}

vector<fmpzxx> LampenSimulierenFLINTv2(unsigned long long n, uint64_t anz, bool einsenAnzeigen, string Session, WINDOW* outputWin, WINDOW* titelWin, int timerOrtx, int timerOrty, const bool& tty)
{
	_PRINTINPUT_3_("Funktionsaufruf: LampenSimulierenFLINTv2")
	fmpzxx AnzRunden(2);
	vector<bool> Lampen(n, true);
	vector<fmpzxx> PositiveRunden;
	fmpzxx Schritte(n);
	const fmpzxx n_flintlib(n);
	unsigned long long Lampejetzt;
	unsigned long long print = 0;		// Anz bereits durchgeführter Iterationen
	unsigned long long cPrint = 0;		// Checkpoint für print
	unsigned long long dPrint = 0;		// Anz Iterationen zwischen den 2 letzten Sicherungen
	bool increasedBackupFrequency = false;
	bool AnzRunden_vs_n = false;	// ob AnzRunden größer als n ist
	auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
	auto berechnungsEndeHR = berechnungsStartHR;
	auto berechnungsZwCP_HR = berechnungsStartHR;
	std::chrono::nanoseconds Laufzeit;
	string durHR;
	string CP_HR;
	string CPdHR;
	uint64_t AnzPR = 0;		// = PositiveRunden.size(), aber ist effizienter

	if (std::filesystem::exists(Session))
	{
		// Lade die Vorherige Session falls eine existiert
		CheckpointLSF(Session, true, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
		berechnungsStartHR -= Laufzeit;
	}

	vector<bool> AlleLampenAn(n, true);
	vector<bool> AlleLampenAus(n, false);

	PositiveRunden.reserve(anz);
	if (einsenAnzeigen)
	{
		PositiveRunden.push_back(fmpzxx{1});
	}

	start_color();  // Aktiviert die Farbunterstützung
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
	init_pair(6, COLOR_BLUE, COLOR_BLACK);

	{
		lock_cout;
		wattron(outputWin, A_BOLD);			// Fett
		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern; Indikator für den Start
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren
		wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 2, 76, "AnzPR: 0");						// Anzahl der bereits gefundendn positiver Runden (hier: 0)
		wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren
		wattroff(outputWin, A_BOLD);
		wrefresh(outputWin);
	}

	Lampen[0] = false;

	while (AnzPR < anz)
	{
		Schritte += AnzRunden;
		//if (AnzRunden > n_flintlib)
		if (AnzRunden_vs_n)
		{
			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
			}

			// optimize that
			// TODO braucht Benchmarktests
			#if __LS_division_variant == 0
				AnzRunden = 1 + Schritte / n_flintlib;
				Lampejetzt = fmpz_tdiv_ui(Schritte._fmpz(), n);
			#elif __LS_division_variant == 1
				fmpzxx Lj; // = neu Lampejetzt
				fmpz_tdiv_qr(AnzRunden._fmpz(), Lj._fmpz(), Schritte._fmpz(), n_flintlib._fmpz());
				AnzRunden += 1;
				Lampejetzt = fmpz_get_ui(Lj._fmpz());
			#elif __LS_division_variant == 2
				Lampejetzt = mpz_tdiv_q_ui((mpz_ptr)AnzRunden._fmpz(), (mpz_srcptr)Schritte._fmpz(), n);
				AnzRunden += 1;
			#elif __LS_division_variant == 3
				fmpz_tdiv_q_ui(AnzRunden._fmpz(), Schritte._fmpz(), n);
				AnzRunden += 1;
				Lampejetzt = fmpz_tdiv_ui(Schritte._fmpz(), n);
			#endif
		}
		//else if (AnzRunden > n_flintlib || AnzRunden < 1 + Schritte / n_flintlib)
		else if(AnzRunden < 1 + Schritte / n_flintlib)
		{
			AnzRunden_vs_n = AnzRunden > n_flintlib;
			if (Lampen == AlleLampenAn || Lampen == AlleLampenAus)
			{
				PositiveRunden.push_back(AnzRunden);
				AnzPR = PositiveRunden.size();
			}
			Lampejetzt = fmpz_fdiv_ui(Schritte._fmpz(), n);
		}
		Lampen[Lampejetzt] = !Lampen[Lampejetzt];			// Lampe umschalten

		print++;

		if(print % 32768 == 0)
		{
			if (increasedBackupFrequency || print % 1048576 == 0)
			{
				dPrint = print - cPrint;
				cPrint = print;
				Laufzeit = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
				if(!increasedBackupFrequency) increasedBackupFrequency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsEndeHR).count() >= 7200'000'000'000; // wenn die Zwischenzeit länger als 2 Stunden sind
				CheckpointLSF(Session, false, n, anz, einsenAnzeigen, AnzRunden, Lampen, PositiveRunden, Schritte, Lampejetzt, print, cPrint, dPrint, Laufzeit);
				berechnungsZwCP_HR = berechnungsEndeHR;
				dt(berechnungsStartHR, durHR);
				dt(berechnungsZwCP_HR, CP_HR);
				Pdt(berechnungsZwCP_HR, CPdHR);

				// Redirect output to the ncurses window
				
				{
					lock_cout;

					wattron(titelWin, A_BOLD);  		// Fett
					wattron(outputWin, A_BOLD);  		// Fett

					wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
					mvwprintw(outputWin, 2, 2, "Zeit: %s      ", durHR.c_str());
					wattron(outputWin, A_DIM);          // Halbdurchsichtig
					if(tty) mvwprintw(outputWin, 2, 30, "dt: %s    ", CP_HR.c_str());				// tty unterstützt nicht so viele unicode zeichen
					else    mvwprintw(outputWin, 2, 30, "Δt: %s    ", CP_HR.c_str());
					// if(tty) mvwprintw(outputWin, 2, 30, "dn: %llu dt: %s", dPrint, CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
					// else    mvwprintw(outputWin, 2, 30, "Δn: %llu Δt: %s", dPrint, CP_HR.c_str());
					wattron(outputWin, A_ITALIC);       // Kursiv
					mvwprintw(outputWin, 2, 55, "dt/dn: %s    ", CPdHR.c_str());
					wattroff(outputWin, A_DIM);
					wattroff(outputWin, A_ITALIC);
					wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(1));  // Rot auf Schwarz
					mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, AnzPR);	// Anzahl der bereits gefundendn positiver Runden
					mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
					wattroff(outputWin, COLOR_PAIR(1)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
					mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
					wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

					wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
					mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", fmpz_sizeinbase(Schritte._fmpz(), 265));
					mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
					wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

					wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
					printCurrentTime(titelWin, timerOrty, timerOrtx);
					wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren

					wattroff(titelWin, A_BOLD);
					wattroff(outputWin, A_BOLD);

					wrefresh(titelWin);
					wrefresh(outputWin);
				}
			}
		}
	}

	dPrint = print - cPrint;
	cPrint = print;

	berechnungsZwCP_HR = berechnungsEndeHR;
	dt(berechnungsStartHR, durHR);
	// dt(berechnungsZwCP_HR, CP_HR);
	Pdt(berechnungsZwCP_HR, CPdHR);

	{
		lock_cout;

		wattron(outputWin, COLOR_PAIR(4));  // Gelb auf Schwarz
		mvwprintw(outputWin, 2, 2, "Zeit: %s", durHR.c_str());
		wattron(outputWin, A_DIM);          // Halbdurchsichtig
		// if(tty) mvwprintw(outputWin, 2, 30, "dt: %s", CP_HR.c_str());	// tty unterstützt nicht so viele unicode zeichen
		// else    mvwprintw(outputWin, 2, 30, "Δt: %s", CP_HR.c_str());
		wattron(outputWin, A_ITALIC);       // Kursiv
		mvwprintw(outputWin, 2, 55, "dt/dn: %s      ", CPdHR.c_str());
		wattroff(outputWin, A_DIM);
		wattroff(outputWin, A_ITALIC);
		wattroff(outputWin, COLOR_PAIR(4)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(5));  // Grün auf Schwarz
		mvwprintw(outputWin, 2, 76, "AnzPR: %" PRIu64, anz);				// Anzahl der bereits gefundendn positiver Runden (hier: alle)
		mvwprintw(outputWin, 1, 2, "RAM: %s", giveRAM('k').c_str());
		mvwprintw(outputWin, 0, 2, " n = %llu ", n);					// Titelfarbe ändern
		wattroff(outputWin, COLOR_PAIR(5)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(2));  // Cyan auf Schwarz
		mvwprintw(outputWin, 1, 20, "Iteration: %llu", print);
		wattroff(outputWin, COLOR_PAIR(2)); // Farbe deaktivieren

		wattron(outputWin, COLOR_PAIR(3));  // Magenta auf Schwarz
		mvwprintw(outputWin, 1, 45, "Schritte: %ld Bytes", fmpz_sizeinbase(Schritte._fmpz(), 265));
		mvwprintw(outputWin, 2, 86, "CPU-Zeit: %s", CPUProfiler::cpuTimeStr().c_str());
		wattroff(outputWin, COLOR_PAIR(3)); // Farbe deaktivieren

		wrefresh(outputWin);

		wattron(titelWin, A_BOLD);  		// Fett
		wattron(titelWin, COLOR_PAIR(6));	// Blau auf Schwarz
		printCurrentTime(titelWin, timerOrty, timerOrtx);
		wattroff(titelWin, COLOR_PAIR(6));	// Farbe deaktivieren
		wattroff(titelWin, A_BOLD);

		wrefresh(titelWin);
	}

	return PositiveRunden;
}

// @return Anzahl der bits die es geprüft hat
uint64_t Benchmarking(std::string Logdatei, unsigned long long n, uint64_t Start, uint64_t batchSize)
{
	_PRINTINPUT_3_("Funktionsaufruf: Benchmarking")
	const fmpzxx n_flintlib(n);
	fmpz_init_set_ui(global_puffer_fmpz_t5, n);
	string durHR;
	string durCPU;
	uint64_t currentSchritteBits = Start;
	InterruptRequiredByApp = true;
	UserInterrupt = 0;
	while (UserInterrupt == 0)
	{
		uint64_t minSchritteBits = currentSchritteBits;
		currentSchritteBits += batchSize;
		uint64_t maxSchritteBits = currentSchritteBits;
		unsigned long long Lampejetzt;
		mpz_init(global_puffer_mpz_t1);
		mpz_init(global_puffer_mpz_t2);
		mpz_init(global_puffer_mpz_t3);
		fmpz_init(global_puffer_fmpz_t4);

		for (size_t f = 0; f < anzLSvarianten; f++)
		{
			fmpzxx Schritte; fmpz_set_d_2exp(Schritte._fmpz(), 1, minSchritteBits);
			fmpzxx maxSchritte; fmpz_set_d_2exp(maxSchritte._fmpz(), 1, maxSchritteBits);
			fmpzxx AnzRunden(0);
			fmpz_get_mpz(global_puffer_mpz_t1, (AnzRunden)._fmpz());
			fmpz_get_mpz(global_puffer_mpz_t2, (Schritte)._fmpz());
			fmpz_get_mpz(global_puffer_mpz_t3, (maxSchritte)._fmpz());
			fmpz_init_set(global_puffer_fmpz_t1, (AnzRunden)._fmpz());
			fmpz_init_set(global_puffer_fmpz_t2, (Schritte)._fmpz());
			fmpz_init_set(global_puffer_fmpz_t3, (maxSchritte)._fmpz());
			mpz_class tmp1(global_puffer_mpz_t1);
			mpz_class tmp2(global_puffer_mpz_t2);
			mpz_class tmp3(global_puffer_mpz_t3);
			global_puffer_mpz_c1 = tmp1;
			global_puffer_mpz_c2 = tmp2;
			global_puffer_mpz_c3 = tmp3;
			auto berechnungsStartHR = std::chrono::high_resolution_clock::now();
			auto berechnungsStartCPU = CPUProfiler::cpuTimeTs();
				while (LSvarianten[f](&n, &n_flintlib, &AnzRunden, &Schritte, &Lampejetzt, &maxSchritte));
			auto berechnungsEndeHR = std::chrono::high_resolution_clock::now();
			durCPU = CPUProfiler::cpuTimeStrDiff(berechnungsStartCPU);
			dt(berechnungsStartHR, durHR);
			fmpz_clear(global_puffer_fmpz_t1);
			fmpz_clear(global_puffer_fmpz_t2);
			fmpz_clear(global_puffer_fmpz_t3);
			cout << "[Benchmark] Bits: " << minSchritteBits << " - " << maxSchritteBits << "\tVariante: " << f << "\tZeit (Wanduhr): " << durHR << "\tZeit (CPU): " << durCPU << endl;
		}
		mpz_clear(global_puffer_mpz_t1);
		mpz_clear(global_puffer_mpz_t2);
		mpz_clear(global_puffer_mpz_t3);
		fmpz_clear(global_puffer_fmpz_t4);
		cout << endl;
	}
	fmpz_clear(global_puffer_fmpz_t5);
	UserInterrupt = 0;
	InterruptRequiredByApp = false;
	return currentSchritteBits;
}

#endif // !ifndef _DISABLELIBFLINTXX_

int main(int argc, const char** argv)
{
	Starttime = std::chrono::steady_clock::now();

	// Signal-Handler initialisieren
	for(int i = 0; i <= SIGRTMAX; i++)
	{
		signal(i, signalHandler);
	}

	_LOGFILEINIT_();

	ostringstream						Dateiausgabe;

    const char*							termType				= std::getenv("TERM");				_PRINTVAR_2_(termType)		// Terminal-Typ
	const char*							Sprache					= std::getenv("LANG");				// Terminal-Sprache
	const bool							tty						= isTTY(termType);					// Ob es ein TTY Terminal oder eine Terminal-App ist
	char								usePresetLang;												// Ob die vorgegebene Sprache benutzt werden soll

	ttyGlobal = tty; // Aktualisiere globale Variable
	cout << "Language detected: \"" << Sprache << "\" Use this language? (y/n) ";
	cin >> usePresetLang;
	if(usePresetLang!='n')
	{
		_PRINTINPUT_2_("Language: " << Sprache)
		// Setze die Locale auf die gewünschte Sprache (z.B. "de_DE.UTF-8")
		std::locale::global(std::locale(Sprache));
	} else {
		string alternateLang;
		cout << "enter alternative language: ";
		cin >> alternateLang;
		_PRINTINPUT_2_("Language: " << alternateLang)
		std::locale::global(std::locale(alternateLang));
	}


//	const auto							AnzThreadsUnterstützt	= thread::hardware_concurrency;		// soviele threads wie CPU-Kerne
	const auto							AnzThreadsUnterstützt	= cpuCores;							// soviele threads wie CPU-Kerne
	long long							AnzThreads3;
//	unsigned long long					AnzThreads4;												// Anzahl der Threads für case 4+
	unsigned int						AnzThreads4;												// Anzahl der Threads für case 4+
	uint64_t							finishedThreads;											// Anzahl der bereits fertigen Threads

	// Unicode-Zeichen für abgerundete Ecken
	setcchar(&ls, L"│", 0, 0, NULL);
	setcchar(&rs, L"│", 0, 0, NULL);
	setcchar(&ts, L"─", 0, 0, NULL);
	setcchar(&bs, L"─", 0, 0, NULL);
	setcchar(&tl, L"╭", 0, 0, NULL);
	setcchar(&tr, L"╮", 0, 0, NULL);
	setcchar(&bl, L"╰", 0, 0, NULL);
	setcchar(&br, L"╯", 0, 0, NULL);


	cout << "Version " << _V << " Compiled on: " << __DATE__ << ' ' << __TIME__ << "\nTerminal Typ: " << termType << '\n'
	     << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads für Prüfmethode Nr.3 eingeben: ";
	cin >> AnzThreads3;					_PRINTVAR_2_(AnzThreads3)

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
		double								Dauer;
		string								durHR;

		double								diffN;

		uint64_t							AnzThreads;

		bool								FileUnopenable;

		//	vector<vector<unsigned long long>>	Vectorliste;

		/*	for (size_t i = 0; i < AnzThreads; i++)
			{
				vector<unsigned long long>	defaultVector;
				Vectorliste.push_back(defaultVector);
			}*/

		while (true)
		{
			string Session;
			string filename;
			fstream output_fstream;

			try
			{
				const std::string menu = std::string("Programm von Lorenz Taschner & Lars Krabbenhöft\n")
					+ "Lampen prüfen bis (n,k)\n"
					+ "Welche Prüfmethode?\n"
					+ "0  = Beenden\t\t\t1  = Simulation\t\t\t\t\t2  = einzelne Lampen Testen\n"
					+ "3  = optimierte Version Nr.1\t4  = optimierte Version Nr.2\t\t\t5  = optimierte & erweiterte Simulation Nr.1\n"
					+ "6  = optimierte Version Nr.3\t7  = optimierte & erweiterte Simulation Nr.2\t8  = optimierte Version Nr.4\n"
					+ "9  = optimierte Version Nr.5\t10 = optimierte Version Nr.6\t\t\t11 = optimierte & erweiterte Simulation Nr.3\n"
					+ "12 = optimierte Version Nr.6.2\t13 = optimierte & erweiterte Simulation Nr.4\t14 = optimierte & erweiterte Simulation Nr.5\n"
					+ "15 = optimierte & erweiterte Simulation mit GMPLIB\t\t16 = optimierte & erweiterte Simulation mit GMPLIB V2\n"
					+ "17 = optimierte & erweiterte Simulation mit GMPLIB V2 - Zwischenstand laden\t18 = 16, aber multithreaded\n"
					+ "19 = 16, aber + ncurses & async\t20 = optimierte & erweiterte Simulation mit GMPLIB V5\n"
					+ "21 = 20, aber rückwärts\t\t22 = optimierte & erweiterte Simulation mit GMPLIB V6\n"
					+ "23 = optimierte & erweiterte Simulation mit FLINT\t\t24 = optimierte & erweiterte Simulation mit GMPLIB V7\n"
					+ "25 = optimierte & erweiterte Simulation mit GMPLIB V8 (Jetzt mit Nachrichtenfenster!)\n"
					+ "\n-1 = Benchmark für die Schritte\t-2 = Verbose simulation\n\n"
					+ '\n';
				cout << menu;
				cin >> prüfart;					_PRINTVAR_2_(prüfart)

				unsigned long long testLampen;
				Dateiausgabe.clear();


				switch (prüfart)
				{
				case 0:
					endwin();	// end ncurses
					return 0;
					break;
				case 1:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK;				_PRINTVAR_4_(maxK)

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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
					break;
				case -2:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "Anz. Iterationen eingeben: ";
					cin >> maxK;				_PRINTVAR_4_(maxK)

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{

						cout << LampenVerbosSimulieren(i, maxK, false) << "\n\n";

					}

					berechnungsEnde = steady_clock::now();
					//			cout << "Laufzeit: " << nanoseconds{ berechnungsEnde - berechnungsStart }.count() << "ns\n";
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
					break;
				case -1:
					#ifndef _DISABLELIBFLINTXX_
						cout << "n eingeben: ";
						cin >> minN;				_PRINTVAR_4_(minN)
						cout << "Start: ";
						cin >> mkb;					_PRINTVAR_4_(mkb)
						cout << "anzahl bits für Testpacktet: ";
						cin >> anz;					_PRINTVAR_4_(anz)
						cout << "Ausgabe von den Ergebnissen: ";
						cin >> filename;			_PRINTVAR_4_(filename)

						berechnungsStart = steady_clock::now();

						anz = Benchmarking(filename, minN, mkb, anz);

						berechnungsEnde = steady_clock::now();
						cout << "Gesamtzahl an geprüften Bits: " << anz << '\n';					_PRINTVAR_4_(anz)
						Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
						cout << "Laufzeit: " << Dauer << "s\n\n";
					#else
						cout << "[Error] FLINTxx not enabled while compiling!" << endl;
					#endif // _DISABLELIBFLINTXX_
					break;
				case 2:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)

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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
					break;
				case 3:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)

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
								_PRINTERROR_
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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
					break;
				case 4:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben: ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;		_PRINTVAR_4_(wechselfaktorK)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}
					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 5:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK1024;			_PRINTVAR_4_(maxK1024)
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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 6:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;		_PRINTVAR_4_(wechselfaktorK)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 7:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(maxN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK1KB;				_PRINTVAR_4_(maxK1KB)
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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 8:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << "wechselfaktor für k eingeben (empfohlen 0): ";
					cin >> wechselfaktorK;		_PRINTVAR_4_(wechselfaktorK)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 9:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 10:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "Threadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
					}

					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 11:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK1KB;				_PRINTVAR_4_(maxK1KB)
					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 12:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben (>= max n): ";
					cin >> maxK;				_PRINTVAR_4_(maxK)
					cout << "Anzahl der zu testenden Lampen eingeben (<= min n): ";
					cin >> testLampen;			_PRINTVAR_4_(testLampen)
					cout << "Wechselfaktor für n eingeben (empfohlen 2, mindestens 1): ";
					cin >> wechselfaktorN;		_PRINTVAR_4_(wechselfaktorN)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads4;			_PRINTVAR_4_(AnzThreads4)
					cout << "minimale Threadgröße eingeben (empfohlen 100): ";
					cin >> minG;				_PRINTVAR_4_(minG)
					cout << "maximale Threadgröße eingeben (empfohlen 500): ";
					cin >> maxG;				_PRINTVAR_4_(maxG)
					cout << "maximale Abweichung eingeben: ";
					cin >> MaxAbweichung;		_PRINTVAR_4_(MaxAbweichung)
					cout << GeeigneteThreadgroeszenBerechnen(maxN - minN + 1, AnzThreads4, minG, maxG, MaxAbweichung);
					cout << "\n\nThreadgröße (Anzahl der n je Thread): ";
					cin >> delN;				_PRINTVAR_4_(delN)

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
								_PRINTERROR_
								cout << "Fehler:\t" << e.what() << "\t\tn = " << i + j << endl;
							}
						}
						maxNecht = i + AnzThreads4 * delN - 1;
						PrintProgressBar((i - minN) / diffN, getConsoleWidth());
					}
					PrintProgressBar(1, getConsoleWidth()); cout << endl;
					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;

				case 13:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "max k eingeben: ";
					cin >> maxK16KB;			_PRINTVAR_4_(maxK16KB)
					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)

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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;

				case 14:
#ifdef _ENABLEBIGINTS_
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)

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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
#else
					cout << "[Error] big ints not enabled while compiling!" << endl;
#endif // _ENABLEBIGINTS_
					break;
				case 15:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Basis von max k eingeben: ";
					cin >> mkb;					_PRINTVAR_4_(mkb)
				    cout << "Exponent von max k eingeben (" << mkb << "te Potenz): ";
					cin >> mke;					_PRINTVAR_4_(mke)
//				    cout << "max k eingeben: ";
//					cin >> maxK_GMPLIB;
					maxK_GMPLIB = mkb;
					mpz_ui_pow_ui(maxK_GMPLIB.get_mpz_t(), mkb, mke);

					cout << "max k = " << maxK_GMPLIB << '\n';
					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)

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

					_PRINTVAR_4_(maxNecht)
					berechnungsEnde = steady_clock::now();
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "berechnet bis: " << maxNecht << "\nLaufzeit: " << Dauer << "s\n\n";
					break;
				case 16:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

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
							vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv2(i, anz, false, Session);

							ostringstream oss2;

							// Convert all but the last element to avoid a trailing ","
							copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<mpz_class>(oss2, "\n"));

							// Now add the last element with no delimiter
							oss2 << PositiveRunden.back();

							output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << endl;
						}
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					dt(berechnungsStartHR, durHR);
					cout << "Laufzeit: " << durHR << "\n\n";		_PRINTVAR_2_(durHR)
					break;
				case -16:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(maxN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(maxN)

					berechnungsStart = steady_clock::now();

					for (size_t i = minN; i <= maxN; i++)
					{
						vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv2(i, anz, false, string("tmp-16"));

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
					Dauer = duration<double>{ berechnungsEnde - berechnungsStart }.count();		_PRINTVAR_2_(Dauer)
					cout << "Laufzeit: " << Dauer << "s\n\n";
					break;
				case 17:
					cout << "Zwischenstand laden: ";
					cin >> Session;				_PRINTVAR_4_(Session)
					cout << "Ergebnis speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Fehler: Failed to open " << filename << '\n';
					}
					else {
						auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);

						string Ausgabe;
						vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv3(Session);

						ostringstream oss2;

						// Convert all but the last element to avoid a trailing ","
						copy(PositiveRunden.begin(), PositiveRunden.end() - 1, ostream_iterator<mpz_class>(oss2, "\n"));

						// Now add the last element with no delimiter
						oss2 << PositiveRunden.back();

						uint64_t i = readVariable(Session + "/n", (uint64_t)1729);

						output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << endl;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					dt(berechnungsStartHR, durHR);
					cout << "Laufzeit: " << durHR << "\n\n";		_PRINTVAR_2_(durHR)
					break;
				case 18:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					delN = maxN - minN + 1;

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open()) {
						cout << "Fehler: Failed to open " << filename << '\n';
					}
					else {
						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = minN; i <= maxN; i++) {
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, anz, &output_fstream, Session, berechnungsStartHR]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);
								{
									lock_cout;
									cout << "[Thread] Launched Thread Nr. " << i << "\tTime: " << elapsed << endl;
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv2(i, anz, false, Session + "/" + std::to_string(i));

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
						}

						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					dt(berechnungsStartHR, durHR);
					cout << "Laufzeit: " << durHR << "\n\n";		_PRINTVAR_2_(durHR)
					break;
				case 19:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					delN = maxN - minN + 1;			// = maxN - minN + 1
					diffN = delN - 1;				// = maxN - minN

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open())
					{
						cout << "Fehler: Failed to open " << filename << '\n';

						dt(berechnungsStartHR, durHR);
					}
					else {
						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);

						// Erstelle ein Fenster für die Titelzeile
						WINDOW *titleWin = newwin(1, COLS, 0, 0);
						wrefresh(titleWin);

						// Create an array to store pointers to ncurses windows
						WINDOW *threadWins[delN];
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(4, COLS, i * 4 + 1, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
						}

						const int zeileDrunter = 4 * delN + 2;	// die Zeile unter den Ganzen Fenstern
						const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = minN; i <= maxN; i++) {
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, &minN, &diffN]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);
								{
									lock_cout;
									mvwprintw(threadWins[i - minN], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[i - minN]);
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv4(i, anz, false, Session + "/" + std::to_string(i), threadWins[i - minN]);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[i - minN]);
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i >= minN + AnzThreads) finishedThreads = i - minN - AnzThreads + 1;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(minN, finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(minN, finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType);
						}

						dt(berechnungsStartHR, durHR);

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					break;
				case 20:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					delN = maxN - minN + 1;			// = maxN - minN + 1
					diffN = delN - 1;				// = maxN - minN

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open())
					{
						cout << "Fehler: Failed to open " << filename << '\n';

						dt(berechnungsStartHR, durHR);
					}
					else {
						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);

						// Erstelle ein Fenster für die Titelzeile
						const int titleWinHeight = 2 - tty;		// 1 hoch, wenn tty, sonst 2 hoch
						WINDOW *titleWin = newwin(titleWinHeight, COLS, 0, 0);
						wrefresh(titleWin);

						// Create an array to store pointers to ncurses windows
						WINDOW *threadWins[delN];
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(4, COLS, i * 4 + titleWinHeight, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
						}

						const int zeileDrunter = 4 * delN + titleWinHeight + 1;	// die Zeile unter den Ganzen Fenstern
						const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
						const int timerOrty = !tty;				// wenn es ein tty ist, hat die erste zeile noch genügend platz, sonst nicht
						const int timerOrtx = tty ? delN + (75 + Vsize + 34 - 3) + std::strlen(termType) : 0;

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = minN; i <= maxN; i++) {
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);
								{
									lock_cout;
									mvwprintw(threadWins[i - minN], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[i - minN]);
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv5(i, anz, false, Session + "/" + std::to_string(i), threadWins[i - minN], titleWin, timerOrtx, timerOrty, tty);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[i - minN]);
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i >= minN + AnzThreads) finishedThreads = i - minN - AnzThreads + 1;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(minN, finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(minN, finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						dt(berechnungsStartHR, durHR);

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					break;
				case 21:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();

					delN = maxN - minN + 1;			// = maxN - minN + 1
					diffN = delN - 1;				// = maxN - minN

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open())
					{
						cout << "Fehler: Failed to open " << filename << '\n';

						dt(berechnungsStartHR, durHR);
					}
					else {
						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);

						// Erstelle ein Fenster für die Titelzeile
						constexpr int titleWinHeight = 1;
						WINDOW *titleWin = newwin(titleWinHeight, COLS, 0, 0);
						wrefresh(titleWin);

						// Create an array to store pointers to ncurses windows
						WINDOW *threadWins[delN];
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(4, COLS, i * 4 + titleWinHeight, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
						}

						const int zeileDrunter = 4 * delN + titleWinHeight + 1;	// die Zeile unter den Ganzen Fenstern
						const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
						const int timerOrtx = delN + (50 + Vsize + 34 - 3) + std::strlen(termType);
						constexpr int timerOrty = 0;

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = maxN; i >= minN; i--)					// falsch rum für besseres Zeitmanagement
						{
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);
								{
									lock_cout;
									mvwprintw(threadWins[i - minN], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[i - minN]);
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv5(i, anz, false, Session + "/" + std::to_string(i), threadWins[i - minN], titleWin, timerOrtx, timerOrty, tty);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[i - minN]);
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i <= maxN - AnzThreads + 1) finishedThreads = (maxN - i) - AnzThreads + 2;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						dt(berechnungsStartHR, durHR);

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";
					break;
				case 22:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
				    cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Datei speichern unter: ";
					cin >> filename;			_PRINTVAR_4_(filename)
					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					berechnungsStartHR = std::chrono::high_resolution_clock::now();
					StartTimeGlobal = berechnungsStartHR;

					delN = maxN - minN + 1;			// = maxN - minN + 1
					diffN = delN - 1;				// = maxN - minN

					output_fstream.open(filename, ios_base::out);
					if (!output_fstream.is_open())
					{
						cout << "Fehler: Failed to open " << filename << '\n';

						dt(berechnungsStartHR, durHR);
					}
					else {
						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);

						// Erstelle ein Fenster für die Titelzeile
						constexpr int titleWinHeight = 2;
						WINDOW *titleWin = newwin(titleWinHeight, COLS, 0, 0);
						wrefresh(titleWin);

						// Create an array to store pointers to ncurses windows
						WINDOW *threadWins[delN];
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(4, COLS, i * 4 + titleWinHeight, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
						}

						const int zeileDrunter = 4 * delN + titleWinHeight + 1;	// die Zeile unter den Ganzen Fenstern
						const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
						constexpr int timerOrty = 1;
						constexpr int timerOrtx = 0;

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = maxN; i >= minN; i--)					// falsch rum für besseres Zeitmanagement
						{
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);
								{
									lock_cout;
									mvwprintw(threadWins[i - minN], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[i - minN]);
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv6(i, anz, false, Session + "/" + std::to_string(i), threadWins[i - minN], titleWin, timerOrtx, timerOrty, tty);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[i - minN]);
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i <= maxN - AnzThreads + 1) finishedThreads = (maxN - i) - AnzThreads + 2;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						dt(berechnungsStartHR, durHR);

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					break;
				case 23:
					#ifndef _DISABLELIBFLINTXX_
						cout << "min n eingeben: ";
						cin >> minN;				_PRINTVAR_4_(minN)
						cout << "max n eingeben: ";
						cin >> maxN;				_PRINTVAR_4_(maxN)
						cout << "Anzahl der PR je Lampenanzahl: ";
						cin >> anz;					_PRINTVAR_4_(anz)
						cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
						cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

						cout << "Datei speichern unter: ";
						cin >> filename;			_PRINTVAR_4_(filename)
						cout << "Zwischenstand speichern unter: ";
						cin >> Session;				_PRINTVAR_4_(Session)

						berechnungsStartHR = std::chrono::high_resolution_clock::now();
						StartTimeGlobal = berechnungsStartHR;

						delN = maxN - minN + 1;			// = maxN - minN + 1
						diffN = delN - 1;				// = maxN - minN

						output_fstream.open(filename, ios_base::out);
						if (!output_fstream.is_open())
						{
							cout << "Fehler: Failed to open " << filename << '\n';

							dt(berechnungsStartHR, durHR);
						}
						else {
							// erstelle Ordner für die Session
							erstelleVerzeichnis(Session.c_str());

							initscr();
							start_color();
							cbreak();
							noecho();
							curs_set(0);
							flint_set_num_threads((int)AnzThreadsUnterstützt);

							// Erstelle ein Fenster für die Titelzeile
							constexpr int titleWinHeight = 2;
							WINDOW *titleWin = newwin(titleWinHeight, COLS, 0, 0);
							wrefresh(titleWin);

							// Create an array to store pointers to ncurses windows
							WINDOW *threadWins[delN];
							for (int i = 0; i < delN; i++) {
								threadWins[i] = newwin(4, COLS, i * 4 + titleWinHeight, 0);
								box(threadWins[i], 0, 0);
								if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
								mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
								wrefresh(threadWins[i]);
							}

							const int zeileDrunter = 4 * delN + titleWinHeight + 1;	// die Zeile unter den Ganzen Fenstern
							const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
							constexpr int timerOrty = 1;
							constexpr int timerOrtx = 0;

							// Vector to store futures
							std::vector<std::future<void>> futures;

							for (size_t i = maxN; i >= minN; i--)					// falsch rum für besseres Zeitmanagement
							{
								// Use async to run the function asynchronously
								auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
								{
									string elapsed;
									adt(berechnungsStartHR, elapsed);
									{
										lock_cout;
										mvwprintw(threadWins[i - minN], 1, 72, "Startzeit: %s", elapsed.c_str());
										wrefresh(threadWins[i - minN]);
									}

									// Perform the slow operation in the async thread
									vector<fmpzxx> PositiveRunden = LampenSimulierenFLINT(i, anz, false, Session + "/" + std::to_string(i), threadWins[i - minN], titleWin, timerOrtx, timerOrty, tty);

									std::ostringstream oss2;
									std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<fmpzxx>(oss2, "\n"));
									oss2 << PositiveRunden.back();

									// Synchronize file output with a mutex
									lock_output;
									output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

									// Close the ncurses window when the thread finishes
									//delwin(threadWins[i - minN]);
								});

								// Store the future for later retrieval
								futures.push_back(std::move(fut));

								// Check if the number of active threads exceeds the limit
								while (futures.size() >= AnzThreads)
								{
									auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
									{
										return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
									});

									// Remove completed futures
									if (it != futures.end())
									{
										it->wait();
										futures.erase(it);
									}
								}

								finishedThreads = 0;
								if (i <= maxN - AnzThreads + 1) finishedThreads = (maxN - i) - AnzThreads + 2;
								auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
								printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
							}

							// Wait for the remaining threads to finish
							for (auto& fut : futures)
							{
								fut.wait();
								finishedThreads++;
								auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
								printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
							}

							dt(berechnungsStartHR, durHR);

							if(!tty)
							{
								cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
								cin.get();
								cin.get();
							}
							endwin();	// end ncurses

							if(tty) cout << gotoZeileDrunter;
							cout << "Datei gespeichert als " << filename << '!' << endl;
						}
						cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					#else
						cout << "[Error] FLINTxx not enabled while compiling!" << endl;
					#endif // _DISABLELIBFLINTXX_
					break;
				case 24:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					FileUnopenable = false;
					do {
						if (FileUnopenable) cout << "Fehler beim öffnen der Datei! Gib eine andere, noch nicht verwendete Datei ein.\n";
						cout << "Datei speichern unter: ";
						cin >> filename;		_PRINTVAR_4_(filename)
						output_fstream.open(filename, ios_base::out);
						FileUnopenable = true;
					} while (!output_fstream.is_open());

					{
						berechnungsStartHR = std::chrono::high_resolution_clock::now();
						StartTimeGlobal = berechnungsStartHR;

						delN = maxN - minN + 1;			// = maxN - minN + 1
						diffN = delN - 1;				// = maxN - minN

						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);
						std::thread input_thread(input_listener); // Starte Eingabe-Thread

						// Erstelle ein Fenster für die Titelzeile
						constexpr int titleWinHeight = 2;
						Titelfensterhöhe = titleWinHeight;
						WINDOW *titleWin = newwin(titleWinHeight, COLS, 0, 0);
						wrefresh(titleWin);
						FensterAktiviert = true;

						// Create an array to store pointers to ncurses windows
						vector<WINDOW *> threadWins;
						threadWins.resize(delN);
						FensterExistiert.resize(delN,false);
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(Fensterhöhe, COLS, i * Fensterhöhe + titleWinHeight, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
							FensterExistiert[i] = true;
						}

						// mache daraus globale variablen
						TitelFenster = titleWin;
						ThreadFenster = threadWins;

						const int zeileDrunter = Fensterhöhe * delN + titleWinHeight + 1;	// die Zeile unter den Ganzen Fenstern
						const auto gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
						constexpr int timerOrty = 1;
						constexpr int timerOrtx = 0;

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = maxN; i >= minN; i--)					// falsch rum für besseres Zeitmanagement
						{
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);

								const auto index = i - minN;
								{
									lock_cout;
									mvwprintw(threadWins[index], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[index]);
									//FensterExistiert[index] = true;
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv7(i, anz, false, Session + "/" + std::to_string(i), threadWins[index], titleWin, timerOrtx, timerOrty, tty);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[index]);
								//FensterExistiert[index] = false;
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i <= maxN - AnzThreads + 1) finishedThreads = (maxN - i) - AnzThreads + 2;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						dt(berechnungsStartHR, durHR);

						EingabeAktiviert = false;
						input_thread.join();

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						FensterAktiviert = false;
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					break;
				case 25:
					cout << "min n eingeben: ";
					cin >> minN;				_PRINTVAR_4_(minN)
					cout << "max n eingeben: ";
					cin >> maxN;				_PRINTVAR_4_(maxN)
					cout << "Anzahl der PR je Lampenanzahl: ";
					cin >> anz;					_PRINTVAR_4_(anz)
					cout << AnzThreadsUnterstützt << " Threads werden unterstützt. Anzahl gewünschter Threads eingeben: ";
					cin >> AnzThreads;			_PRINTVAR_4_(AnzThreads)

					cout << "Zwischenstand speichern unter: ";
					cin >> Session;				_PRINTVAR_4_(Session)

					FileUnopenable = false;
					do {
						if (FileUnopenable) cout << "Fehler beim öffnen der Datei! Gib eine andere, noch nicht verwendete Datei ein.\n";
						cout << "Datei speichern unter: ";
						cin >> filename;		_PRINTVAR_4_(filename)
						output_fstream.open(filename, ios_base::out);
						FileUnopenable = true;
					} while (!output_fstream.is_open());

					{
						berechnungsStartHR = std::chrono::high_resolution_clock::now();
						StartTimeGlobal = berechnungsStartHR;

						delN = maxN - minN + 1;			// = maxN - minN + 1
						diffN = delN - 1;				// = maxN - minN

						// erstelle Ordner für die Session
						erstelleVerzeichnis(Session.c_str());

						cout_mutex.lock();

						initscr();
						start_color();
						cbreak();
						noecho();
						curs_set(0);
						init_pair(1, COLOR_RED, COLOR_BLACK);
						std::thread input_thread(input_listener); // Starte Eingabe-Thread

						ThreadFensterShift = 0;

						// Erstelle ein Fenster für die Titelzeile
						constexpr int titleWinHeight = 2;
						Titelfensterhöhe = titleWinHeight;
						WINDOW *titleWin = newwin(titleWinHeight, COLS, ThreadFensterShift, 0);
						wrefresh(titleWin);
						FensterAktiviert = true;

						ThreadFensterShift += titleWinHeight;

						// Erstelle ein Fenster für die Titelzeile
						constexpr int NotifsWinHeight = 4;
						Nachrichtenfensterhöhe = NotifsWinHeight;
						WINDOW *notifWin = newwin(NotifsWinHeight, COLS, ThreadFensterShift, 0);
						wattron(notifWin, A_BOLD | COLOR_PAIR(1)); // Set bold and red color
						box(notifWin, 0, 0);
						if(!tty) wborder_set(notifWin, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
						mvwprintw(notifWin, 0, 2, " Systemnachrichten ");
						wattroff(notifWin, A_BOLD | COLOR_PAIR(1)); // Turn off bold and red color
						wrefresh(notifWin);
						NachrichtenAktiviert = true;

						ThreadFensterShift += NotifsWinHeight;

						// Create an array to store pointers to ncurses windows
						vector<WINDOW *> threadWins;
						threadWins.resize(delN);
						FensterExistiert.resize(delN,false);
						for (int i = 0; i < delN; i++) {
							threadWins[i] = newwin(Fensterhöhe, COLS, i * Fensterhöhe + ThreadFensterShift, 0);
							box(threadWins[i], 0, 0);
							if(!tty) wborder_set(threadWins[i], &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
							mvwprintw(threadWins[i], 0, 2, " n = %lld ", minN + i);
							wrefresh(threadWins[i]);
							FensterExistiert[i] = true;
						}

						// mache daraus globale variablen
						TitelFenster = titleWin;
						NachrichtenFenster = notifWin;
						ThreadFenster = threadWins;

						cout_mutex.unlock();

						const int zeileDrunter = Fensterhöhe * delN + ThreadFensterShift + 1;	// die Zeile unter den Ganzen Fenstern
						gotoZeileDrunter = "\033[" + std::to_string(zeileDrunter) + ";1H";
						constexpr int timerOrty = 1;
						constexpr int timerOrtx = 0;

						auto el = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
						printProgressBar(0, delN, delN, el, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);

						// Vector to store futures
						std::vector<std::future<void>> futures;

						for (size_t i = maxN; i >= minN; i--)					// falsch rum für besseres Zeitmanagement
						{
							// Use async to run the function asynchronously
							auto fut = std::async(std::launch::async, [i, &anz, &output_fstream, Session, berechnungsStartHR, &threadWins, titleWin, &minN, &diffN, &timerOrtx, &timerOrty, &tty]()
							{
								string elapsed;
								adt(berechnungsStartHR, elapsed);

								const auto index = i - minN;
								{
									lock_cout;
									mvwprintw(threadWins[index], 1, 72, "Startzeit: %s", elapsed.c_str());
									wrefresh(threadWins[index]);
									//FensterExistiert[index] = true;
								}

								// Perform the slow operation in the async thread
								vector<mpz_class> PositiveRunden = LampenSimulierenGMPLIBv8(i, anz, false, Session + "/" + std::to_string(i), threadWins[index], titleWin, timerOrtx, timerOrty, tty);

								std::ostringstream oss2;
								std::copy(PositiveRunden.begin(), PositiveRunden.end() - 1, std::ostream_iterator<mpz_class>(oss2, "\n"));
								oss2 << PositiveRunden.back();

								// Synchronize file output with a mutex
								lock_output;
								output_fstream << "Lampenanzahl: " << i << "; positive Runde(n) :\n" << oss2.str() << "\n" << std::endl;

								// Close the ncurses window when the thread finishes
								//delwin(threadWins[index]);
								//FensterExistiert[index] = false;
							});

							// Store the future for later retrieval
							futures.push_back(std::move(fut));

							// Check if the number of active threads exceeds the limit
							while (futures.size() >= AnzThreads)
							{
								auto it = std::find_if(futures.begin(), futures.end(), [](const std::future<void>& f)
								{
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								});

								// Remove completed futures
								if (it != futures.end())
								{
									it->wait();
									futures.erase(it);
								}
							}

							finishedThreads = 0;
							if (i <= maxN - AnzThreads + 1) finishedThreads = (maxN - i) - AnzThreads + 2;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						// Wait for the remaining threads to finish
						for (auto& fut : futures)
						{
							fut.wait();
							finishedThreads++;
							auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - berechnungsStartHR);
							printProgressBar(finishedThreads, delN, delN, elapsed, 'k', titleWin, cout_mutex, termType, timerOrtx, timerOrty);
						}

						dt(berechnungsStartHR, durHR);

						EingabeAktiviert = false;
						input_thread.join();

						if(!tty)
						{
							cout << gotoZeileDrunter << "\nBerechnungen beendet! Taste drücken, um ins Menü zurückzukommen." << endl;
							cin.get();
							cin.get();
						}
						FensterAktiviert = false;
						NachrichtenAktiviert = false;
						endwin();	// end ncurses

						if(tty) cout << gotoZeileDrunter;
						gotoZeileDrunter = "\n";
						cout << "Datei gespeichert als " << filename << '!' << endl;
					}
					cout << "Laufzeit: " << durHR << "                                                      \n\n";	_PRINTVAR_2_(durHR)
					break;
				default:
					cout << "\aFehlerhafte Eingabe!\n\n";
					break;
				}
			}
			catch (const std::exception& e)
			{
				_PRINTERROR_
				cerr << "\aFehler:\n" << e.what() << endl;
				endwin();	// end ncurses
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
			const std::string menu = std::string("Programm von Lorenz Taschner & Lars Krabbenhöft\n")
				+ "Lampen prüfen bis (n,k)\n"
				+ "Welche Prüfmethode?\n"
				+ "0  = Beenden\n"
				+ "1 = Für eine große Lampenanzahl\n"
				+ "2 = Für eine große Rundenanzahl\n"
				+ "3 = wenn du deinen PC sprengen willst (ca. 2^(2^17) Runden)"
				+ '\n';
			cout << menu;
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
							_PRINTERROR_
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
			_PRINTERROR_
			cout << "Fehler:\n" << e.what() << endl;
		}
	}

	}
}
