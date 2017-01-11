# opentracing-cpp

This file describes how clients can instrument their applications with the `opentracing-cpp`
API. It assumes you have finished the required reading outlined [here](../README.md).

### Outline

The `opentracing-cpp` interface uses the [Curiously Recurring Template Pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
(CRTP) to define a set of compile time, polymorphic interfaces to `OpenTracing` implementations.

In order to trace their systems, clients must:

 - Select their `opentracing-cpp` implementation
 - Install the selected `Tracer` in their applications in main
 - Use the API's Tracer interface to instrument their applications requests


### Select the implementation

`opentracing-cpp` only defines the interfaces. In order to make any use of the
API, you will need to install a concrete `Tracer` implementation. To do so, it is
best practice to first create an reusable header you can share throughout your
organization that declares the implementation you will be using.

This header should provide a few typedefs that make it easy to swap implementations
later.

```
// acme_tracing.h
#ifndef INCLUDED_ACME_TRACING
#define INCLUDED_ACME_TRACING

// Include the interface templates
#include <opentracing/tracer.h>

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
    // For convenience
}
#endif
```

By creating the `Tracer` typedef, we can now rely on the GenericTracer interface,
but avoid knowing any of the explicit implementation details in client code.

The `Tracer` exposes a number of public typedefs which make it easier to create our
types:

 - `Tracer::Span`             - Interface to Span objects
 - `Tracer::SpanContext`      - Interface to SpanContext objects
 - `Tracer::SpanOptions`      - Interface to SpanOptions objects

Our theoretical "ACME Inc." organization can use these type names to instrument their applications.
The `Tracer` typedef is not standardized, but it is recommended. The rest of the documentation
will assume that `Tracer` was the chosen name for the Tracer type.

If clients ever require that they swap `OpenTracing` implementations, they
will need to update only this header file and rebuild applications.
(Easier said then done for larger organizations...)

### Installing the Tracer

Once a implementation is chosen, your applications can rely on your global `opentracing-cpp`
configuration header to set up the `Tracer`. The `Tracer` implementation singleton is installed
explicitly in main. This must be done before any other parts of the API can be used:

```
// acmetask.m.cpp
#include <acme_tracing.h>

using namespace acme;

int main(int argc, const char * argv[])
{
    TracerImpl tracerImpl;
    Tracer::install(&tracerImpl);

    // ...

    return 0;
}

```

### Creating Spans

Once the `Tracer` is installed, we can begin to instrument the application.
Say we have a service that acts as a proxy to a number of other HTTP servers.
We may have a `getAccount` handler that reaches out to a user account service.

To trace our application, we'll first need to start the span:

```
// requesthandler.m.cpp
#include <acme_tracing.h>

using namespace acme;

int getAccount(Repsonse * resp, const Request& req)
{
    Tracer::Span* span(Tracer::start("get_account"));
    assert(span);

    // When our span is complete, we have to call finish
    span->finish();

    // When we're done with our Span, we need to return it to
    // the Tracer implementation to be cleaned up
    Tracer::cleanup(span);

    return 0;
}
```

### Defining carriers

In order to instrument our entire network, we need a way to `inject` and `extract`
spans into and out of our RPC calls. Extending our example, when we make our request
to the backend HTTP service, we'll need a way to inject the details of the context
into our outgoing HTTP request.

We'll define our HttpWriter inline with our request to do the `inject` part of this
process first. In practice, you'll want the Writers/Readers you use to be consistent
across your organization, so putting them into a library would be ideal.

```
#include <opentracing/carriers.h>

using namespace opentracing;
using namespace acme;

class HttpWriter : public GenericBinaryWriter<HttpWriter>
{
  public:
    HttpWriter(HttpRequest * req): m_req(req){}

    int injectImp(const std::vector<char>& blob)
    {
        std::string header(blob.data(), blob.size());
        req->addHeader("x-acme-tracing-blob", header);
        return 0;
    }

  private:
    HttpRequest * m_req;
};

int getAccount(Repsonse * resp, const Request& req)
{
    Span* span(Tracer::start("get_account"));
    assert(span);

    HttpResponse httpResponse;
    HttpRequest httpRequest(req);

    // Embed the details of our span into the outgoing HTTP headers:
    HttpWriter writer(&httpRequest);
    Tracer::inject(&writer, span->context());

    sendHttpRequest(&httpResponse, httpRequest);

    // ...

    span->finish();
    Tracer::cleanup(span);
    return 0;
}
```

On the server side of the HTTP request, we'll want to continue the original Trace by creating
a new SpanContext as a 'child of' the original:

```
#include <opentracing/carriers.h>

using namespace opentracing;
using namespace acme;

struct HttpReader: public GenericBinaryReader<HttpReader>
{
  public:
    int extractImp(std::vector<std::string> * const buf) const
    {
        std::string header = m_req.getHeader("x-acme-tracing-blob");

        if (!header.empty())
        {
            buf->assign(header.begin(), header.end());
            return 0;
        }
        else
        {
            return 1;
        }
    }

  private:
    const HttpRequest& m_req;
};

int httpGetAccount(const HttpRequest& httpRequest)
{
    SpanContext* context(Tracer::extract(HttpReader(httpRequest)));

    Span* span;

    if (!context)
    {
        span = Tracer::start("get_account");
    }
    else
    {
        SpanOptions* opts(Tracer::makeSpanOptions());

        opts->setReference(SpanRelationship::e_ChildOf, *context);
        opts->setOperation("get_account_server");

        span = Tracer::start(opts);

        Tracer::cleanup(context);
        Tracer::cleanup(opts);
    }
    assert(span);

    // Send back our response...

    span->finish();
    Tracer::cleanup(span);

    return 0;
}
```

### Tags, Logs, and Baggage

The [OpenTracing specification](https://github.com/opentracing/specification/blob/master/specification.md) outlines how
users should be able tag their spans, log structured data for a span, or attach arbitrary baggage that propagates
through the entire system.

##### Tags
The `opentracing-cpp` interface allows you to tag your spans with `key:value` pairs:

```
Span* span = Tracer::start("get_bookmarks");
span->tag("account", account_id);
span->tag("site", site);
```

The `key` must be a string, but the value can be any type that can be externalized via

`std::ostream& operator<<(std::ostream&, const Type& t)`.


##### Logs
The `log` functions work similarly, but logs are also implicitly associated with the current wall-time as well.
Users can control the time-stamp behavior if they wish by providing it explicitly:

```
Span* span(Tracer::start("get_bookmarks"));
span->log("db_access", account_id);             // Use current wall-time
span->log("redis_access", site, 1484003943000); // Accessed redis on Jan 9, 2017 at 23:19:02 GMT
```

##### Baggage
Baggage is special. It is a set of text-only, key:value pairs that propagates with traces as they make their way
through a system. It can be prohibitively expensive if abused. It is also not a replacement for traditional message
schemas (e.g., protobuf). Care must be taken when adding any baggage.

With all that being said, it can be added directly to any Span you create through the underlying SpanContext.

```
Span* span = Tracer::start("get_bookmarks");
span->context().setBaggage("database", std::to_string(database_id));
span->context().setBaggage("user", user);
```

Baggage can also be read back through the original Span or SpanContexts extracted from carriers.
Note that SpanContexts created through 'extract()' are read-only.

```
Span* span(Tracer::start("get_bookmarks"));

span->context().setBaggage("database", std::to_string(database_id));
span->context().setBaggage("user", user);

for(SpanContext::BaggageIterator it = span->context().baggageBegin();
                                       it != span->context().baggageEnd();
                                     ++it)
{
    std::cout << "baggage item: " << it->key() << " val: " << it->value() << std::endl;
}
```

If you have access to C++11 features, we can use the range based for loop syntax too.

```
HttpReader reader(request);
SpanContext* context(Tracer::extract(reader));

for(const auto& baggage : context->baggageRange())
{
    std::cout << "baggage item: " << baggage.key() << " val: " << baggage.value() << std::endl;
}

Tracer::cleanup(context);
```

For details on the semantics of `BaggageIterators` and how they work, see [baggage.h](../opentracing/baggage.h).
