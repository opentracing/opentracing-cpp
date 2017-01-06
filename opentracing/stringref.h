#ifndef INCLUDED_OPENTRACING_STRINGREF_H
#define INCLUDED_OPENTRACING_STRINGREF_H

// ===========
// stringref.h
// ===========
// class StringRefImp    - Templated implementation for a constant reference to an external string
// typedef StringRef     - Typedef for StringRefImp<char>
// typedef StringRefWide - Typedef for StringRefImp<wchar_t>
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

#include <opentracing/config.h>
#include <cstring>
#include <cwchar>
#include <string>

namespace opentracing {

// ==================
// class StringRefImp
// ==================
// Represent a constant reference to an external character array. The external
// array need not be null-terminated, if explicitly created with a known length.
//
// This class does not own the data. It is expected to be used in situations
// where the character data resides in some other buffer, whose lifetime extends
// past that of the StringRefImp. For this reason, it is not in general safe to
// store a StringRefImp.

template <typename CHAR>
class StringRefImp {
  public:
    StringRefImp();
    // Conststruct an empty StringRef

    template <size_t N>
    StringRefImp(const CHAR (&str)[N]);
    // Explicitly create string reference from a const CHARacter array

    explicit StringRefImp(const CHAR* str);
    // Explicitly create string reference from const CHARacter pointer

    StringRefImp(const std::basic_string<CHAR>& str);
    // Create constant string reference from pointer and length

    StringRefImp(const CHAR* str, size_t len);
    // Create constant string reference from pointer and length

    operator const CHAR*() const;
    // Implicit conversion to plain CHAR *

    template <size_t N>
    void             reset(const CHAR (&str)[N]);
    // Reset the string reference given a const CHARacter array

    void reset(const CHAR* const str);
    // Reset this string ref to point at the supplied c-string

    void reset(const std::basic_string<CHAR>& str);
    // Reset the string reference given a std::string

    void reset(const CHAR* const str, const size_t length);
    // Reset this string ref to point at the supplied 'str' of 'length' bytes.

    const CHAR* data() const;
    // Return address of the referenced string

    size_t length() const;
    // Return the length of the referenced string

  private:
    static size_t getLength(const CHAR* len);
    // Similar to strlen, but for both char/wchar types

    template <size_t N>
    StringRefImp(CHAR (&str)[N]);
    // Disallow construction from non-const array

    template <size_t N>
    void             reset(CHAR (&str)[N]);
    // Disallow reset from non-const array

    const CHAR* m_data;    // Pointer to external storage
    size_t      m_length;  // Length of data pointed to by 'm_data'
};

// --------
// Typedefs
// --------

typedef StringRefImp<char> StringRef;
typedef StringRefImp<wchar_t> StringRefWide;

// ------------------
// Class StringRefImp
// ------------------

template <typename CHAR>
inline StringRefImp<CHAR>::StringRefImp() : m_data(0), m_length(0)
{
}

template <typename CHAR>
template <size_t N>
inline StringRefImp<CHAR>::StringRefImp(const CHAR (&str)[N]) : m_data(str), m_length(N - 1)
{
}

template <typename CHAR>
inline StringRefImp<CHAR>::StringRefImp(const CHAR* const str)
: m_data(str), m_length(getLength(str))
{
}

template <typename CHAR>
inline StringRefImp<CHAR>::StringRefImp(const std::basic_string<CHAR>& str)
: m_data(str.c_str()), m_length(str.length())
{
}

template <typename CHAR>
inline StringRefImp<CHAR>::StringRefImp(const CHAR* str, size_t len)
: m_data(str), m_length(len)
{
}

template <typename CHAR>
inline StringRefImp<CHAR>::operator const CHAR*() const
{
    return m_data;
}

template <typename CHAR>
inline void
StringRefImp<CHAR>::reset(const CHAR* const str, const size_t length)
{
    m_data   = str;
    m_length = length;
}

template <typename CHAR>
template <size_t N>
inline void
StringRefImp<CHAR>::reset(const CHAR (&str)[N])
{
    m_data   = str;
    m_length = N;
}

template <typename CHAR>
inline void
StringRefImp<CHAR>::reset(const CHAR* const str)
{
    m_data   = str;
    m_length = getLength(str);
}

template <typename CHAR>
inline void
StringRefImp<CHAR>::reset(const std::basic_string<CHAR>& str)
{
    m_data   = str.data();
    m_length = str.length();
}

template <typename CHAR>
inline const CHAR*
StringRefImp<CHAR>::data() const
{
    return m_data;
}

template <typename CHAR>
inline size_t
StringRefImp<CHAR>::length() const
{
    return m_length;
}

template <typename CHAR>
inline size_t
StringRefImp<CHAR>::getLength(const CHAR* s)
{
    const CHAR* p = s;
    while (*p) ++p;
    return size_t(p - s);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_STRINGREF_H
