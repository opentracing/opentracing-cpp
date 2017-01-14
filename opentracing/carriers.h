#ifndef INCLUDED_OPENTRACING_CARRIERS_H
#define INCLUDED_OPENTRACING_CARRIERS_H

// ==========
// carriers.h
// ==========
// class TextMapPair         - Struct of std::string name:value pairs
//
// class GenericTextWriter   - CRTP interface for text map writers (inject)
// class GenericBinaryWriter - CRTP interface for binary writers (inject)
// class GenericWriter       - CRTP interface for explicit writers (inject)
//
// class GenericTextReader   - CRTP interface for text map readers (extract)
// class GenericBinaryReader - CRTP interface for binary readers (extract)
// class GenericReader       - CRTP interface for explicit readers (extract)
//
// ========
// Carriers
// ========
// GenericTextWriter, GenericBinaryWriter, and GenericWriter are used by the
// Tracer's 'inject()' interface. They are used to embed SpanContexts into a
// carrier object. Those carrier objects are passed between process boundaries
// by applications as a part of their existing message passing infrastructure.
//
// When an RPC call is received, the SpanContext can be 'extract()'ed by
// the Tracer using the GenericTextReader, GenericBinaryReader, or GenericReader
// interfaces. If the readers are successful, an immutable SpanContext is
// returned to applications. That SpanContext can then be used to create
// new spans or accessing baggage.

#include <opentracing/stringref.h>
#include <string>
#include <vector>

namespace opentracing {

// =================
// class TextMapPair
// =================
// TextMapPair is a struct used to encapsulate name:value pairs going
// into (inject) and out of (extract) text map carriers.

class TextMapPair {
  public:
    TextMapPair();
    TextMapPair(const StringRef& name, const StringRef& value);

    std::string m_name;
    std::string m_value;
};

// =======================
// class GenericTextWriter
// =======================
// GenericTextWriters are used to 'inject' a list of TextMapPair strings
// into an arbitrary carrier message.
//
// It is templated on the implementation only, which must support:
//
// class TextWriter : GenericTextWriter<TextWriter>{
//   public:
//      int injectImp(const std::vector<TextMapPair>&);
// };
//
// Implementations are responsible for translating the text map passed
// to them into a carrier object. When constructed, Writers should hold
// onto any references they may need to populate the outgoing RPC carrier
// object.

template <typename CARRIER>
class GenericTextWriter {
  public:
    int inject(const std::vector<TextMapPair>& textmap);
    // Inject the supplied 'Textmap' into this carrier.

  protected:
    GenericTextWriter();
    GenericTextWriter(const GenericTextWriter&);
    // Protected to avoid direct construction
};

// =========================
// class GenericBinaryWriter
// =========================
// GenericBinaryWriters are used to 'inject' a binary blob of data into an
// arbitrary carrier message.
//
// It is templated on the implementation only, which must support:
//
// class BinaryWriter : GenericBinaryWriter<BinaryWriter>{
//   public:
//      int injectImp(const void* const buf, const size_t len);
// };
//
// Implementations are responsible for passing the blob along with a carrier
// object. When constructed, Writers should hold onto any references they may
// need to populate the outgoing RPC carrier object.

template <typename CARRIER>
class GenericBinaryWriter {
  public:
    int inject(const void * const buf, const size_t len);
    // Inject the binary representation of a span context into this carrier.

  protected:
    GenericBinaryWriter();
    GenericBinaryWriter(const GenericBinaryWriter&);
};

// ===================
// class GenericWriter
// ===================
// GenericWriters are used to 'inject' an explicit SpanContext into an
// arbitrary carrier message.
//
// If clients want to rely on the details of a particular SpanContext
// implementation (for API simplicity, performance reasons, etc.), they can
// create a GenericWriter that is templated on the *specific* SpanContext
// implementation that they are using. This removes flexibility, making
// it harder to change opentracing-cpp implementations, however,
// it is better to embed the dependency in the type system than rely
// on workarounds such as run time dynamic_cast/reinterpret_cast checks.
//
// For this to work, implementations must support:
//
// class ExplicitWriter: GenericWriter<ExplicitWriter, SpanContextImpl>
// {
//    public:
//      int inject(const SpanContextImpl&);
// };
//
// The Tracer will be responsible for all downcasting, and the explicit
// SpanContextImpl object will be passed directly to the Writer. It can
// then use any of the public API's that the Tracer may be using, to
// interact with the SpanContext directly.
//
// Implementations are responsible for emebeding the details of the
// SpanContextImpl into a carrier object. When constructed  of the
// SpanContextImpl along with a carrier object. When constructed, the outgoing
// RPC carrier objects.

template <typename CARRIER, typename CONTEXT>
class GenericWriter {
  public:
    int inject(const CONTEXT& context);
    // Inject the supplied 'context' directly into this carrier.

  protected:
    GenericWriter();
    GenericWriter(const GenericWriter&);
};

// =======================
// class GenericTextReader
// =======================
// GenericTextReaders are used to 'extract' a list of TextMapPair strings
// out of an arbitrary carrier message.
//
// It is templated on the implementation only, which must support:
//
// class TextReader : GenericTextReader<TextReader>{
//   public:
//      int extractImp(std::vector<TextMapPair>*) const;
// };
//
// Implementations are responsible for translating their carrier object
// into a text map, loading results into the vector they're passed.
// When constructed, they should hold onto any references they may need to
// populate the text map pair. The Tracer then uses that list to populate
// its SpanContexts appropriately.

template <typename CARRIER>
class GenericTextReader {
  public:
    int extract(std::vector<TextMapPair>* const textmap) const;
    // Extract the supplied 'Textmap' from this carrier.

  protected:
    GenericTextReader();
    GenericTextReader(const GenericTextReader&);
};

// =========================
// class GenericBinaryReader
// =========================
// GenericBinaryReaders are used to 'extract' a blob out of an arbitrary carrier
// message.
//
// It is templated on the implementation only, which must support:
//
// class BinaryReader : GenericBinaryReader<BinaryReader>{
//   public:
//      int extractImp(std::vector<char> *const buf) const;
// };
//
// When constructed, they should hold onto any references they may need to
// retrieve the blob when 'extractImp()' is invoked.

template <typename CARRIER>
class GenericBinaryReader {
  public:
    int extract(std::vector<char>* const buffer) const;
    // Load the binary representation of a span context into 'buffer'.
    // Return 0 upon success and a non-zero value otherwise.

  protected:
    GenericBinaryReader();
    GenericBinaryReader(const GenericBinaryReader&);
};

// ===================
// class GenericReader
// ===================
// GenericReaders are used to 'extract' an explicit SpanContext out of an
// arbitrary carrier message.
//
// If clients want to rely on the details of a particular SpanContext
// implementation (for API simplicity, performance reasons, etc.), they can
// create a GenericReader that is templated on the *specific* SpanContext
// implementation that they are using. This removes flexibility, making
// it harder to change opentracing-cpp implementations, however,
// it is better to embed the dependency in the type system than rely
// on workarounds such as run time dynamic_cast/reinterpret_cast checks.
//
// For this to work, implementations must support:
//
// class ExplicitReader: GenericReader<ExplicitReader, SpanContextImpl>
// {
//    public:
//      int extractImp(SpanContextImpl* const) const;
// };
//
// The Tracer will be responsible for downcasting, and the explicit
// SpanContextImpl address will be passed directly to the Reader. The
// Reader can then use any of the public methods that the SpanContextImpl
// exposes, which may be much more than the GenericSpanContext interface.

template <typename CARRIER, typename CONTEXT>
class GenericReader {
  public:
    int extract(CONTEXT* const context) const;
    // Extract the supplied 'context' directly from this carrier.

  protected:
    GenericReader();
    GenericReader(const GenericReader&);
};

// -----------------
// class TextMapPair
// -----------------

inline TextMapPair::TextMapPair() : m_name(), m_value()
{
}

inline TextMapPair::TextMapPair(const StringRef& name, const StringRef& value)
: m_name(name.data(), name.length()), m_value(value.data(), value.length())
{
}

// -----------------------
// class GenericTextWriter
// -----------------------

template <typename CARRIER>
inline int
GenericTextWriter<CARRIER>::inject(const std::vector<TextMapPair>& textmap)
{
    return static_cast<CARRIER*>(this)->injectImp(textmap);
}

template <typename CARRIER>
inline GenericTextWriter<CARRIER>::GenericTextWriter()
{
}

template <typename CARRIER>
inline GenericTextWriter<CARRIER>::GenericTextWriter(const GenericTextWriter&)
{
}

// -------------------------
// class GenericBinaryWriter
// -------------------------

template <typename CARRIER>
inline int
GenericBinaryWriter<CARRIER>::inject(const void* const buf, const size_t len)
{
    return static_cast<CARRIER*>(this)->injectImp(buf, len);
}

template <typename CARRIER>
inline GenericBinaryWriter<CARRIER>::GenericBinaryWriter()
{
}

template <typename CARRIER>
inline GenericBinaryWriter<CARRIER>::GenericBinaryWriter(
    const GenericBinaryWriter&)
{
}

// -------------------
// class GenericWriter
// -------------------

template <typename CARRIER, typename CONTEXT>
inline int
GenericWriter<CARRIER, CONTEXT>::inject(const CONTEXT& context)
{
    return static_cast<CARRIER*>(this)->injectImp(context);
}

template <typename CARRIER, typename CONTEXT>
inline GenericWriter<CARRIER, CONTEXT>::GenericWriter()
{
}

template <typename CARRIER, typename CONTEXT>
inline GenericWriter<CARRIER, CONTEXT>::GenericWriter(const GenericWriter&)
{
}

// -----------------------
// class GenericTextReader
// -----------------------

template <typename CARRIER>
inline int
GenericTextReader<CARRIER>::extract(
    std::vector<TextMapPair>* const textmap) const
{
    return static_cast<const CARRIER*>(this)->extractImp(textmap);
}

template <typename CARRIER>
inline GenericTextReader<CARRIER>::GenericTextReader()
{
}

template <typename CARRIER>
inline GenericTextReader<CARRIER>::GenericTextReader(const GenericTextReader&)
{
}

// -------------------------
// class GenericBinaryReader
// -------------------------

template <typename CARRIER>
inline int
GenericBinaryReader<CARRIER>::extract(std::vector<char>* const buf) const
{
    return static_cast<const CARRIER*>(this)->extractImp(buf);
}

template <typename CARRIER>
inline GenericBinaryReader<CARRIER>::GenericBinaryReader()
{
}

template <typename CARRIER>
inline GenericBinaryReader<CARRIER>::GenericBinaryReader(
    const GenericBinaryReader&)
{
}

// -------------------
// class GenericReader
// -------------------

template <typename CARRIER, typename CONTEXT>
inline int
GenericReader<CARRIER, CONTEXT>::extract(CONTEXT* const context) const
{
    return static_cast<const CARRIER*>(this)->extractImp(context);
}

template <typename CARRIER, typename CONTEXT>
inline GenericReader<CARRIER, CONTEXT>::GenericReader()
{
}

template <typename CARRIER, typename CONTEXT>
inline GenericReader<CARRIER, CONTEXT>::GenericReader(const GenericReader&)
{
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_CARRIERS_H
