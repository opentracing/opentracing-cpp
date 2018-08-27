#ifndef OPENTRACING_MOCKTRACER_UTILITY_H
#define OPENTRACING_MOCKTRACER_UTILITY_H

#include <opentracing/mocktracer/tracer.h>
#include <cstdint>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// If the native architecture is big endian, swaps the endianness of x
uint64_t SwapEndianIfBig(uint64_t x);
uint32_t SwapEndianIfBig(uint32_t x);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_UTILITY_H
