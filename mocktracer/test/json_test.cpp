#include <opentracing/mocktracer/in_memory_recorder.h>
#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/tracer.h>
#include <algorithm>
#include <cctype>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>
using namespace opentracing;
using namespace mocktracer;

TEST_CASE("json") {
  auto recorder = new InMemoryRecorder{};
  MockTracerOptions tracer_options;
  tracer_options.recorder.reset(recorder);
  auto tracer = std::shared_ptr<opentracing::Tracer>{
      new MockTracer{std::move(tracer_options)}};

  SpanContextData span_context_data;
  span_context_data.trace_id = 123;
  span_context_data.span_id = 456;
  span_context_data.baggage = {{"b1", "v1"}, {"b2", "v2"}};

  SpanData span_data;
  span_data.span_context = span_context_data;
  span_data.references = {{SpanReferenceType::ChildOfRef, 123, 457}};
  span_data.operation_name = "o1";
  span_data.start_timestamp =
      std::chrono::system_clock::time_point{} + std::chrono::hours{51};
  span_data.duration = std::chrono::microseconds{92};
  span_data.tags = {{"t1", 123}, {"t2", "cat"}};
  span_data.logs = {{span_data.start_timestamp, {{"l1", 1}, {"l2", 1.5}}}};
  std::ostringstream oss;
  ToJson(oss, {span_data});

  std::string expected_serialization = R"(
      [{
      	"span_context": {
      		"trace_id": "000000000000007b",
      		"span_id": "00000000000001c8",
      		"baggage": {
      			"b1": "v1",
      			"b2": "v2"
      		}
      	},
      	"references": [{
      		"reference_type": "CHILD_OF",
      		"trace_id": "000000000000007b",
      		"span_id": "00000000000001c9"
      	}],
      	"operation_name": "o1",
      	"start_timestamp": 183600000000,
      	"duration": 92,
      	"tags": {
      		"t1": 123,
      		"t2": "cat"
      	},
      	"logs": [{
      		"timestamp": 183600000000,
      		"fields": [{
      			"key": "l1",
      			"value": 1
      		}, {
      			"key": "l2",
      			"value": 1.5
      		}]
      	}]
      }])";
  expected_serialization.erase(
      std::remove_if(expected_serialization.begin(),
                     expected_serialization.end(),
                     [](char c) { return std::isspace(c); }),
      expected_serialization.end());

  CHECK(oss.str() == expected_serialization);
}
