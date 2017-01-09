# opentracing-cpp

This file describes how clients can instrument their applications with the `opentracing-cpp`
API. It assumes you have finished the required reading outlined [here](../README.md).

### Outline

The `opentracing-cpp` interface uses the [Curiously Repeating Template Pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern) (CRTP) to define a set of
compile time, polymorphic interfaces to `OpenTracing` implementations.

In order to trace their systems, clients must:
    * Select their `opentracing-cpp` implementation
    * Install the selected `Tracer` in their applications in main
    * Use the API's Tracer interface to instrument their applications requests


### Select the implementation

`opentracing-cpp` only defines the interfaces. In order to make any use of the
API, you will need to install a concrete `Tracer` implementation.

To do so, it is best practice to first create an reusable header you can share
throughout your organization that declares the implementation you will be using.

This header should provide a few typedefs that make it easy to swap implementations
later.

```
// acme_tracing.h
#ifndef INCLUDED_ACME_TRACING
#define INCLUDED_ACME_TRACING

// Include the interface templates
#include <opentracing/tracer.h>
#include <opentracing/span.h>
#include <opentracing/spancontext.h>
#include <opentracing/spanoptions.h>

// Include the chosen implementation
#include <opentracing/noop.h>

namespace acme {
    typedef opentracing::NoopTracer TracerImpl;

    typedef opentracing::GenericTracer<
                opentracing::NoopTracer,
                opentracing::NoopSpan,
                opentracing::NoopOptions,
                opentracing::NoopContext,
                opentracing::NoopAdapter> Tracer;
    // Tracer is required for O(1) implementation swaps

    typedef Tracer::SpanContext SpanContext;
    typedef Tracer::SpanOptions SpanOptions;
    typedef Tracer::Span        Span;

    typedef Tracer::SpanContextGuard SpanContextGuard;
    typedef Tracer::SpanOptionsGuard SpanOptions;
    typedef Tracer::SpanGuard        SpanGuard;
    // Exposing the 'Span' typedefs is done for convenience
}
#endif
```

By creating the `Tracer` typedef, we can now rely on the the GenericTracer interface,
but avoid knowing any of the explicit implementation details in client code.

The `Tracer` exposes a number of public typedefs which make it easier to create our
types:

    * `Tracer::Span`             - Interface to Span objects
    * `Tracer::SpanContext`      - Interface to SpanContext objects
    * `Tracer::SpanOptions`      - Interface to SpanOptions objects
    * `Tracer::SpanGuard`        - RAII Wrappers for Spans
    * `Tracer::SpanOptionsGuard` - RAII Wrappers for SpanOptions
    * `Tracer::SpanContextGuard` - RAII Wrappers for SpanContexts


Our theoretical "ACME Inc." organization can use to instrument their applications.
The `Tracer` typedef is not standardized, but it is recommended. The rest of the
documentation will assume that `Tracer` was the chosen name for the Tracer type.

If clients ever require that they swap `OpenTracing` implementations, they
will need to update only this header file and rebuild applications.

### Installing the Tracer

Now that you have selected your chosen implementation, your applications can rely
on your global `opentracing-cpp` configuration header to set up the `Tracer`.
The `Tracer` implementation is installed explicitly as singleton in main. This must
be done before any other parts of the API can be used:

```
// acmetask.m.cpp
#include <acme_tracing.h>

int main(int argc, const char * argv[])
{
    acme::TracerImpl tracerImpl;
    acme::Tracer::install(&tracerImpl);

    // ...

    return 0;
}

```

### Creating Spans

Once the `Tracer` is installed, we can begin to instrument the application.

Say we have a service that acts as a proxy to a number of other HTTP servers.
We may have a `getAccount` handler that reaches out to the account service.

We'll first need to start the span:

```
// requesthandler.m.cpp
#include <acme_tracing.h>

int getAccount(Repsonse * resp, const Request& req)
{
    static Tracer * const s_tracer = Tracer::instance();

    acme::Tracer::SpanGuard span(s_tracer->start("get_account"));

    if (!span.get())
    { /* log error */
        ;
    }

    // When 'span' goes out of scope, the destructor will call s_tracer->cleanup().
    // This will automatically 'finish()' the span and return resources to the
    // Tracer.

    return 0;
}
```

### Defining carriers

For this trace to be distributed across all of our organizations tasks, we need
a way to `inject` and `extract` the span into our RPC carriers.

For our above example, when we make our request to the backend HTTP service,
we'll need a way to inject the details of the context into our HTTP request.

```
// For this example, we'll define our HttpWriter inline with our request.
// In practice, you'll want the Writers/Readers you use to be consistent
// across your organization, so putting them into a library would be
// ideal.

#include <opentracing/carriers.h>

class HttpWriter : public GenericBinaryWriter<HttpWriter>
{
  public:
    HttpWriter(HttpRequest * req): m_req(req){}

    int injectImp(const void* blob, const_size len)
    {
        std::string blob(static_cast<char*>(blob), len);
        req->addHeader("x-acme-tracing-blob", blob);
        return 0;
    }

  private:
    HttpRequest * m_req;
};

int getAccount(Repsonse * resp, const Request& req)
{
    static Tracer * const s_tracer = Tracer::instance();

    acme::SpanGuard span(s_tracer->start("get_account"));

    if(!span.get()){ /* log error */; }

    HttpRequest httpRequest;
    HttpWriter writer(&httpRequest);
    s_tracer->inject(&writer, span->context());

    // Business logic...
    HttpResponse httpResponse;
    sendHttpRequest(&httpResponse, httpRequest);

    return 0;
}
```

On the HTTP servers end, we'll want to create another span as a child:

```
#include <opentracing/carriers.h>

struct HttpReader: public GenericBinaryReader<HttpReader>
{
  public:
    int extractImp(void* const buf, size_t* const written, const size_t len)
    {
        std::string header = m_req.getHeader("x-acme-tracing-blob");

        if (header.length() >= len)
        {
            return 1;
        }

        std::memcpy(buf, header.data(), header.size());
        *written = header.size();
        return 0;
    }

  private:
    const HttpRequest& m_req;
};

int httpGetAccount(const HttpRequest& httpRequest)
{
    static Tracer* const s_tracer = Tracer::instance();

    acme::SpanContextGuard context(s_tracer->extract(HttpReader(httpRequest)));

    acme::SpanGuard span;

    if (!context.get())
    {
        span = s_tracer->start("get_account");
    }
    else
    {
        acme::SpanOptionsGuard opts(s_tracer->makeSpanOptions());

        opts->addReference(opentracing::SpanRelationship::e_childOf, *context);
        opts->setOperation("get_account");
        span = s_tracer->start(opts);
    }

    return 0;
}
```
