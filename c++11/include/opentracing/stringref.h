#ifndef OPENTRACING_STRINGREF_H
#define OPENTRACING_STRINGREF_H

#include <opentracing/version.h>
#include <cstring>
#include <ostream>
#include <string>

// ===========
// stringref.h
// ===========
// class StringRef - Constant reference to an external string
//
// -----------------
// String References
// -----------------
// This string references is a simplified version of the boost::string_ref.
// Its purpose is to avoid a number of efficiency problems that appear
// commonly when interacting with 'std::string' and c-strings.
//
// See the boost documentation for more background:
// http://www.boost.org/doc/libs/master/libs/utility/doc/html/string_ref.html
//
// -----
// Note:
// -----
// Although we have the ability to use wide string refs, there are side
// effects in exposing an OpenTracing interface that works with narrow and wide
// strings at the same time. Storage on the implementation will have a 'native'
// format.
//
// Exposing references to that format avoid copies means clients would be
// dependent on that format. If they're dependent on that detail and then switch
// out the implementation to a different format, there would be lots of code
// that breaks if it was expecting wstring and starts receiving string all of a
// sudden. That design issue still needs to be addressed.

namespace opentracing {
inline namespace OPENTRACING_INLINE_NAMESPACE {
// ===============
// class StringRef
// ===============
// Represent a constant reference to an external character array. The external
// array need not be null-terminated, if explicitly created with a known length.
//
// This class does not own the data. It is expected to be used in situations
// where the character data resides in some other buffer, whose lifetime extends
// past that of the StringRef. For this reason, it is not in general safe to
// store a StringRef.

class StringRef {
 public:
  // Construct an empty StringRef
  StringRef() noexcept : data_(nullptr), length_(0) {}

  // Explicitly create string reference from a const character array
  template <size_t N>
  StringRef(const char (&str)[N]) noexcept : data_(str), length_(N - 1) {}

  // Explicitly create string reference from const character pointer
  explicit StringRef(const char* str) noexcept
      : data_(str), length_(std::strlen(str)) {}

  // Create constant string reference from pointer and length
  StringRef(const std::basic_string<char>& str) noexcept
      : data_(str.c_str()), length_(str.length()) {}

  // Create constant string reference from pointer and length
  StringRef(const char* str, size_t len) noexcept : data_(str), length_(len) {}

  // Disallow construction from non-const array
  template <size_t N>
  StringRef(char (&str)[N]) = delete;

  // Implicit conversion to std::string
  operator std::string() const { return {data_, length_}; }

  // Reset the string reference given a const character array
  template <size_t N>
  void reset(const char (&str)[N]) noexcept {
    data_ = str;
    length_ = N;
  }

  // Reset this string ref to point at the supplied c-string
  void reset(const char* str) noexcept {
    data_ = str;
    length_ = std::strlen(str);
  }

  // Reset the string reference given a std::string
  void reset(const std::basic_string<char>& str) noexcept {
    data_ = str.data();
    length_ = str.length();
  }

  // Reset this string ref to point at the supplied 'str' of 'length' bytes.
  void reset(const char* str, const size_t length) noexcept {
    data_ = str;
    length_ = length;
  }

  // Disallow reset from non-const array
  template <size_t N>
  void reset(char (&str)[N]) = delete;

  // Return address of the referenced string
  const char* data() const noexcept { return data_; }

  // Return the length of the referenced string
  size_t length() const noexcept { return length_; }

 private:
  const char* data_;  // Pointer to external storage
  size_t length_;     // Length of data pointed to by 'data_'
};

inline std::ostream& operator<<(std::ostream& os,
                                const opentracing::StringRef& ref) {
  return os.write(ref.data(), static_cast<std::streamsize>(ref.length()));
}

}  // namespace OPENTRACING_INLINE_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_STRINGREF_H
