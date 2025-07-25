#pragma once

// This Files only Purpose id to contain the global Variables.

#ifdef _WIN32 // Windows
	#define lock_output
	#define lock_cout
	#define lock_log
	#define lock_einst
#else
	std::mutex output_mutex;					// Mutex für den Dateizugriff
	std::mutex cout_mutex;						// Mutex für den Zugriff auf std::cout
	std::mutex log_mutex;						// Mutex für den Zugriff auf die Logdatei
	std::mutex einst_mutex;						// Mutex für den Zugriff auf die Einstellungen
	#define lock_output	std::lock_guard<std::mutex> lock(output_mutex)
	#define lock_cout	std::lock_guard<std::mutex> lock(cout_mutex)
	#define lock_log	std::lock_guard<std::mutex> lock(log_mutex)
	#define lock_einst	std::lock_guard<std::mutex> lock(einst_mutex)
#endif
constexpr auto			Vsize = strlen(_V);		// String-Länge der Programmversion als eine globale Konstante
//std::chrono::_V2::system_clock::time_point StartTimeGlobal;
auto		 			StartTimeGlobal = std::chrono::high_resolution_clock::now();
std::atomic<uint64_t>	SleepTimer_ms = 200;
std::atomic<int>		UserInterrupt = 0;		// Wenn es einen Interrupt gab, dann setze Wert auf den Interrupt-Wert, sonst 0
std::atomic<uint64_t>	AnzInterrupts = 0;		// Anzahl Interrupts
constexpr auto			MaxInterrupts = 256;	// Maximale Anzahl an Interrupts, bevor das Programm sich selbst beendet.
std::atomic<bool>		InterruptRequiredByApp = false;
auto					Starttime = std::chrono::steady_clock::now();
mpz_t					global_puffer_mpz_t1,	global_puffer_mpz_t2,	global_puffer_mpz_t3;
mpz_class				global_puffer_mpz_c1,	global_puffer_mpz_c2,	global_puffer_mpz_c3;

#ifndef _DISABLELIBFLINTXX_
	fmpz_t				global_puffer_fmpz_t1,	global_puffer_fmpz_t2,	global_puffer_fmpz_t3, global_puffer_fmpz_t4, global_puffer_fmpz_t5;
	flint::fmpzxx		global_puffer_mpzxx1;
#endif

// Fenster die in ncurses geöffnet sind
bool					FensterAktiviert = false;
WINDOW 					*TitelFenster;
WINDOW 					*NachrichtenFenster;
std::vector<WINDOW *> 	ThreadFenster;
std::vector<bool>		FensterExistiert;
constexpr int			Fensterhöhe = 4;
int						Titelfensterhöhe;
int						Nachrichtenfensterhöhe;
int						ThreadFensterShift;
std::atomic<bool>		redrawInProgress = false;
std::atomic<bool>		redrawAgain = false;
std::atomic<bool>		NachrichtenAktiviert = false;
std::atomic<bool>		EingabeAktiviert = false;
std::atomic<bool>		Pausiert = false;

std::string				gotoZeileDrunter = "\n";

bool					ttyGlobal;

// debug value, normally false
bool					DefaultIncreasedBackupFrequency = true;

// Globale Einstellungen
std::string globSession = "";
std::map<std::string, std::string> Einstellungen;
constexpr auto			EinstellungsDateiname = "Einstellungen.ini";
constexpr auto			__EINST_LANG__ = "Lang";
constexpr auto			__EINST_OFFS__ = "Zeilenoffset";
constexpr auto			BenchmarkErgebnisOrdner = "Benchergs";

// Rückgabewert, bei dem weitergemacht wird
constexpr int			contRetVal = 0xBADEAFFE;

// Unicode-Zeichen für abgerundete Ecken
cchar_t ls, rs, ts, bs, tl, tr, bl, br;

//new makro for size of mpz in bytes. since mpz_sizeinbase only supports up to 62 we use the fact that 16^2=256
#define lp_mpz_bytesize(x) (mpz_sizeinbase(x.get_mpz_t(), 16) / 2)
