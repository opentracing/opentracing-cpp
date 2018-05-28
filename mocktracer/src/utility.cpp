#include "utility.h"
#include <climits>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

// Converts swaps the endianness of a number.
//
// Taken from https://stackoverflow.com/a/105339/4447365
template <typename T>
static T SwapEndian(T u) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

  union {
    T u;
    unsigned char u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); k++)
    dest.u8[k] = source.u8[sizeof(T) - k - 1];

  return dest.u;
}

// Determines whether the architecture is big endian.
//
// Taken from https://stackoverflow.com/a/1001373/4447365
static bool IsBigEndian() {
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};

  return bint.c[0] == 1;
}

uint64_t SwapEndianIfBig(uint64_t x) {
  if (IsBigEndian()) {
    return SwapEndian(x);
  } else {
    return x;
  }
}

uint32_t SwapEndianIfBig(uint32_t x) {
  if (IsBigEndian()) {
    return SwapEndian(x);
  } else {
    return x;
  }
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
