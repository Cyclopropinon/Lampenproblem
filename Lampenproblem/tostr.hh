/* IMPLEMENTATION */

/* calculate absolute value */
constexpr int abs_val (int x)
    { return x < 0 ? -x : x; }

/* calculate number of digits needed, including minus sign */
constexpr int num_digits (int x)
    { return x < 0 ? 1 + num_digits (-x) : x < 10 ? 1 : 1 + num_digits (x / 10); }

/* metaprogramming string type: each different string is a unique type */
template<char... args>
struct metastring {
    const char data[sizeof... (args)] = {args...};
};

/* recursive number-printing template, general case (for three or more digits) */
template<int size, int x, char... args>
struct numeric_builder {
    typedef typename numeric_builder<size - 1, x / 10, '0' + abs_val (x) % 10, args...>::type type;
};

/* special case for two digits; minus sign is handled here */
template<int x, char... args>
struct numeric_builder<2, x, args...> {
    typedef metastring<x < 0 ? '-' : '0' + x / 10, '0' + abs_val (x) % 10, args...> type;
};

/* special case for one digit (positive numbers only) */
template<int x, char... args>
struct numeric_builder<1, x, args...> {
    typedef metastring<'0' + x, args...> type;
};

/* convenience wrapper for numeric_builder */
template<int x>
class numeric_string
{
private:
    /* generate a unique string type representing this number */
    typedef typename numeric_builder<num_digits (x), x, '\0'>::type type;

    /* declare a static string of that type (instantiated later at file scope) */
    static constexpr type value {};

public:
    /* returns a pointer to the instantiated string */
    static constexpr const char * get ()
        { return value.data; }
};

/* instantiate numeric_string::value as needed for different numbers */
template<int x>
constexpr typename numeric_string<x>::type numeric_string<x>::value;

/* SAMPLE USAGE * /

#include <stdio.h>

/* exponentiate a number, just for fun * /
static constexpr int exponent (int x, int e)
    { return e ? x * exponent (x, e - 1) : 1; }


/* test a few sample numbers * /
static constexpr const char * five = numeric_string<5>::get ();
static constexpr const char * one_ten = numeric_string<110>::get ();
static constexpr const char * minus_thirty = numeric_string<-30>::get ();

/* works for any constant integer, including constexpr calculations * /
static constexpr const char * eight_cubed = numeric_string<exponent (8, 3)>::get ();

int main (void)
{
    printf ("five = %s\n", five);
    printf ("one ten = %s\n", one_ten);
    printf ("minus thirty = %s\n", minus_thirty);
    printf ("eight cubed = %s\n", eight_cubed);

    return 0;
}*/
