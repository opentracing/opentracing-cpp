#include <opentracing/mocktracer/json.h>
#include <cstdlib>
#include <iostream>
#include <opentracing/mocktracer/nlohmann/json.hpp>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
template <>
struct adl_serializer<SpanContextData> {
  static void to_json(json& j, const SpanContextData& span_context_data) {
    j["trace_id"] = std::to_string(span_context_data.trace_id);
    j["span_id"] = std::to_string(span_context_data.span_id);
    j["baggage"] = span_context_data.baggage;
  }

  static void from_json(const json& j, SpanContextData& span_context_data) {
    std::string trace_id = j.at("trace_id");
    std::string span_id = j.at("span_id");
    span_context_data.trace_id = std::strtoull(trace_id.c_str(), nullptr, 10);
    span_context_data.span_id = std::strtoull(span_id.c_str(), nullptr, 10);
    for (auto& element : json::iterator_wrapper(j.at("baggage"))) {
      span_context_data.baggage[element.key()] = element.value();
    }
  }
};

template <>
struct adl_serializer<SpanReferenceType> {
  static void to_json(json& j, const SpanReferenceType& reference_type) {
    if (reference_type == SpanReferenceType::ChildOfRef) {
      j = "CHILD_OF";
    } else {
      j = "FOLLOWS_FROM";
    }
  }

  static void from_json(const json& j, SpanReferenceType& reference_type) {
    auto reference_string = j.get<std::string>();
    if (reference_string == "CHILD_OF") {
      reference_type = SpanReferenceType::ChildOfRef;
    } else if (reference_string == "FOLLOWS_FROM") {
      reference_type = SpanReferenceType::FollowsFromRef;
    } else {
      throw std::domain_error{std::string{"invalid reference type: `"} +
                              reference_string + "`"};
    }
  }
};

template <>
struct adl_serializer<SpanReferenceData> {
  static void to_json(json& j, const SpanReferenceData& span_reference_data) {
    j["reference_type"] = span_reference_data.reference_type;
    j["trace_id"] = span_reference_data.trace_id;
    j["span_id"] = span_reference_data.span_id;
  }

  static void from_json(const json& j, SpanReferenceData& span_reference_data) {
    span_reference_data.reference_type = j.at("reference_type");
    span_reference_data.trace_id = j.at("trace_id");
    span_reference_data.span_id = j.at("span_id");
  }
};

template <class Rep, class Period>
struct adl_serializer<std::chrono::duration<Rep, Period>> {
  static void to_json(json& j,
                      const std::chrono::duration<Rep, Period>& duration) {
    j = std::to_string(duration.count());
  }

  static void from_json(const json& j,
                        std::chrono::duration<Rep, Period>& duration) {
    const std::string ticks_str = j;
    std::istringstream iss{ticks_str};
    Rep ticks;
    iss >> ticks;
    duration = std::chrono::duration<Rep, Period>{ticks};
  }
};

template <>
struct adl_serializer<SystemTime> {
  static void to_json(json& j, const SystemTime& timestamp) {
    j = timestamp.time_since_epoch();
  }

  static void from_json(const json& j, SystemTime& timestamp) {
    SystemClock::duration time_since_epoch = j;
    timestamp = SystemTime{time_since_epoch};
  }
};

static void ToJson(json& j, const Value& value);

namespace {
struct JsonValueVisitor {
  json& j;

  void operator()(std::nullptr_t) {
    j["type"] = "nullptr";
    j["value"] = nullptr;
  }

  void operator()(bool value) {
    j["type"] = "bool";
    j["value"] = value;
  }

  void operator()(double value) {
    j["type"] = "double";
    j["value"] = value;
  }

  void operator()(int64_t value) {
    j["type"] = "int64";
    j["value"] = value;
  }

  void operator()(uint64_t value) {
    j["type"] = "uint64";
    j["value"] = value;
  }

  void operator()(const std::string& value) {
    j["type"] = "string";
    j["value"] = value;
  }

  void operator()(const char* s) { this->operator()(std::string{s}); }

  void operator()(const Values& values) {
    j["type"] = "array";
    std::vector<json> json_values;
    json_values.reserve(values.size());
    for (auto& value : values) {
      json json_value;
      ToJson(json_value, value);
      json_values.emplace_back(std::move(json_value));
    }
    j["value"] = std::move(json_values);
  }

  void operator()(const Dictionary& dictionary) {
    j["type"] = "object";
    json json_values;
    for (auto& key_value : dictionary) {
      json json_value;
      ToJson(json_value, key_value.second);
      json_values[key_value.first] = std::move(json_value);
    }
    j["value"] = json_values;
  }
};
}  // namespace

static void ToJson(json& j, const Value& value) {
  JsonValueVisitor value_visitor{j};
  apply_visitor(value_visitor, value);
}

static void FromJson(const json& j, Value& value) {
  std::string type = j["type"];
  if (type == "nullptr") {
    value = nullptr;
  } else if (type == "string") {
    std::string s = j["value"];
    value = s;
  } else if (type == "bool") {
    value = bool{j["value"]};
  } else if (type == "int64") {
    value = int64_t{j["value"]};
  } else if (type == "uint64") {
    value = uint64_t{j["value"]};
  } else if (type == "double") {
    value = double{j["value"]};
  } else if (type == "array") {
    json json_values = j["value"];
    Values values;
    values.reserve(json_values.size());
    for (auto& json_value : json_values) {
      Value value_item;
      FromJson(json_value, value_item);
      values.emplace_back(std::move(value_item));
    }
    value = std::move(values);
  } else if (type == "object") {
    json json_values = j["value"];
    Dictionary values;
    for (auto& element : json::iterator_wrapper(json_values)) {
      Value value_item;
      FromJson(element.value(), value_item);
      values[element.key()] = std::move(value_item);
    }
    value = std::move(values);
  } else {
    throw std::domain_error{std::string{"unexpected json type: "} + type};
  }
}

template <>
struct adl_serializer<Value> {
  static void to_json(json& j, const Value& value) { ToJson(j, value); }

  static void from_json(const json& j, Value& value) { FromJson(j, value); }
};

template <>
struct adl_serializer<LogRecord> {
  static void to_json(json& j, const LogRecord& log_record) {
    j["timestamp"] = log_record.timestamp;
    std::vector<json> json_fields;
    json_fields.reserve(log_record.fields.size());
    for (auto& field : log_record.fields) {
      json json_field;
      json_field["key"] = field.first;
      json_field["value"] = field.second;
      json_fields.emplace_back(std::move(json_field));
    }
    j["fields"] = std::move(json_fields);
  }

  static void from_json(const json& j, LogRecord& log_record) {
    log_record.timestamp = j.at("timestamp");
    json json_fields = j.at("fields");
    log_record.fields.reserve(json_fields.size());
    for (auto& json_field : json_fields) {
      std::string key = json_field.at("key");
      Value value = json_field.at("value");
      log_record.fields.emplace_back(std::move(key), std::move(value));
    }
  }
};

template <>
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
    span_data.span_context = j.at("span_context");
    json json_references = j.at("references");
    span_data.references.reserve(json_references.size());
    for (auto& json_reference : json_references) {
      SpanReferenceData reference = json_reference;
      span_data.references.emplace_back(std::move(reference));
    }
    span_data.operation_name = j.at("operation_name");
    span_data.start_timestamp = j.at("start_timestamp");
    span_data.duration = j.at("duration");
    for (auto& json_tag : json::iterator_wrapper(j.at("tags"))) {
      span_data.tags[json_tag.key()] = json_tag.value();
    }
    json json_logs = j.at("logs");
    span_data.logs.reserve(json_logs.size());
    for (auto& json_log : json_logs) {
      LogRecord log_record = json_log;
      span_data.logs.emplace_back(std::move(log_record));
    }
  }
};

std::string ToJson(const std::vector<SpanData>& spans) {
  json j = spans;
  return j.dump();
}

std::vector<SpanData> FromJson(string_view s) {
  json j = json::parse(s.begin(), s.end());
  std::vector<SpanData> result = j;
  return result;
}

std::ostream& operator<<(std::ostream& out,
                         const SpanContextData& span_context_data) {
  json j = span_context_data;
  out << j.dump();
  return out;
}

std::ostream& operator<<(std::ostream& out, const SpanData& span_data) {
  json j = span_data;
  out << j.dump();
  return out;
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
