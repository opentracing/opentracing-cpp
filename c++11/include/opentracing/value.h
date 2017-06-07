#ifndef OPENTRACING_VALUE_H
#define OPENTRACING_VALUE_H

#include <cstdint>
#include <opentracing/3rd_party/mapbox_variant/variant.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace opentracing {
// Variant value types for span tags and log payloads.
class Value;

typedef std::unordered_map<std::string, Value> Dictionary;
typedef std::vector<Value> Values;
typedef mapbox::util::variant<bool, double, int64_t, uint64_t, std::string,
                              std::nullptr_t, const char*,
                              mapbox::util::recursive_wrapper<Values>,
                              mapbox::util::recursive_wrapper<Dictionary>>
    variant_type;

class Value : public variant_type {
 public:
  Value() : variant_type(nullptr) {}

  template <typename T>
  Value(T&& t) : variant_type(std::forward<T>(t)) {}

  template <int N>
  Value(const char (&cstr)[N]) : variant_type(std::string(cstr)) {}
};
}  // namespace opentracing

#endif  // OPENTRACING_VALUE_H
