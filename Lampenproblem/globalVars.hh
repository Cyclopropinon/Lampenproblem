#pragma once

// This Files only Purpose id to contain the global Variables.

std::mutex output_mutex;			// Mutex für den Dateizugriff
std::mutex cout_mutex;				// Mutex für den Zugriff auf std::cout
constexpr auto Vsize = strlen(_V);	// String-Länge der Programmversion als eine globale Konstante
std::chrono::_V2::system_clock::time_point StartTimeGlobal;
