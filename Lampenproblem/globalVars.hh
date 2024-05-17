#pragma once

// This Files only Purpose id to contain the global Variables.

#ifdef _WIN32 // Windows
	#define lock_output
	#define lock_cout
#else
	std::mutex output_mutex;				// Mutex für den Dateizugriff
	std::mutex cout_mutex;					// Mutex für den Zugriff auf std::cout
	std::mutex log_mutex;					// Mutex für den Zugriff auf die Logdatei
	#define lock_output	std::lock_guard<std::mutex> lock(output_mutex)
	#define lock_cout	std::lock_guard<std::mutex> lock(cout_mutex)
	#define lock_log	std::lock_guard<std::mutex> lock(log_mutex)
#endif
constexpr auto Vsize = strlen(_V);			// String-Länge der Programmversion als eine globale Konstante
std::chrono::_V2::system_clock::time_point StartTimeGlobal;
std::atomic<int>      UserInterrupt = 0;	// Wenn es einen Interrupt gab, dann setze Wert auf den Interrupt-Wert, sonst 0
std::atomic<uint64_t> AnzInterrupts = 0;	// Anzahl Interrupts
std::atomic<bool>     InterruptRequiredByApp = false;
auto Starttime = std::chrono::steady_clock::now();
mpz_t global_puffer_mpz_t1, global_puffer_mpz_t2, global_puffer_mpz_t3;
flint::fmpzxx global_puffer_mpzxx1;
