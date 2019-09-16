#ifndef OPENTRACING_VALUE_H
#define OPENTRACING_VALUE_H

#include <opentracing/string_view.h>
#include <opentracing/version.h>
#include <cstdint>
#include <opentracing/variant/variant.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// Variant value types for span tags and log payloads.
class Value;

typedef std::unordered_map<std::string, Value> Dictionary;
typedef std::vector<Value> Values;
typedef util::variant<bool, double, int64_t, uint64_t, std::string,
                      opentracing::string_view, std::nullptr_t, const char*,
                      util::recursive_wrapper<Values>,
                      util::recursive_wrapper<Dictionary>>
    variant_type;

class Value : public variant_type {
 public:
  Value() noexcept : variant_type(nullptr) {}
  Value(std::nullptr_t) noexcept : variant_type(nullptr) {}

  // variant_type's constructors will do some undesirable casting, for example
  //      variant_type(123)
  // will construct a bool variant; hence, constructors are expanded
  // out so as to provide more sensible behavior.
  Value(bool x) noexcept : variant_type(x) {}

  template <typename T,
            typename std::enable_if<std::is_integral<T>::value &&
                                    std::is_signed<T>::value>::type* = nullptr>
  Value(T t) noexcept : variant_type(static_cast<int64_t>(t)) {}

  template <typename T, typename std::enable_if<
                            std::is_integral<T>::value &&
                            std::is_unsigned<T>::value>::type* = nullptr>
  Value(T t) noexcept : variant_type(static_cast<uint64_t>(t)) {}

  template <typename T, typename std::enable_if<
                            std::is_floating_point<T>::value>::type* = nullptr>
  Value(T t) noexcept : variant_type(static_cast<double>(t)) {}

  Value(const char* s) noexcept : variant_type(s) {}

  template <int N>
  Value(const char (&cstr)[N]) : variant_type(std::string(cstr)) {}

  Value(const std::string& s) : variant_type(s) {}
  Value(std::string&& s) : variant_type(std::move(s)) {}
  Value(opentracing::string_view s) noexcept : variant_type(s) {}

  Value(const Values& values) : variant_type(values) {}
  Value(Values&& values) : variant_type(std::move(values)) {}

  Value(const Dictionary& values) : variant_type(values) {}
  Value(Dictionary&& values) : variant_type(std::move(values)) {}
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_VALUE_H
