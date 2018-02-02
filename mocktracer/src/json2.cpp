#include <opentracing/mocktracer/recorder.h>
#include <opentracing/string_view.h>
#include <iomanip>
#include <sstream>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// The implementation is based off of this answer from StackOverflow:
// https://stackoverflow.com/a/33799784
static void WriteEscapedString(std::ostream& writer,
                               opentracing::string_view s) {
  writer << '"';
  for (char c : s) {
    switch (c) {
      case '"':
        writer << R"(\")";
        break;
      case '\\':
        writer << R"(\\)";
        break;
      case '\b':
        writer << R"(\b)";
        break;
      case '\n':
        writer << R"(\n)";
        break;
      case '\r':
        writer << R"(\r)";
        break;
      case '\t':
        writer << R"(\t)";
        break;
      default:
        if ('\x00' <= c && c <= '\x1f') {
          writer << R"(\u)";
          writer << std::hex << std::setw(4) << std::setfill('0')
                 << static_cast<int>(c);
        } else {
          writer << c;
        }
    }
  }
  writer << '"';
}

static void ToJson(std::ostream& writer,
                   const SpanContextData& span_context_data) {
  writer << '{';
  writer << R"("trace_id":)" << span_context_data.trace_id << ',';
  writer << R"("span_id":)" << span_context_data.span_id << ',';
  writer << R"("baggage":{)";
  auto num_baggage = span_context_data.baggage.size();
  size_t baggage_index = 0;
  for (auto& baggage_item : span_context_data.baggage) {
    WriteEscapedString(writer, baggage_item.first);
    writer << ':';
    WriteEscapedString(writer, baggage_item.second);
    if (++baggage_index < num_baggage) {
      writer << ',';
    }
  }
  writer << '}';
}

static void ToJson(std::ostream& writer,
                   const SpanReferenceData& span_reference_data) {
  writer << '{';
  writer << R"("reference_type":)";
  if (span_reference_data.reference_type == SpanReferenceType::ChildOfRef) {
    writer << R"("CHILD_OF")";
  } else {
    writer << R"("FOLLOWS_FROM")";
  }
  writer << ',';

  writer << R"("trace_id":)" << span_reference_data.trace_id << ',';
  writer << R"("span_id":)" << span_reference_data.span_id;

  writer << '}';
}

static void ToJson(std::ostream& writer, const Value& value);

namespace {
struct ValueVisitor {
  std::ostream& writer;

  void operator()(bool value) {
    if (value) {
      writer << "true";
    } else {
      writer << "false";
    }
  }

  void operator()(double value) {
    if (std::isnan(value)) {
      writer << R"("NaN")";
    } else if (std::isinf(value)) {
      if (std::signbit(value)) {
        writer << R"("-Inf")";
      } else {
        writer << R"("+Inf")";
      }
    } else {
      writer << value;
    }
  }

  void operator()(int64_t value) { writer << value; }

  void operator()(uint64_t value) { writer << value; }

  void operator()(const std::string& s) { WriteEscapedString(writer, s); }

  void operator()(std::nullptr_t) { writer << "null"; }

  void operator()(const char* s) { WriteEscapedString(writer, s); }

  void operator()(const Values& values) {
    writer << '[';
    size_t i = 0;
    for (const auto& value : values) {
      ToJson(writer, value);
      if (++i < values.size()) {
        writer << ',';
      }
    }
    writer << ']';
  }

  void operator()(const Dictionary& dictionary) {
    writer << '{';
    size_t i = 0;
    for (const auto& key_value : dictionary) {
      WriteEscapedString(writer, key_value.first);
      writer << ':';
      ToJson(writer, key_value.second);
      if (++i < dictionary.size()) {
        writer << ',';
      }
    }
    writer << '}';
  }
};
}  // anonymous namespace

void ToJson(std::ostream& writer, const Value& value) {
  ValueVisitor value_visitor{writer};
  apply_visitor(value_visitor, value);
}

template <class Rep, class Period>
static void ToJson(std::ostream& writer,
                   const std::chrono::duration<Rep, Period>& duration) {
  auto count =
      std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  writer << count;
}

static void ToJson(std::ostream& writer, const LogRecord& log_record) {
  writer << '{';
  writer << R"("timestamp":)";
  ToJson(writer, log_record.timestamp.time_since_epoch());
  writer << ',';
  writer << R"("fields":)";
  writer << '[';
  auto num_fields = log_record.fields.size();
  size_t field_index = 0;
  for (auto& field : log_record.fields) {
    writer << '{';
    writer << R"("key":)";
    WriteEscapedString(writer, field.first);
    writer << ',';
    writer << R"("value":)";
    ToJson(writer, field.second);
    writer << '}';
    if (++field_index < num_fields) {
      writer << ',';
    }
  }
  writer << ']';
  writer << '}';
}

static void ToJson(std::ostream& writer, const SpanData& span_data) {}

static void ToJson(std::ostream& writer,
                   const std::vector<SpanData>& span_data) {}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
