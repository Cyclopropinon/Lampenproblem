#pragma once

#include <vector>
#include <flint/flint.h>
#include <flint/fmpzxx.h>
#include <functional>

// Definiere eine Typalias für eine Funktion mit zwei integer Argumenten und integer Rückgabewert
using LampenSchrittVariante = std::function<int(unsigned long long, flint::fmpzxx, flint::fmpzxx, unsigned long long)>;

// Definiere die Funktion exec
int exec(int f, int n, int k, const std::vector<LampenSchrittVariante>& funcs) {
    // Überprüfe, ob der Index gültig ist
    if (f >= 0 && f < funcs.size()) {
        // Rufe die Funktion an der Position f im Vector auf
        return funcs[f](n, k);
    } else {
        // Gib einen Fehler aus, wenn der Index ungültig ist
        std::cerr << "Ungültiger Index für Funktionenvektor!" << std::endl;
        // Gib einen Standardwert zurück
        return 0;
    }
}

// Beispiel für eine Funktion, die du aufrufen könntest
int add(int a, int b) {
    return a + b;
}

inline void weiterlaufen_muster(unsigned long long& n, flint::fmpzxx& n_flintlib, flint::fmpzxx& AnzRunden, unsigned long long& Lampejetzt)
{
    // Musterfunktion; wird nicht benutzt
}

inline void weiterlaufen0(unsigned long long& n, flint::fmpzxx& n_flintlib, flint::fmpzxx& AnzRunden, unsigned long long& Lampejetzt)
{
    //
}

inline void weiterlaufen1(unsigned long long& n, flint::fmpzxx& n_flintlib, flint::fmpzxx& AnzRunden, unsigned long long& Lampejetzt)
{
    //
}

inline void weiterlaufen2(unsigned long long& n, flint::fmpzxx& n_flintlib, flint::fmpzxx& AnzRunden, unsigned long long& Lampejetzt)
{
    //
}

inline void weiterlaufen3(unsigned long long& n, flint::fmpzxx& n_flintlib, flint::fmpzxx& AnzRunden, unsigned long long& Lampejetzt)
{
    //
}
