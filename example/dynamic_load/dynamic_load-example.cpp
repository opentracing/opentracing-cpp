// Demonstrates how to load a tracer library in at runtime and how to use it
// to construct spans. To run it using the mocktracer, invoke with
//
//     TRACER_CONFIG=`mktemp`
//     echo '{ "output_file": "/dev/stdout" }' > $TRACER_CONFIG
//     dynamic_load-example /path/to/libopentracing_mocktracer.so $TRACER_CONFIG

#include <opentracing/dynamic_load.h>
#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: <tracer_library> <tracer_config_file>\n";
    return -1;
  }

  // Load the tracer library.
  std::string error_message;
  auto handle_maybe =
      opentracing::DynamicallyLoadTracingLibrary(argv[1], error_message);
  if (!handle_maybe) {
    std::cerr << "Failed to load tracer library " << error_message << "\n";
    return -1;
  }

  // Read in the tracer's configuration.
  std::ifstream istream{argv[2]};
  if (!istream.good()) {
    std::cerr << "Failed to open tracer config file " << argv[2] << ": "
              << std::strerror(errno) << "\n";
    return -1;
  }
  std::string tracer_config{std::istreambuf_iterator<char>{istream},
                            std::istreambuf_iterator<char>{}};

  // Construct a tracer.
  auto& tracer_factory = handle_maybe->tracer_factory();
  auto tracer_maybe =
      tracer_factory.MakeTracer(tracer_config.c_str(), error_message);
  if (!tracer_maybe) {
    std::cerr << "Failed to create tracer " << error_message << "\n";
    return -1;
  }
  auto& tracer = *tracer_maybe;

  // Use the tracer to create some spans.
  {
    auto span_a = tracer->StartSpan("A");
    assert(span_a != nullptr);
    span_a->SetTag("abc", 123);
    auto span_b =
        tracer->StartSpan("B", {opentracing::ChildOf(&span_a->context())});
    assert(span_b != nullptr);
    span_b->SetTag("xyz", 987);
  }

  tracer->Close();
  return 0;
}
