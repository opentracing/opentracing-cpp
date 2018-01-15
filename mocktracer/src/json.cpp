#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/nlohmann/json.hpp>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
template <>
struct adl_serializer<SpanContextData> {
  static void to_json(json& j, const SpanContextData& span_context_data) {
    j["trace_id"] = span_context_data.trace_id;
    j["span_id"] = span_context_data.span_id;
    j["baggage"] = span_context_data.baggage;
  }

  static void from_json(const json& j, SpanContextData& span_context_data) {
  }
};

template<>
struct adl_serializer<SpanReferenceType> {
  static void to_json(json& j, const SpanReferenceType& reference_type) {
    if (reference_type == SpanReferenceType::ChildOfRef) {
      j = "CHILD_OF";
    } else {
      j = "FOLLOWS_FROM";
    }
  }

  static void from_json(const json& j, SpanReferenceType& reference_type) {
  }
};

template<>
struct adl_serializer<SpanReferenceData> {
  static void to_json(json& j, const SpanReferenceData& span_reference_data) {
    j["reference_type"] = span_reference_data.reference_type;
    j["trace_id"] = span_reference_data.trace_id;
    j["span_id"] = span_reference_data.span_id;
  }

  static void from_json(const json& j, SpanReferenceData& span_reference_data) {
  }
};

template <class Rep, class Period>
struct adl_serializer<std::chrono::duration<Rep, Period>> {
  static void to_json(json& j,
                      const std::chrono::duration<Rep, Period>& duration) {
    j = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  }

  static void from_json(const json& j,
                   std::chrono::duration<Rep, Period>& duration) {}
};

template <>
struct adl_serializer<SystemTime> {
  static void to_json(json& j, const SystemTime& timestamp) {
    j = timestamp.time_since_epoch();
  }

  static void from_json(const json& j, SystemTime& timestamp) {
  }
};

static void ToJson(json& j, const Value& value);

namespace {
struct JsonValueVisitor {
  json& j;

  template <class T>
  void operator()(const T& value) {
    j = value;
  }

  void operator()(const Values& values) {
    std::vector<json> json_values;
    json_values.reserve(values.size());
    for (auto& value : values) {
      json json_value;
      ToJson(json_value, value);
      json_values.emplace_back(std::move(json_value));
    }
    j = std::move(json_values);
  }

  void operator()(const Dictionary& dictionary) {
    for (auto& key_value : dictionary) {
      json json_value;
      ToJson(json_value, key_value.second);
      j[key_value.first] = std::move(json_value);
    }
  }
};
} // namespace

static void ToJson(json& j, const Value& value) {
  JsonValueVisitor value_visitor{j};
  apply_visitor(value_visitor, value);
}

template <>
struct adl_serializer<Value> {
  static void to_json(json& j, const Value& value) {
    ToJson(j, value);
  }

  static void from_json(const json& j, Value& value) {
  }
};

template <>
struct adl_serializer<LogRecord> {
  static void to_json(json& j, const LogRecord& log_record) {
    j["timetamp"] = log_record.timestamp;
    std::vector<json> json_fields;
    json_fields.reserve(log_record.fields.size());
    for (const auto& field : log_record.fields) {
      json json_field;
      json_field["key"] = field.first;
      json_field["value"] = field.second;
      json_fields.emplace_back(std::move(json_field));
    }
    j["fields"] = json_fields;
  }

  static void from_json(const json& j, LogRecord& log_record) {
  }
};

template<>
struct adl_serializer<SpanData> {
  static void to_json(json& j, const SpanData& span_data) {
    j["span_context"] = span_data.span_context;
    j["references"] = span_data.references;
    j["operation_name"] = span_data.operation_name;
    j["start_timestamp"] = span_data.start_timestamp;
    j["duration"] = span_data.duration;
    j["tags"] = span_data.tags;
    j["logs"] = span_data.logs;
  }

  static void from_json(const json& j, SpanData& span_data) {
  }
};

std::string ToJson(const std::vector<SpanData>& spans) {
  json j = spans;
  return j.dump();
}

std::vector<SpanData> FromJson(string_view json) {
  return {};
}

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
