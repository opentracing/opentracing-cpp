# OpenTracing API for C++
C++ implementation of the OpenTracing API http://opentracing.io

[![Join the chat at https://gitter.im/opentracing/opentracing-cpp](https://badges.gitter.im/opentracing/opentracing-cpp.svg)](https://gitter.im/opentracing/opentracing-cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Required Reading

In order to understand the C++ platform API, one must first be familiar with the
[OpenTracing project](http://opentracing.io) and
[terminology](http://opentracing.io/documentation/pages/spec) more generally. 

## Compile and install

#### Linux/MacOS

```bash
mkdir .build
cd .build
cmake ..
make
sudo make install
```

To test:

```bash
make test
```

#### Windows

```bash
mkdir .build
cd .build
cmake -G "Visual Studio 15 2017 Win64" ..
```
To build the targets in debug mode
```bash
MSBuild.exe opentracing-cpp.sln /p:Configuration=Debug /Target=Build
```
To build the targets in release mode
```bash
MSBuild.exe opentracing-cpp.sln /p:Configuration=Release /Target=Build
```

To test:
Run the below command to run the tests with the debug targets
```bash
ctest -C Debug
```
Run the below command to run the tests with the release targets
```bash
ctest -C Release
```

## API overview for those adding instrumentation

Everyday consumers of this `opentracing` package really only need to worry
about a couple of key abstractions: the `StartSpan` function, the `Span`
interface, and binding a `Tracer` at `main()`-time. Here are code snippets
demonstrating some important use cases.

#### Singleton initialization

The simplest starting point is `opentracing/tracer.h`. As early as possible, call

```cpp
    #include <opentracing/tracer.h>
    #include <some_tracing_impl.h>
    
    int main() {
      Tracer::InitGlobal(make_some_tracing_impl());
      ...
    }
```

#### Non-Singleton initialization

If you prefer direct control to singletons, manage ownership of the
`opentracing::Tracer` implementation explicitly.

#### Starting an empty trace by creating a "root span"

It's always possible to create a "root" `Span` with no parent or other causal
reference.

```cpp
    void xyz() {
        ...
        auto tracer = /* Some Tracer */
        auto span = tracer->StartSpan("operation_name");
        if (!span)
          // Error creating span.
          ...
        span->Finish();
        ...
    }
```

#### Creating a (child) Span given an existing (parent) Span

```cpp
    void xyz(const opentracing::Span& parent_span, ...) {
        ...
        auto tracer = /* Some Tracer */
        auto span = tracer->StartSpan(
            "operation_name",
            {opentracing::ChildOf(&parent_span.context())});
        if (!span)
          // Error creating span.
          ...
        span->Finish();
        ...
    }
```

#### Inject Span context into a TextMapWriter

```cpp
    struct CustomCarrierWriter : opentracing::TextMapWriter {
      explicit CustomCarrierWriter(
          std::unordered_map<std::string, std::string>& data_)
          : data{data_} {}
    
      opentracing::expected<void> Set(
          opentracing::string_view key,
          opentracing::string_view value) const override {
        // OpenTracing uses opentracing::expected for error handling. This closely
        // follows the expected proposal for the C++ Standard Library. See
        //    http://open-std.org/JTC1/SC22/WG21/docs/papers/2017/p0323r3.pdf
        // for more background.
        opentracing::expected<void> result;
    
        auto was_successful = data.emplace(key, value);
        if (was_successful.second) {
          // Use a default constructed opentracing::expected<void> to indicate
          // success.
          return result;
        } else {
          // `key` clashes with existing data, so the span context can't be encoded
          // successfully; set opentracing::expected<void> to an std::error_code.
          return opentracing::make_unexpected(
              std::make_error_code(std::errc::not_supported));
        }
      }
    
      std::unordered_map<std::string, std::string>& data;
    };

    ...
    
    std::unordered_map<std::string, std::string> data;
    CustomCarrierWriter carrier{data};
    auto was_successful = tracer->Inject(span->context(), carrier);
    if (!was_successful) {
      // Injection failed, log an error message.
      std::cerr << was_successful.error().message() << "\n";
    }
```

#### Extract Span context from a TextMapReader

```cpp
    struct CustomCarrierReader : opentracing::TextMapReader {
      explicit CustomCarrierReader(
          const std::unordered_map<std::string, std::string>& data_)
          : data{data_} {}
    
      using F = std::function<opentracing::expected<void>(
          opentracing::string_view, opentracing::string_view)>;
    
      opentracing::expected<void> ForeachKey(F f) const override {
        // Iterate through all key-value pairs, the tracer will use the relevant keys
        // to extract a span context.
        for (auto& key_value : data) {
          auto was_successful = f(key_value.first, key_value.second);
          if (!was_successful) {
            // If the callback returns and unexpected value, bail out of the loop.
            return was_successful;
          }
        }
    
        // Indicate successful iteration.
        return {};
      }
    
      // Optional, define TextMapReader::LookupKey to allow for faster extraction.
      opentracing::expected<opentracing::string_view> LookupKey(
          opentracing::string_view key) const override {
        auto iter = data.find(key);
        if (iter != data.end()) {
          return opentracing::make_unexpected(opentracing::key_not_found_error);
        }
        return opentracing::string_view{iter->second};
      }
    
      const std::unordered_map<std::string, std::string>& data;
    };
    
    ...

    CustomCarrierReader carrier{data};
    auto span_context_maybe = tracer->Extract(carrier);
    if (!span_context_maybe) {
      // Extraction failed, log an error message.
      std::cerr << span_context_maybe.error().message() << "\n";
    }
  
    // If `carrier` contained a span context, `span_context` will point to a
    // representation of it; otherwise, if no span context existed, `span_context`
    // will be nullptr;
    std::unique_ptr<opentracing::SpanContext> span_context =
        std::move(*span_context_maybe);
```

## API compatibility

For the time being, "mild" backwards-incompatible changes may be made without
changing the major version number. As OpenTracing and `opentracing-cpp` mature,
backwards compatibility will become more of a priority.

## C/C++98

This library requires C++11 or later. But if you're interested in a C or C++98
API contact us on [gitter](https://gitter.im/opentracing/opentracing-cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge).
We're open to supporting additional APIs in a separate repository if there are
people willing to maintain it.

## License

By contributing to opentracing.cpp, you agree that your contributions will be licensed under its [Apache 2.0 License](./LICENSE).
