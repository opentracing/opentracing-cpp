#ifndef INCLUDED_OPENTRACING_CARRIERS_H
#define INCLUDED_OPENTRACING_CARRIERS_H

#include <opentracing/config.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include <map>
#include <string>

namespace opentracing {

// Text map carriers
template <typename IMPL>
class GenericTextWriter {
  public:
    int inject(const std::map<std::string, std::string>& Textmap);
    // Inject the supplied 'Textmap' into this carrier.

  protected:
    GenericTextWriter();
    GenericTextWriter(const GenericTextWriter&);
    // Protected to avoid direct construction
};

template <typename IMPL>
class GenericTextReader {
  public:
    int extract(std::map<std::string, std::string>* const Textmap) const;
    // Extract the supplied 'Textmap' from this carrier.

  protected:
    GenericTextReader();
    GenericTextReader(const GenericTextReader&);
};

// Binary carriers
template <typename IMPL>
class GenericBinaryWriter {
  public:
    int inject(const void* const buf, const size_t len);
    // Inject the supplied 'buf' of 'len' bytes into this carrier.

  protected:
    GenericBinaryWriter();
    GenericBinaryWriter(const GenericBinaryWriter&);
};

template <typename IMPL>
class GenericBinaryReader {
  public:
    int extract(void* const buf, const size_t len) const;
    // Extract up to 'len' bytes of the binary representation of a span context
    // into 'buf'.

  protected:
    GenericBinaryReader();
    GenericBinaryReader(const GenericBinaryReader&);
};

// Explicit carriers
template <typename IMPL, typename SPAN_CONTEXT>
class GenericWriter {
  public:
    int inject(const SPAN_CONTEXT& context);
    // Inject the supplied 'context' directly into this carrier.

  protected:
    GenericWriter();
    GenericWriter(const GenericWriter&);
};

template <typename IMPL, typename SPAN_CONTEXT>
class GenericReader {
  public:
    int extract(SPAN_CONTEXT* const context) const;
    // Extract the supplied 'context' directly from this carrier.

  private:
    GenericReader();
    GenericReader(const GenericReader&);
};

template <typename IMPL>
int
GenericTextWriter::inject(const std::map<std::string, std::string>& Textmap)
{
    return static_cast<IMPL*>(this)->inject(Textmap);
}

template <typename IMPL>
GenericTextWriter::GenericTextWriter()
{
}

template <typename IMPL>
GenericTextWriter::GenericTextWriter(const GenericTextWriter&)
{
}

template <typename IMPL>
int
GenericTextReader::extract(
    std::map<std::string, std::string>* const Textmap) const
{
    return static_cast<IMPL*>(this)->extract(Textmap);
}

template <typename IMPL>
GenericTextReader::GenericTextReader()
{
}

template <typename IMPL>
GenericTextReader::GenericTextReader(const GenericTextReader&)
{
}

template <typename IMPL>
int
GenericBinaryWriter::inject(const void* buf, const size_t len)
{
    return static_cast<IMPL*>(this)->inject(buf, len);
}

template <typename IMPL>
GenericBinaryWriter::GenericBinaryWriter()
{
}

template <typename IMPL>
GenericBinaryWriter::GenericBinaryWriter(const GenericBinaryWriter&)
{
}

template <typename IMPL>
int
GenericBinaryReader::extract(void* const buf, const size_t len) const
{
    return static_cast<IMPL*>(this)->extract(buf, len);
}

template <typename IMPL>
GenericBinaryReader::GenericBinaryReader()
{
}

template <typename IMPL>
GenericBinaryReader::GenericBinaryReader(const GenericBinaryReader&)
{
}

template <typename IMPL, typename SPAN_CONTEXT>
int
GenericWriter::inject(const SPAN_CONTEXT& context)
{
    return static_cast<IMPL*>(this)->inject(context);
}

template <typename IMPL>
GenericWriter::GenericWriter()
{
}

template <typename IMPL>
GenericWriter::GenericWriter(const GenericWriter&)
{
}

template <typename IMPL, typename SPAN_CONTEXT>
int
GenericReader::extract(SPAN_CONTEXT* const context) const
{
    return static_cast<IMPL*>(this)->extract(context);
}

template <typename IMPL>
GenericReader::GenericReader()
{
}

template <typename IMPL>
GenericReader::GenericReader(const GenericReader&)
{
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_CARRIERS_H
