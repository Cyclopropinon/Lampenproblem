#pragma once

#include <flint/flint.h>
#include <flint/fmpzxx.h>
#include <functional>
#include <vector>

inline void weiterlaufen_muster(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, unsigned long long *Lampejetzt)
{
    // Musterfunktion; wird nicht benutzt
}

inline void weiterlaufen0(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, unsigned long long *Lampejetzt)
{
    //
}

inline void weiterlaufen1(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, unsigned long long *Lampejetzt)
{
    //
}

inline void weiterlaufen2(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, unsigned long long *Lampejetzt)
{
    //
}

inline void weiterlaufen3(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, unsigned long long *Lampejetzt)
{
    //
}

// Definiere eine Typalias für eine Funktion mit zwei integer Argumenten und integer Rückgabewert
using LampenSchrittVariante = std::function<void(unsigned long long*, flint::fmpzxx*, flint::fmpzxx*, unsigned long long*)>;

std::vector<LampenSchrittVariante> LSvarianten = {
    weiterlaufen0,
    weiterlaufen1,
    weiterlaufen2,
    weiterlaufen3
};
