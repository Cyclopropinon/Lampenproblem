#pragma once

#include <flint/flint.h>
#include <flint/fmpzxx.h>
#include <flint/thread_support.h>
#include <functional>
#include <vector>

inline bool weiterlaufen_muster(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	// Musterfunktion; wird nicht benutzt
	// *Schritte += *AnzRunden;
	// *AnzRunden = 1 + *Schritte / *n_flintlib;
	// *Lampejetzt = *Schritte % *n;
	// return Schritte <= maxSchritte;
}

inline bool weiterlaufen0(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	*AnzRunden = 1 + *Schritte / *n_flintlib;
	*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
	return *Schritte <= *maxSchritte;
}

inline bool weiterlaufen1(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	fmpz_tdiv_qr((*AnzRunden)._fmpz(), global_puffer_mpzxx1._fmpz(), (*Schritte)._fmpz(), (*n_flintlib)._fmpz());
	*AnzRunden += 1;
	*Lampejetzt = fmpz_get_ui(global_puffer_mpzxx1._fmpz());
	return *Schritte <= *maxSchritte;
}

inline bool weiterlaufen2(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	fmpz_get_mpz(global_puffer_mpz_t1, (*AnzRunden)._fmpz());
	fmpz_get_mpz(global_puffer_mpz_t2, (*Schritte)._fmpz());
	*Lampejetzt = mpz_tdiv_q_ui(global_puffer_mpz_t1, global_puffer_mpz_t2, *n);
	fmpz_set_mpz((*AnzRunden)._fmpz(), global_puffer_mpz_t1);
	fmpz_set_mpz((*Schritte)._fmpz(), global_puffer_mpz_t2);
	*AnzRunden += 1;
	return *Schritte <= *maxSchritte;
}

inline bool weiterlaufen3(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	fmpz_tdiv_q_ui((*AnzRunden)._fmpz(), (*Schritte)._fmpz(), *n);
	*AnzRunden += 1;
	*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
	return *Schritte <= *maxSchritte;
}

inline bool weiterlaufen4(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;                //_PRINTWAYPOINT_
	mpz_t a, s;                             //_PRINTWAYPOINT_
	mpz_init(a);                            //_PRINTWAYPOINT_
	mpz_init(s);                            //_PRINTWAYPOINT_
	fmpz_get_mpz(a, (*AnzRunden)._fmpz());  //_PRINTWAYPOINT_
	fmpz_get_mpz(s, (*Schritte)._fmpz());   //_PRINTWAYPOINT_
	*Lampejetzt = mpz_tdiv_q_ui(a, s, *n);  //_PRINTWAYPOINT_
	fmpz_set_mpz((*AnzRunden)._fmpz(), a);  //_PRINTWAYPOINT_
	fmpz_set_mpz((*Schritte)._fmpz(), s);   //_PRINTWAYPOINT_
	mpz_clear(a);                           //_PRINTWAYPOINT_
	mpz_clear(s);                           //_PRINTWAYPOINT_
	*AnzRunden += 1;                        //_PRINTWAYPOINT_
	return *Schritte <= *maxSchritte;
}

/*inline void weiterlaufen5AnzRunden(unsigned long long *n, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	fmpz_tdiv_q_ui((*AnzRunden)._fmpz(), (*Schritte)._fmpz(), *n);
	*AnzRunden += 1;
}

inline void weiterlaufen5Lampejetzt(unsigned long long *n, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
}

inline bool weiterlaufen5(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	std::thread wl5a(weiterlaufen5AnzRunden, n, AnzRunden, Schritte, Lampejetzt);
	std::thread wl5l(weiterlaufen5Lampejetzt, n, AnzRunden, Schritte, Lampejetzt);
    // Warten auf Threads
    wl5a.join();
    wl5l.join();
	return *Schritte <= *maxSchritte;
}

inline void weiterlaufen6thread(bool var, unsigned long long *n, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt)
{
	if (var)
	{
		fmpz_tdiv_q_ui((*AnzRunden)._fmpz(), (*Schritte)._fmpz(), *n);
		*AnzRunden += 1;
	} else {
		*Lampejetzt = fmpz_tdiv_ui((*Schritte)._fmpz(), *n);
	}
}

inline bool weiterlaufen6(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	*Schritte += *AnzRunden;
	flint_parallel_do(weiterlaufen6thread, n, AnzRunden, Schritte, Lampejetzt, 3, 2, FLINT_PARALLEL_UNIFORM);
	// std::thread wl5a(weiterlaufen5AnzRunden, n, AnzRunden, Schritte, Lampejetzt);
	// std::thread wl5l(weiterlaufen5Lampejetzt, n, AnzRunden, Schritte, Lampejetzt);
    // Warten auf Threads
    // wl5a.join();
    // wl5l.join();
	return *Schritte <= *maxSchritte;
}*/

inline bool weiterlaufen7(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)
{
	mpz_add(global_puffer_mpz_t2, global_puffer_mpz_t1, global_puffer_mpz_t2);
	*Lampejetzt = mpz_tdiv_q_ui(global_puffer_mpz_t1, global_puffer_mpz_t2, *n);
	mpz_add_ui(global_puffer_mpz_t1, global_puffer_mpz_t1, 1);
	return mpz_cmp(global_puffer_mpz_t2, global_puffer_mpz_t3) <= 0;
}

// Definiere eine Typalias für die Funktion
//using LampenSchrittVariante = std::function<void(unsigned long long*, flint::fmpzxx*, flint::fmpzxx*, flint::fmpzxx*, unsigned long long*)>;
#define LampenSchrittVariante std::function<bool(unsigned long long *n, const flint::fmpzxx *n_flintlib, flint::fmpzxx *AnzRunden, flint::fmpzxx *Schritte, unsigned long long *Lampejetzt, flint::fmpzxx *maxSchritte)>

// Definiere eine Typalias für die Funktion
//#define LampenSchrittVariante std::function<void(unsigned long long*, const flint::fmpzxx*, flint::fmpzxx*, flint::fmpzxx*, unsigned long long*)>

const std::vector<LampenSchrittVariante> LSvarianten = {
	weiterlaufen0,
	weiterlaufen1,
	weiterlaufen2,
	weiterlaufen3,
	weiterlaufen4,
	//weiterlaufen5,
	//weiterlaufen6,
	weiterlaufen7
};

const auto anzLSvarianten = LSvarianten.size();
