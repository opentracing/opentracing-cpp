#ifndef OPENTRACING_PROPAGATION_H
#define OPENTRACING_PROPAGATION_H

#include <opentracing/preprocessor.h>
#include <opentracing/util.h>
#include <functional>
#include <string>

namespace opentracing {
inline namespace OPENTRACING_VERSION_NAMESPACE {
enum class SpanReferenceType {
  // ChildOfRef refers to a parent Span that caused *and* somehow depends
  // upon the new child Span. Often (but not always), the parent Span cannot
  // finish unitl the child Span does.
  //
  // An timing diagram for a ChildOfRef that's blocked on the new Span:
  //
  //     [-Parent Span---------]
  //          [-Child Span----]
  //
  // See http://opentracing.io/spec/
  //
  // See opentracing.ChildOf()
  ChildOfRef = 1,

  // FollowsFromRef refers to a parent Span that does not depend in any way
  // on the result of the new child Span. For instance, one might use
  // FollowsFromRefs to describe pipeline stages separated by queues,
  // or a fire-and-forget cache insert at the tail end of a web request.
  //
  // A FollowsFromRef Span is part of the same logical trace as the new Span:
  // i.e., the new Span is somehow caused by the work of its FollowsFromRef.
  //
  // All of the following could be valid timing diagrams for children that
  // "FollowFrom" a parent.
  //
  //     [-Parent Span-]  [-Child Span-]
  //
  //
  //     [-Parent Span--]
  //      [-Child Span-]
  //
  //
  //     [-Parent Span-]
  //                 [-Child Span-]
  //
  // See http://opentracing.io/spec/
  //
  // See opentracing.FollowsFrom()
  FollowsFromRef = 2
};

// Carrier format values.
enum class CarrierFormat {
  // OpenTracingBinary encodes the SpanContext for propagation as opaque
  // binary data.
  OpenTracingBinary = 1,  // RESERVED, NOT IMPLEMENTED

  // HTTPHeaders represents SpanContexts as HTTP header string pairs.
  //
  // The HTTPHeaders format requires that the keys and values be valid
  // as HTTP headers as-is (i.e., character casing may be unstable and
  // special characters are disallowed in keys, values should be
  // URL-escaped, etc).
  //
  // For Tracer::Inject(): the carrier must be a `TextMapReader`.
  //
  // For Tracer::Extract(): the carrier must be a `TextMapWriter`.
  //
  // For example, Inject():
  //
  //   std::vector<std::pair<std::string, std::string>> *headers = ...;
  //   if (!span.tracer().Inject(span, CarrierFormat::HTTPHeadersCarrier,
  // 	  			   make_ordered_string_pairs_writer(headers))) {
  //     throw error("inject failed");
  //   }
  //
  // Or Extract():
  //
  //   SpanContext extracted;
  //   extracted = Tracer::Global().Extract(CarrierFormat::HTTPHeadersCarrier,
  //                                        make_ordered_string_pairs_reader(*headers));
  //   auto span = Tracer::Global().StartSpan("op", { ChildOf(extracted) });
  //
  HTTPHeaders = 2,

  // TextMap encodes the SpanContext as key:value pairs.
  //
  // The TextMap format is similar to the HTTPHeaderes format,
  // without restrictions on the character set.
  //
  // For Tracer::Inject(): the carrier must be a `TextMapReader`.
  //
  // For Tracer::Extract(): the carrier must be a `TextMapWriter`.
  //
  // See the HTTPHeaders examples.
  TextMap = 3
};

// Base class for implementation-dependent Tracer::Inject carrier-type adapter.
class CarrierReader {
 public:
  virtual ~CarrierReader() = default;
};

// Basic foundation for OpenTracing basictracer-compatible carrier readers.
class BasicCarrierReader : public CarrierReader {
 public:
  virtual void ForeachKey(
      std::function<void(const std::string& key, const std::string& value)> f)
      const = 0;
};

// Base class for implementation-dependent Tracer::Extract carrier-type adapter.
class CarrierWriter {
 public:
  virtual ~CarrierWriter() = default;
};

// Basic foundation for OpenTracing basictracer-compatible carrier writers.
class BasicCarrierWriter : public CarrierWriter {
 public:
  virtual Expected<void, std::string> Set(const std::string& key,
                                          const std::string& value) const = 0;
};

// Base class for injecting into TextMap and HTTPHeaders carriers.
class TextMapReader : public BasicCarrierReader {
  // TODO distinguish TextMap and HTTPHeaders behavior.
};

// Base class for extracting from TextMap and HTTPHeaders carriers.
class TextMapWriter : public BasicCarrierWriter {
  // TODO distinguish TextMap and HTTPHeaders behavior.
};
}  // namespace OPENTRACING_VERSION_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_PROPAGATION_H
