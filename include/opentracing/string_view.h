#ifndef OPENTRACING_STRING_VIEW_H
#define OPENTRACING_STRING_VIEW_H

#include <opentracing/version.h>
#include <algorithm>
#include <cstring>
#include <ostream>
#include <string>

// ===========
// string_view.h
// ===========
// class string_view - Constant reference to an external string
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
BEGIN_OPENTRACING_ABI_NAMESPACE
// ===============
// class string_view
// ===============
// Represent a constant reference to an external character array. The external
// array need not be null-terminated, if explicitly created with a known length.
//
// This class does not own the data. It is expected to be used in situations
// where the character data resides in some other buffer, whose lifetime extends
// past that of the string_view. For this reason, it is not in general safe to
// store a string_view.

class string_view {
 public:
  // Construct an empty string_view
  string_view() noexcept : data_(nullptr), length_(0) {}

  // create string reference from const character pointer
  string_view(const char* str) noexcept
      : data_(str), length_(std::strlen(str)) {}

  // Create constant string reference from pointer and length
  string_view(const std::basic_string<char>& str) noexcept
      : data_(str.c_str()), length_(str.length()) {}

  // Create constant string reference from pointer and length
  string_view(const char* str, size_t len) noexcept
      : data_(str), length_(len) {}

  // Implicit conversion to std::string
  operator std::string() const { return {data_, length_}; }

  // Return address of the referenced string
  const char* data() const noexcept { return data_; }

  // Returns true if `length_` == 0
  bool empty() const noexcept { return length_ == 0; }

  // Return the length of the referenced string
  size_t length() const noexcept { return length_; }
  size_t size() const noexcept { return length_; }

  // Returns a RandomAccessIterator to the first element.
  const char* begin() const noexcept { return data(); }

  // Returns a RandomAccessIterator for the last element.
  const char* end() const noexcept { return data() + length(); }

  // Returns the character in the i-th position.
  const char& operator[](std::size_t i) { return *(data() + i); }

 private:
  const char* data_;  // Pointer to external storage
  size_t length_;     // Length of data pointed to by 'data_'
};

inline bool operator==(string_view lhs, string_view rhs) noexcept {
  return lhs.length() == rhs.length() &&
         std::equal(lhs.data(), lhs.data() + lhs.length(), rhs.data());
}

inline bool operator==(string_view lhs, const std::string& rhs) noexcept {
  return lhs == string_view(rhs);
}

inline bool operator==(const std::string& lhs, string_view rhs) noexcept {
  return string_view(lhs) == rhs;
}

inline bool operator==(string_view lhs, const char* rhs) noexcept {
  return lhs == string_view(rhs);
}

inline bool operator==(const char* lhs, string_view rhs) noexcept {
  return string_view(lhs) == rhs;
}

inline bool operator!=(string_view lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(string_view lhs, const std::string& rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(const std::string& lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(string_view lhs, const char* rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(const char* lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os,
                                const opentracing::string_view& ref) {
  return os.write(ref.data(), static_cast<std::streamsize>(ref.length()));
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_STRING_VIEW_H
