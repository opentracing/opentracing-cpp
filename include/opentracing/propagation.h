#ifndef OPENTRACING_PROPAGATION_H
#define OPENTRACING_PROPAGATION_H

#include <opentracing/string_view.h>
#include <opentracing/symbols.h>
#include <opentracing/util.h>
#include <opentracing/version.h>
#include <functional>
#include <memory>
#include <string>
#include <system_error>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
class Tracer;
class SpanContext;

enum class SpanReferenceType {
  // ChildOfRef refers to a parent Span that caused *and* somehow depends
  // upon the new child Span. Often (but not always), the parent Span cannot
  // finish until the child Span does.
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

// Returns the std::error_category class used for opentracing propagation
// errors.
//
// See
//   http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-1.html
//   https://ned14.github.io/boost.outcome/md_doc_md_03-tutorial_b.html
OPENTRACING_API const std::error_category& propagation_error_category();

// `invalid_span_context_error` occurs when Tracer::Inject() is asked to operate
// on a SpanContext which it is not prepared to handle (for example, since it
// was created by a different tracer implementation).
const std::error_code invalid_span_context_error(1,
                                                 propagation_error_category());

// `invalid_carrier_error` occurs when Tracer::Inject() or Tracer::Extract()
// implementations expect a different type of `carrier` than they are given.
const std::error_code invalid_carrier_error(2, propagation_error_category());

// `span_context_corrupted_error` occurs when the `carrier` passed to
// Tracer::Extract() is of the expected type but is corrupted.
const std::error_code span_context_corrupted_error(
    3, propagation_error_category());

// `key_not_found_error` occurs when TextMapReader::LookupKey fails to find
// an entry for the provided key.
const std::error_code key_not_found_error(4, propagation_error_category());

// `lookup_key_not_supported_error` occurs when TextMapReader::LookupKey is
// not supported for the provided key.
const std::error_code lookup_key_not_supported_error(
    5, propagation_error_category());

// TextMapReader is the Extract() carrier for the TextMap builtin format. With
// it, the caller can decode a SpanContext from entries in a propagated map of
// Unicode strings.
//
// See the HTTPHeaders examples.
class TextMapReader {
 public:
  virtual ~TextMapReader() = default;

  // LookupKey returns the value for the specified `key` if available. If no
  // such key is present, it returns `key_not_found_error`.
  //
  // TextMapReaders are not required to implement this method. If not supported,
  // the function returns `lookup_key_not_supported_error`.
  //
  // Tracers may use this as an alternative to `ForeachKey` as a faster way to
  // extract span context.
  virtual expected<string_view> LookupKey(string_view /*key*/) const {
    return make_unexpected(lookup_key_not_supported_error);
  }

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
  virtual expected<void> ForeachKey(
      std::function<expected<void>(string_view key, string_view value)> f)
      const = 0;
};

// TextMapWriter is the Inject() carrier for the TextMap builtin format. With
// it, the caller can encode a SpanContext for propagation as entries in a map
// of unicode strings.
//
// See the HTTPHeaders examples.
class TextMapWriter {
 public:
  virtual ~TextMapWriter() = default;

  // Set a key:value pair to the carrier. Multiple calls to Set() for the
  // same key leads to undefined behavior.
  //
  // NOTE: The backing store for the TextMapWriter may contain data unrelated
  // to SpanContext. As such, Inject() and Extract() implementations that
  // call the TextMapWriter and TextMapReader interfaces must agree on a
  // prefix or other convention to distinguish their own key:value pairs.
  virtual expected<void> Set(string_view key, string_view value) const = 0;
};

// HTTPHeadersReader is the Extract() carrier for the HttpHeaders builtin
// format. With it, the caller can decode a SpanContext from entries in HTTP
// request headers.
//
// For example, Extract():
//
//   const Tracer& tracer = /* some tracer */
//   const HTTPHeadersReader& carrier_reader = /* some carrier */
//   auto span_context_maybe = tracer.Extract(carrier_reader);
//   if (!span_context_maybe) {
//     throw std::runtime_error(span_context_maybe.error().message());
//   }
//   auto span = tracer.StartSpan("op",
//                                { ChildOf(span_context_maybe->get()) });
class HTTPHeadersReader : public TextMapReader {};

// HTTPHeadersWriter is the Inject() carrier for the TextMap builtin format.
// With it, the caller can encode a SpanContext for propagation as entries in
// http request headers
//
// For example, Inject():
//
//   const HTTPHeadersWriter& carrier_writer = /* some carrier */
//   auto was_successful = span.tracer().Inject(span,
//                                              carrier_writer);
//   if (!was_successful) {
//     throw std::runtime_error(was_successful.error().message());
//   }
class HTTPHeadersWriter : public TextMapWriter {};

// CustomCarrierReader is the Extract() carrier for a custom format. With it,
// the caller can decode a SpanContext from entries in a custom protocol.
class CustomCarrierReader {
 public:
  virtual ~CustomCarrierReader() = default;

  // Extract is expected to specialize on the tracer implementation so as to
  // most efficiently decode its context.
  virtual expected<std::unique_ptr<SpanContext>> Extract(
      const Tracer& tracer) const = 0;
};

// CustomCarrierWriter is the Inject() carrier for a custom format.  With it,
// the caller can encode a SpanContext for propagation as entries in a custom
// protocol.
class CustomCarrierWriter {
 public:
  virtual ~CustomCarrierWriter() = default;

  // Inject is expected to specialize on the tracer implementation so as to most
  // efficiently encode its context.
  virtual expected<void> Inject(const Tracer& tracer,
                                const SpanContext& sc) const = 0;
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_PROPAGATION_H
