#pragma once

// This Files only Purpose id to contain the global Variables.

#ifdef _WIN32 // Windows
	#define lock_output
	#define lock_cout
#else
	std::mutex output_mutex;				// Mutex für den Dateizugriff
	std::mutex cout_mutex;					// Mutex für den Zugriff auf std::cout
	#define lock_output std::lock_guard<std::mutex> lock(output_mutex)
	#define lock_cout   std::lock_guard<std::mutex> lock(cout_mutex)
#endif
constexpr auto Vsize = strlen(_V);			// String-Länge der Programmversion als eine globale Konstante
std::chrono::_V2::system_clock::time_point StartTimeGlobal;
std::atomic<int>      UserInterrupt = 0;	// Wenn es einen Interrupt gab, dann setze Wert auf den Interrupt-Wert, sonst 0
std::atomic<uint64_t> AnzInterrupts = 0;	// Anzahl Interrupts
