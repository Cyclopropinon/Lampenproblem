#pragma once

#include <flint/flint.h>
#include <flint/fmpzxx.h>
#include <functional>
#include <vector>

inline void weiterlaufen_muster(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	// Musterfunktion; wird nicht benutzt
}

inline void weiterlaufen0(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	*AnzRunden = 1 + *Schritte / *n_flintlib;
	*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
}

inline void weiterlaufen1(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	flint::fmpzxx Lj; // = neu Lampejetzt
	fmpz_tdiv_qr((*AnzRunden)._fmpz(), Lj._fmpz(), (*Schritte)._fmpz(), (*n_flintlib)._fmpz());
	*AnzRunden += 1;
	*Lampejetzt = fmpz_get_ui(Lj._fmpz());
}

inline void weiterlaufen2(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	*Lampejetzt = mpz_tdiv_q_ui((mpz_ptr)(*AnzRunden)._fmpz(), (mpz_srcptr)(*Schritte)._fmpz(), *n);
	*AnzRunden += 1;
}

inline void weiterlaufen3(unsigned long long *n, flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	fmpz_tdiv_q_ui((*AnzRunden)._fmpz(), (*Schritte)._fmpz(), *n);
	*AnzRunden += 1;
	*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
}

// Definiere eine Typalias für eine Funktion mit zwei integer Argumenten und integer Rückgabewert
using LampenSchrittVariante = std::function<void(unsigned long long*, flint::fmpzxx*, flint::fmpzxx*, flint::fmpzxx*, unsigned long long*)>;

const std::vector<LampenSchrittVariante> LSvarianten = {
	weiterlaufen0,
	weiterlaufen1,
	weiterlaufen2,
	weiterlaufen3
};

const auto anzLSvarianten = LSvarianten.size();
