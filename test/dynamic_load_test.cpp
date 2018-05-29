#include <opentracing/dynamic_load.h>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <random>
using namespace opentracing;

#define CATCH_CONFIG_RUNNER
#include <opentracing/catch2/catch.hpp>

static std::string mocktracer_library;

TEST_CASE("dynamic_load") {
  std::string error_message;

  SECTION(
      "Dynamically loading a library that doesn't exists gives a proper error "
      "code.") {
    auto handle_maybe = DynamicallyLoadTracingLibrary("abc/123", error_message);
    REQUIRE(!handle_maybe);
    CHECK(handle_maybe.error() == dynamic_load_failure_error);
  }

  error_message.clear();
  auto handle_maybe =
      DynamicallyLoadTracingLibrary(mocktracer_library.c_str(), error_message);
  REQUIRE(handle_maybe);
  REQUIRE(error_message.empty());

  SECTION("Creating a tracer from invalid json gives an error.") {
    auto tracer_maybe =
        handle_maybe->tracer_factory().MakeTracer("abc 123", error_message);
    REQUIRE(!tracer_maybe);
  }

  SECTION("Creating a tracer with an invalid output_file gives an error.") {
    auto tracer_maybe = handle_maybe->tracer_factory().MakeTracer(
        R"({"output_file": ""})", error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(tracer_maybe.error() == invalid_configuration_error);
  }

  SECTION(
      "We can create spans from an OpenTracing library dynamically loaded.") {
    std::string span_filename{"spans."};
    const auto random_id = std::random_device{}();
    span_filename.append(std::to_string(random_id));
    std::string configuration = R"({ "output_file": ")";
    configuration.append(span_filename);
    configuration.append(R"(" })");

    {
      auto tracer_maybe = handle_maybe->tracer_factory().MakeTracer(
          configuration.c_str(), error_message);
      REQUIRE(tracer_maybe);
      auto tracer = *tracer_maybe;
      tracer->StartSpan("abc");
      tracer->Close();
    }

    std::ifstream istream{span_filename};
    REQUIRE(istream.good());
    std::string spans_json{std::istreambuf_iterator<char>{istream},
                           std::istreambuf_iterator<char>{}};
    istream.close();
    std::remove(span_filename.c_str());
    CHECK(!spans_json.empty());
  }
}

int main(int argc, char* argv[]) {
  Catch::Session session;

  using namespace Catch::clara;
  auto cli = session.cli() | Opt(mocktracer_library,
                                 "mocktracer_library")["--mocktracer_library"];

  session.cli(cli);
  int rcode = session.applyCommandLine(argc, argv);
  if (rcode != 0) {
    return rcode;
  }

  if (mocktracer_library.empty()) {
    std::cerr << "Must provide mocktracer_library!\n";
    return -1;
  }

  return session.run();
}
