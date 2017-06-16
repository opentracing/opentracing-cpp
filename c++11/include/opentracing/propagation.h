#ifndef OPENTRACING_PROPAGATION_H
#define OPENTRACING_PROPAGATION_H

#include <opentracing/stringref.h>
#include <opentracing/util.h>
#include <opentracing/version.h>
#include <functional>
#include <string>
#include <system_error>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
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

// Returns the std::error_category class used for opentracing propagation
// errors.
//
// See
//   http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-1.html
//   https://ned14.github.io/boost.outcome/md_doc_md_03-tutorial_b.html
const std::error_category& propagation_error_category();

// `unsupported_format_error`` occurs when the `format` passed to
// Tracer::Inject() or Tracer::Extract() is not recognized by the Tracer
// implementation.
const std::error_code unsupported_format_error(0, propagation_error_category());

// `invalid_span_context_error` errors occur when Tracer::Inject() is asked to
// operate on a SpanContext which it is not prepared to handle (for
// example, since it was created by a different tracer implementation).
const std::error_code invalid_span_context_error(1,
                                                 propagation_error_category());

// `invalid_carrier_error` errors occur when Tracer::Inject() or
// Tracer::Extract() implementations expect a different type of `carrier` than
// they are given.
const std::error_code invalid_carrier_error(2, propagation_error_category());

// `span_context_corrupted_error` occurs when the `carrier` passed to
// Tracer::Extract() is of the expected type but is corrupted.
const std::error_code span_context_corrupted_error(
    3, propagation_error_category());

// Base class for implementation-dependent Tracer::Inject carrier-type
// adapter.
class CarrierReader {
 public:
  virtual ~CarrierReader() = default;
};

// Base class for implementation-dependent Tracer::Extract carrier-type adapter.
class CarrierWriter {
 public:
  virtual ~CarrierWriter() = default;
};

// TextMapWriter is the Inject() carrier for the TextMap builtin format. With
// it, the caller can encode a SpanContext for propagation as entries in a map
// of unicode strings.
class TextMapReader : public CarrierReader {
 public:
  // ForeachKey returns TextMap contents via repeated calls to the `f`
  // function. If any call to `f` returns an error, ForeachKey terminates and
  // returns that error.
  //
  // NOTE: The backing store for the TextMapReader may contain data unrelated
  // to SpanContext. As such, Inject() and Extract() implementations that
  // call the TextMapWriter and TextMapReader interfaces must agree on a
  // prefix or other convention to distinguish their own key:value pairs.
  //
  // The "foreach" callback pattern reduces unnecessary copying in some cases
  // and also allows implementations to hold locks while the map is read.
  virtual Expected<void> ForeachKey(
      std::function<Expected<void>(StringRef key, StringRef value)> f)
      const = 0;
};

// TextMapWriter is the Inject() carrier for the TextMap builtin format. With
// it, the caller can encode a SpanContext for propagation as entries in a map
// of unicode strings.
class TextMapWriter : public CarrierWriter {
 public:
  // Set a key:value pair to the carrier. Multiple calls to Set() for the
  // same key leads to undefined behavior.
  //
  // NOTE: The backing store for the TextMapWriter may contain data unrelated
  // to SpanContext. As such, Inject() and Extract() implementations that
  // call the TextMapWriter and TextMapReader interfaces must agree on a
  // prefix or other convention to distinguish their own key:value pairs.
  virtual Expected<void> Set(const std::string& key,
                             const std::string& value) const = 0;
};

// HTTPHeadersReader is the Inject() carrier for the HttpHeaders builtin format.
// With it, the caller can encode a SpanContext for propagation as entries in
// http request headers.
class HTTPHeadersReader : public TextMapReader {};

// HTTPHeadersWriter is the Inject() carrier for the TextMap builtin format.
// With it, the caller can encode a SpanContext for propagation as entries in
// http request headers
class HTTPHeadersWriter : public TextMapWriter {};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_PROPAGATION_H
