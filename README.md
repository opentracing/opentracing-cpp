# OpenTracing API for C++
C++ implementation of the OpenTracing API http://opentracing.io

[![Join the chat at https://gitter.im/opentracing/opentracing-cpp](https://badges.gitter.im/opentracing/opentracing-cpp.svg)](https://gitter.im/opentracing/opentracing-cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Required Reading

In order to understand the C++ platform API, one must first be familiar with the
[OpenTracing project](http://opentracing.io) and
[terminology](http://opentracing.io/spec/) more generally. 

## Compile and install

```
mkdir .build
cd .build
cmake ..
make
sudo make install
```

To test:

```
make test
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

##### Non-Singleton initialization

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

## API compatibility

For the time being, "mild" backwards-incompatible changes may be made without
changing the major version number. As OpenTracing and `opentracing-cpp` mature,
backwards compatibility will become more of a priority.
