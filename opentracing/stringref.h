#ifndef INCLUDED_OPENTRACING_STRINGREF_H
#define INCLUDED_OPENTRACING_STRINGREF_H

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

#include <cstring>
#include <ostream>
#include <string>

namespace opentracing {

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
    StringRef();
    // Construct an empty StringRef

    template <size_t N>
    StringRef(const char (&str)[N]);
    // Explicitly create string reference from a const character array

    explicit StringRef(const char* str);
    // Explicitly create string reference from const character pointer

    StringRef(const std::basic_string<char>& str);
    // Create constant string reference from pointer and length

    StringRef(const char* str, size_t len);
    // Create constant string reference from pointer and length

    operator const char*() const;
    // Implicit conversion to plain char *

    template <size_t N>
    void             reset(const char (&str)[N]);
    // Reset the string reference given a const character array

    void reset(const char* str);
    // Reset this string ref to point at the supplied c-string

    void reset(const std::basic_string<char>& str);
    // Reset the string reference given a std::string

    void reset(const char* str, const size_t length);
    // Reset this string ref to point at the supplied 'str' of 'length' bytes.

    const char* data() const;
    // Return address of the referenced string

    size_t length() const;
    // Return the length of the referenced string

  private:
    template <size_t N>
    StringRef(char (&str)[N]);
    // Disallow construction from non-const array

    template <size_t N>
    void             reset(char (&str)[N]);
    // Disallow reset from non-const array

    const char* m_data;    // Pointer to external storage
    size_t      m_length;  // Length of data pointed to by 'm_data'
};

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

// ---------------
// Class StringRef
// ---------------

inline StringRef::StringRef() : m_data(0), m_length(0)
{
}

template <size_t N>
StringRef::StringRef(const char (&str)[N]) : m_data(str), m_length(N - 1)
{
}

inline StringRef::StringRef(const char* str)
: m_data(str), m_length(std::strlen(str))
{
}

inline StringRef::StringRef(const std::basic_string<char>& str)
: m_data(str.c_str()), m_length(str.length())
{
}

inline StringRef::StringRef(const char* str, size_t len)
: m_data(str), m_length(len)
{
}

inline StringRef::operator const char*() const
{
    return m_data;
}

inline void
StringRef::reset(const char* str, const size_t length)
{
    m_data   = str;
    m_length = length;
}

template <size_t N>
void
StringRef::reset(const char (&str)[N])
{
    m_data   = str;
    m_length = N;
}

inline void
StringRef::reset(const char* str)
{
    m_data   = str;
    m_length = std::strlen(str);
}

inline void
StringRef::reset(const std::basic_string<char>& str)
{
    m_data   = str.data();
    m_length = str.length();
}

inline const char*
StringRef::data() const
{
    return m_data;
}

inline size_t
StringRef::length() const
{
    return m_length;
}

}  // namespace opentracing

inline std::ostream&
operator<<(std::ostream& os, const opentracing::StringRef& ref)
{
    return os.write(ref.data(), ref.length());
}

#endif  // INCLUDED_OPENTRACING_STRINGREF_H
