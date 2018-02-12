#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/recorder.h>
#include <opentracing/string_view.h>
#include <cmath>
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

static void WriteId(std::ostream& writer, uint64_t id) {
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(16) << std::hex << id;
  if (!oss.good()) {
    writer.setstate(std::ios::failbit);
    return;
  }
  writer << '"' << oss.str() << '"';
}

static void ToJson(std::ostream& writer,
                   const SpanContextData& span_context_data) {
  writer << '{';
  writer << R"("trace_id":)";
  WriteId(writer, span_context_data.trace_id);
  writer << ',';

  writer << R"("span_id":)";
  WriteId(writer, span_context_data.span_id);
  writer << ',';

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

  writer << R"("trace_id":)";
  WriteId(writer, span_reference_data.trace_id);
  writer << ',';
  writer << R"("span_id":)";
  WriteId(writer, span_reference_data.span_id);

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

static void ToJson(std::ostream& writer, const SpanData& span_data) {
  writer << '{';

  writer << R"("span_context":)";
  ToJson(writer, span_data.span_context);
  writer << ',';

  writer << R"("references":)";
  writer << '[';
  auto num_references = span_data.references.size();
  size_t reference_index = 0;
  for (auto& reference : span_data.references) {
    ToJson(writer, reference);
    if (++reference_index < num_references) {
      writer << ',';
    }
  }
  writer << ']';
  writer << ',';

  writer << R"("operation_name":)";
  WriteEscapedString(writer, span_data.operation_name);
  writer << ',';

  writer << R"("start_timestamp":)";
  ToJson(writer, span_data.start_timestamp.time_since_epoch());
  writer << ',';

  writer << R"("duration":)";
  ToJson(writer, span_data.duration);
  writer << ',';

  writer << R"("tags":)";
  writer << '{';
  auto num_tags = span_data.tags.size();
  size_t tag_index = 0;
  for (auto& tag : span_data.tags) {
    WriteEscapedString(writer, tag.first);
    writer << ':';
    ToJson(writer, tag.second);
    if (++tag_index < num_tags) {
      writer << ',';
    }
  }
  writer << '}';
  writer << ',';

  writer << R"("logs":)";
  writer << '[';
  auto num_logs = span_data.logs.size();
  size_t log_index = 0;
  for (auto& log : span_data.logs) {
    ToJson(writer, log);
    if (++log_index < num_logs) {
      writer << ',';
    }
  }
  writer << ']';

  writer << '}';
}

void ToJson(std::ostream& writer, const std::vector<SpanData>& spans) {
  writer << '[';
  auto num_spans = spans.size();
  size_t span_index = 0;
  for (auto& span_data : spans) {
    ToJson(writer, span_data);
    if (++span_index < num_spans) {
      writer << ',';
    }
  }
  writer << ']';
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
