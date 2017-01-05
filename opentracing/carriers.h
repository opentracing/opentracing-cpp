#ifndef INCLUDED_OPENTRACING_CARRIERS_H
#define INCLUDED_OPENTRACING_CARRIERS_H

#include <opentracing/config.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include <opentracing/stringref.h>

#include <string>
#include <vector>

namespace opentracing {

struct TextMapPair {
    TextMapPair();
    TextMapPair(const StringRef& name, const StringRef& value);

    std::string m_name;
    std::string m_value;
};

// Text map carriers
template <typename IMPL>
class GenericTextWriter {
  public:
    virtual ~GenericTextWriter();

    int inject(const std::vector<TextMapPair>& textmap);
    // Inject the supplied 'Textmap' into this carrier.

  protected:
    GenericTextWriter();
    GenericTextWriter(const GenericTextWriter&);
    // Protected to avoid direct construction
};

template <typename IMPL>
class GenericTextReader {
  public:
    virtual ~GenericTextReader();

    int extract(std::vector<TextMapPair>* const textmap) const;
    // Extract the supplied 'Textmap' from this carrier.

  protected:
    GenericTextReader();
    GenericTextReader(const GenericTextReader&);
};

// Binary carriers
template <typename IMPL>
class GenericBinaryWriter {
  public:
    virtual ~GenericBinaryWriter();

    int inject(const void* const buf, const size_t len);
    // Inject the supplied 'buf' of 'len' bytes into this carrier.

  protected:
    GenericBinaryWriter();
    GenericBinaryWriter(const GenericBinaryWriter&);
};

template <typename IMPL>
class GenericBinaryReader {
  public:
    virtual ~GenericBinaryReader();

    int extract(void* const buf, size_t* const written, const size_t len) const;
    // Extract up to 'len' bytes of the binary representation of a span context
    // into 'buf' then store the number of bytes 'written'.

  protected:
    GenericBinaryReader();
    GenericBinaryReader(const GenericBinaryReader&);
};

// Explicit carriers
template <typename IMPL, typename CONTEXT>
class GenericWriter {
  public:
    virtual ~GenericWriter();

    int inject(const CONTEXT& context);
    // Inject the supplied 'context' directly into this carrier.

  protected:
    GenericWriter();
    GenericWriter(const GenericWriter&);
};

template <typename IMPL, typename CONTEXT>
class GenericReader {
  public:
    virtual ~GenericReader();

    int extract(CONTEXT* const context) const;
    // Extract the supplied 'context' directly from this carrier.

  protected:
    GenericReader();
    GenericReader(const GenericReader&);
};

// Inline Definitions

inline TextMapPair::TextMapPair() : m_name(), m_value()
{
}

inline TextMapPair::TextMapPair(const StringRef& name, const StringRef& value)
: m_name(name.data(), name.length()), m_value(value.data(), value.length())
{
}

template <typename IMPL>
int
GenericTextWriter<IMPL>::inject(const std::vector<TextMapPair>& textmap)
{
    return static_cast<IMPL*>(this)->injectImp(textmap);
}

template <typename IMPL>
GenericTextWriter<IMPL>::~GenericTextWriter()
{
}

template <typename IMPL>
GenericTextWriter<IMPL>::GenericTextWriter()
{
}

template <typename IMPL>
GenericTextWriter<IMPL>::GenericTextWriter(const GenericTextWriter&)
{
}

template <typename IMPL>
int
GenericTextReader<IMPL>::extract(std::vector<TextMapPair>* const textmap) const
{
    return static_cast<const IMPL*>(this)->extractImp(textmap);
}

template <typename IMPL>
GenericTextReader<IMPL>::~GenericTextReader()
{
}

template <typename IMPL>
GenericTextReader<IMPL>::GenericTextReader()
{
}

template <typename IMPL>
GenericTextReader<IMPL>::GenericTextReader(const GenericTextReader&)
{
}

template <typename IMPL>
int
GenericBinaryWriter<IMPL>::inject(const void* buf, const size_t len)
{
    return static_cast<IMPL*>(this)->injectImp(buf, len);
}

template <typename IMPL>
GenericBinaryWriter<IMPL>::~GenericBinaryWriter()
{
}

template <typename IMPL>
GenericBinaryWriter<IMPL>::GenericBinaryWriter()
{
}

template <typename IMPL>
GenericBinaryWriter<IMPL>::GenericBinaryWriter(const GenericBinaryWriter&)
{
}

template <typename IMPL>
int
GenericBinaryReader<IMPL>::extract(void* const   buf,
                                   size_t* const written,
                                   const size_t  len) const
{
    return static_cast<const IMPL*>(this)->extractImp(buf, written, len);
}

template <typename IMPL>
GenericBinaryReader<IMPL>::~GenericBinaryReader()
{
}

template <typename IMPL>
GenericBinaryReader<IMPL>::GenericBinaryReader()
{
}

template <typename IMPL>
GenericBinaryReader<IMPL>::GenericBinaryReader(const GenericBinaryReader&)
{
}

template <typename IMPL, typename CONTEXT>
int
GenericWriter<IMPL, CONTEXT>::inject(const CONTEXT& context)
{
    return static_cast<IMPL*>(this)->injectImp(context);
}

template <typename IMPL, typename CONTEXT>
GenericWriter<IMPL, CONTEXT>::~GenericWriter()
{
}

template <typename IMPL, typename CONTEXT>
GenericWriter<IMPL, CONTEXT>::GenericWriter()
{
}

template <typename IMPL, typename CONTEXT>
GenericWriter<IMPL, CONTEXT>::GenericWriter(const GenericWriter&)
{
}

template <typename IMPL, typename CONTEXT>
int
GenericReader<IMPL, CONTEXT>::extract(CONTEXT* const context) const
{
    return static_cast<const IMPL*>(this)->extractImp(context);
}

template <typename IMPL, typename CONTEXT>
GenericReader<IMPL, CONTEXT>::~GenericReader()
{
}

template <typename IMPL, typename CONTEXT>
GenericReader<IMPL, CONTEXT>::GenericReader()
{
}

template <typename IMPL, typename CONTEXT>
GenericReader<IMPL, CONTEXT>::GenericReader(const GenericReader&)
{
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_CARRIERS_H
