#pragma once

#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <gmp.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <mutex>
#include <unistd.h>

#ifdef DNCURSES_WIDECHAR
	#include <ncursesw/ncurses.h>
#else
	#include <ncurses.h>
#endif

#define __defProgBar__

#define dt(Start, out)	{\
                			char buffer[50];\
			    			berechnungsEndeHR = std::chrono::high_resolution_clock::now();\
			    			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(berechnungsEndeHR - Start);\
			    			uint64_t total_seconds = duration.count() / 1'000'000'000;\
			    			uint64_t remaining_ns = duration.count() % 1'000'000'000;\
			    			sprintf(buffer, "%llu,%09llus", total_seconds, remaining_ns);\
			    			out = buffer;\
			    		}

// wie dt(), aber ohne berechnungsEndeHR
#define adt(Start, out)	{\
                			char buffer[50];\
			    			auto adt_BEHR__ = std::chrono::high_resolution_clock::now();\
			    			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(adt_BEHR__ - Start);\
			    			uint64_t total_seconds = duration.count() / 1'000'000'000;\
			    			uint64_t remaining_ns = duration.count() % 1'000'000'000;\
			    			sprintf(buffer, "%llu,%09llus", total_seconds, remaining_ns);\
			    			out = buffer;\
			    		}

// wie dt(), aber zeit ist durch 2^20 = 1'048'576 geteilt.
#define ddt(Start, out)	{\
                			char buffer[50];\
			    			berechnungsEndeHR = std::chrono::high_resolution_clock::now();\
			    			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(berechnungsEndeHR - Start);\
			    			uint64_t total_seconds = duration.count() / 1'048'576'000'000'000;\
			    			uint64_t remaining_ns = (duration.count() % 1'048'576'000'000'000) / 1'048'576;\
			    			sprintf(buffer, "%llu,%09llus", total_seconds, remaining_ns);\
			    			out = buffer;\
			    		}
                        
// wie dt(), aber zeit ist durch den rest von print geteilt.
#define pdt(Start, out)	{\
                			char buffer[50];\
                            auto p = print % 1'048'576;\
			    			berechnungsEndeHR = std::chrono::high_resolution_clock::now();\
			    			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(berechnungsEndeHR - Start);\
			    			uint64_t total_seconds = duration.count() / (1'000'000'000 * p);\
			    			uint64_t remaining_ns = (duration.count() % 1'000'000'000) / p;\
			    			sprintf(buffer, "%llu,%09llus", total_seconds, remaining_ns);\
			    			out = buffer;\
			    		}

std::string giveRAM(char unit)
{
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open())
    {
        throw std::runtime_error("Failed to open /proc/self/status");
    }

    uint64_t rss = 0, swap = 0;
    std::string line;
    while (std::getline(statusFile, line))
    {
        std::istringstream is(line);
        std::string key;
        is >> key;
        if (key == "VmRSS:")
        {
            is >> rss;
        }
        else if (key == "VmSwap:")
        {
            is >> swap;
        }
    }

    uint64_t totalKB = rss + swap;  // Sum of RAM and swap in KB

    std::ostringstream os;
    switch (unit)
    {
    case 'b':
        os << totalKB * 1024 * 8 << " bits";
        break;
    case 'B':
        os << totalKB * 1024 << " Bytes";
        break;
    case 'K':
    case 'k':
        os << totalKB << " KB";
        break;
    case 'M':
        os << totalKB / 1024.0 << " MB";
        break;
    case 'G':
        os << totalKB / 1024.0 / 1024.0 << " GB";
        break;
    default:
        throw std::invalid_argument("Unsupported unit");
    }
    return os.str();
}

int getConsoleWidth() 
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

// @param current das jetztige n (aka i)
void printProgressBar(uint64_t shift, uint64_t current, uint64_t total, int barWidth, const std::chrono::nanoseconds& elapsed, char ramUnit) 
{
    double progress = static_cast<double>(current - shift) / total;
    int pos = static_cast<int>(barWidth * progress) + ((current - shift) > 0);

    constexpr double B   = 3.68065;
    constexpr double lnB = 1.30309; // = ln(B)
    //double factor = std::pow(static_cast<double>(total) / (current - shift), 2.5);  //TODO adjust
    double baseshift = std::log(std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count()) / lnB - current;
    std::chrono::duration<double> dd(std::pow(B, (baseshift + total + shift)));
    std::chrono::nanoseconds estTotalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(dd);
    auto remaining = estTotalTime - elapsed;

    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    auto remainingSeconds = std::chrono::duration_cast<std::chrono::seconds>(remaining).count();
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(estTotalTime).count();

    std::ostringstream out;
    out << "\r[";

    std::string currentStr = "\033[96m" + std::to_string(current) + "\033[0m"; // cyan color
    bool currentDisplayed = false;

    if (pos >= static_cast<int>(currentStr.length()) - 9) // 9 accounts for added escape characters
    {
        out << currentStr;
        currentDisplayed = true;
    }

    for (int i = 0; i < barWidth; ++i) 
    {
        if (currentDisplayed && i < static_cast<int>(currentStr.length()) - 9) 
            continue;

        if (i < pos) 
            out << "#";
        else if (i == pos) 
            out << ">";
        else 
            out << " ";
    }

    out << "] \033[92m" << std::fixed << std::setprecision(2) << progress * 100.0 << "%\033[0m "    // green
        << "\033[93m" << remainingSeconds << "s [" << elapsedSeconds << "s/" << totalSeconds << "s]\033[0m "    // yellow
        << "\033[91mRAM: " << giveRAM(ramUnit) << "\033[0m";    //red

    int escapeCodeLength = 4 * 9; // 9 characters for each of the 4 color escape codes
    int currentLength = currentDisplayed ? static_cast<int>(std::to_string(current).length()) : 0;
    int consoleWidth = getConsoleWidth();
    int remainingSpaces = consoleWidth - out.str().length() + escapeCodeLength;
    if (!currentDisplayed)
        remainingSpaces += currentLength;
    if (remainingSpaces > 0)
        out << std::string(remainingSpaces, ' ');

    std::cout << out.str() << std::flush;
}

// ncurses Variante
// @param current Anzahl der fertigen Threads
void printProgressBar(uint64_t min, uint64_t current, uint64_t total, int barWidth, const std::chrono::nanoseconds& elapsed_ns, char ramUnit, WINDOW *outputWin, std::mutex& pr_mutex, const char* TERM)
{
    double progress = static_cast<double>(current) / total;
    int pos = static_cast<int>(barWidth * progress) + (current > 0);

    constexpr double B   = 3.68065;
    constexpr double lnB = 1.30309; // = ln(B)
    double baseshift = std::log(std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_ns).count()) / lnB - current;
    std::chrono::duration<double> dd(std::pow(B, (baseshift + total + min)));
    std::chrono::seconds estTotalTime = std::chrono::duration_cast<std::chrono::seconds>(dd);
    std::chrono::seconds elapsed = std::chrono::duration_cast<std::chrono::seconds>(elapsed_ns);
    auto remaining = estTotalTime - elapsed;

    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    auto remainingSeconds = std::chrono::duration_cast<std::chrono::seconds>(remaining).count();
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(estTotalTime).count();

    std::string out = "[";

    for (int i = 0; i < barWidth; ++i) 
    {
        if (i < pos) 
            out += '#';
        else if (i == pos) 
            out += '>';
        else 
            out += ' ';
    }
    out += ']';

    std::string currentStr = std::to_string(current);   // cyan color

    std::string prozent;
    if (current == total)
    {
        prozent = " 100% ";
    } else {
        std::stringstream outper;
        auto percent = progress * 100.0;
        if (percent < 10) outper << '0';                // Format: xx.xx%
        outper << std::fixed << std::setprecision(2) << percent << '%';     // grün
        prozent = outper.str();
    }

    std::string Zeitanalyse = std::to_string(remainingSeconds) + "s [" + std::to_string(elapsedSeconds) + "s/" + std::to_string(totalSeconds) + "s]";   //Gelb

    std::lock_guard<std::mutex> lock(pr_mutex);         // Verhindert race conditions

    start_color();  // Aktiviert die Farbunterstützung
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);

    wattron(outputWin, A_BOLD);                         // Fette Schrift

    mvwprintw(outputWin, 0, 0, "%s", out.c_str());      // Fortschrittsbalken

    if (pos >= static_cast<int>(currentStr.length()))
    {
        wattron(outputWin, COLOR_PAIR(2));              // Cyan auf Schwarz
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat"
        mvwprintw(outputWin, 0, 1, "%llu", current);    // embed Zahl
        #pragma GCC diagnostic pop
        wattroff(outputWin, COLOR_PAIR(2));             // Farbe deaktivieren
    }
    
    wattron(outputWin, COLOR_PAIR(5));                  // Grün auf Schwarz
    mvwprintw(outputWin, 0, barWidth + 3, "%s", prozent.c_str());                   // Prozentzahl
    wattroff(outputWin, COLOR_PAIR(5));                 // Farbe deaktivieren

    wattron(outputWin, COLOR_PAIR(4));                  // Gelb auf Schwarz
    mvwprintw(outputWin, 0, barWidth + 10, "                                    "); // vorherige Shrift leeren
    mvwprintw(outputWin, 0, barWidth + 10, "%s", Zeitanalyse.c_str());              // Zeit
    wattroff(outputWin, COLOR_PAIR(4));                 // Farbe deaktivieren

    wattron(outputWin, COLOR_PAIR(1));                  // Rot auf Schwarz
    mvwprintw(outputWin, 0, barWidth + 50, "RAM: %s", giveRAM(ramUnit).c_str());    // RAM verbrauch
    wattroff(outputWin, COLOR_PAIR(1));                 // Farbe deaktivieren

    wattron(outputWin, COLOR_PAIR(3));                  // Magenta auf Schwarz
    mvwprintw(outputWin, 0, barWidth + 70, "Terminal: %s", TERM);                   // Terminal type
    wattroff(outputWin, COLOR_PAIR(3));                 // Farbe deaktivieren

    wattroff(outputWin, A_BOLD);                        // Fett deaktivieren

    wrefresh(outputWin);
}

void pnarc(std::string msg)  // printNumberAndResetCursor
{
    // Speichern der aktuellen Position des Cursors
    std::streampos currentPosition = std::cout.tellp();

    // Ausgabe der Zahl
    std::cout << msg << std::flush;

    // Zurücksetzen des Cursors auf die gespeicherte Position
    std::cout.seekp(currentPosition);
}
