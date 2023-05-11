namespace boost{ namespace multiprecision{

typedef unspecified-type limb_type;

enum cpp_integer_type    { signed_magnitude, unsigned_magnitude };
enum cpp_int_check_type  { checked, unchecked };

template <std::size_t MinBits = 0,
          std::size_t MaxBits = 0,
          cpp_integer_type SignType = signed_magnitude,
          cpp_int_check_type Checked = unchecked,
          class Allocator = std::allocator<limb_type> >
class cpp_int_backend;
//
// Expression templates default to et_off if there is no allocator:
//
template <std::size_t MinBits, std::size_t MaxBits, cpp_integer_type SignType, cpp_int_check_type Checked>
struct expression_template_default<cpp_int_backend<MinBits, MaxBits, SignType, Checked, void> >
{ static const expression_template_option value = et_off; };

typedef number<cpp_int_backend<> >              cpp_int;    // arbitrary precision integer
typedef rational_adaptor<cpp_int_backend<> >    cpp_rational_backend;
typedef number<cpp_rational_backend>            cpp_rational; // arbitrary precision rational number

// Fixed precision unsigned types:
typedef number<cpp_int_backend<128, 128, unsigned_magnitude, unchecked, void> >   uint128_t;
typedef number<cpp_int_backend<256, 256, unsigned_magnitude, unchecked, void> >   uint256_t;
typedef number<cpp_int_backend<512, 512, unsigned_magnitude, unchecked, void> >   uint512_t;
typedef number<cpp_int_backend<1024, 1024, unsigned_magnitude, unchecked, void> > uint1024_t;

// Fixed precision signed types:
typedef number<cpp_int_backend<128, 128, signed_magnitude, unchecked, void> >     int128_t;
typedef number<cpp_int_backend<256, 256, signed_magnitude, unchecked, void> >     int256_t;
typedef number<cpp_int_backend<512, 512, signed_magnitude, unchecked, void> >     int512_t;
typedef number<cpp_int_backend<1024, 1024, signed_magnitude, unchecked, void> >   int1024_t;

// Over again, but with checking enabled this time:
typedef number<cpp_int_backend<0, 0, signed_magnitude, checked> >                 checked_cpp_int;
typedef rational_adaptor<cpp_int_backend<0, 0, signed_magnitude, checked> >       checked_cpp_rational_backend;
typedef number<cpp_rational_backend>                                              checked_cpp_rational;

// Checked fixed precision unsigned types:
typedef number<cpp_int_backend<128, 128, unsigned_magnitude, checked, void> >     checked_uint128_t;
typedef number<cpp_int_backend<256, 256, unsigned_magnitude, checked, void> >     checked_uint256_t;
typedef number<cpp_int_backend<512, 512, unsigned_magnitude, checked, void> >     checked_uint512_t;
typedef number<cpp_int_backend<1024, 1024, unsigned_magnitude, checked, void> >   checked_uint1024_t;

// Fixed precision signed types:
typedef number<cpp_int_backend<128, 128, signed_magnitude, checked, void> >       checked_int128_t;
typedef number<cpp_int_backend<256, 256, signed_magnitude, checked, void> >       checked_int256_t;
typedef number<cpp_int_backend<512, 512, signed_magnitude, checked, void> >       checked_int512_t;
typedef number<cpp_int_backend<1024, 1024, signed_magnitude, checked, void> >     checked_int1024_t;

}} // namespaces
