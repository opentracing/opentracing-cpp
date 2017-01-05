#ifndef INCLUDED_OPENTRACING_STRINGREF_H
#define INCLUDED_OPENTRACING_STRINGREF_H

#include <opentracing/config.h>

#include <cstring>
#include <string>

namespace opentracing {

// ---------
// StringRef
// ---------
// Represent a constant reference to an external character, which does not have
// to be null terminated. This class does not own the string data. It is
// expected to be used in situations where the character data resides in some
// other buffer, whose lifetime extends past that of the StringRef. For this
// reason, it is not in general safe to store a StringRef.

struct StringRef {
  public:
    StringRef();

    template <size_t N>
    StringRef(const char (&str)[N]);
    // Explicitly create string reference from a const character array

    explicit StringRef(const char* str);
    // Explicitly create string reference from const character pointer

    StringRef(const std::string& str);
    // Create constant string reference from pointer and length

    StringRef(const char* str, size_t len);
    // Create constant string reference from pointer and length

    operator const char*() const;
    // Implicit conversion to plain char *

    template <size_t N>
    void             reset(const char (&str)[N]);
    // Reset the string reference given a const character array

    void reset(const char* const str);
    // Reset this string ref to point at the supplied c-string

    void reset(const std::string& str);
    // Reset the string reference given a std::string

    void reset(const char* const str, const size_t length);
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

    const char* m_data;
    size_t      m_length;
};

inline StringRef::StringRef() : m_data(0), m_length(0)
{
}

template <size_t N>
inline StringRef::StringRef(const char (&str)[N]) : m_data(str), m_length(N - 1)
{
}

inline StringRef::StringRef(const char* const str)
: m_data(str), m_length(std::strlen(str))
{
}

inline StringRef::StringRef(const std::string& str)
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
StringRef::reset(const char* const str, const size_t length)
{
    m_data   = str;
    m_length = length;
}

template <size_t N>
inline void
StringRef::reset(const char (&str)[N])
{
    m_data   = str;
    m_length = N;
}

inline void
StringRef::reset(const char* const str)
{
    m_data   = str;
    m_length = std::strlen(str);
}

inline void
StringRef::reset(const std::string& str)
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
#endif  // INCLUDED_OPENTRACING_STRINGREF_H
