#ifndef INCLUDED_TRACING_H
#define INCLUDED_TRACING_H

#include <opentracing/tracer.h>
#include <opentracing/noop.h>

#include <hot.h>

using namespace opentracing;

// Defining two "different" tracer implementations
typedef NoopTracer ZipkinV1;
typedef NoopTracer ZipkinV2;

// Setting up stuff... Would be nice if they weren't that many...
// is there any way `GenericTracer` to deduce those from just MyHotTracer?
typedef HotTracer<ZipkinV1, ZipkinV2> MyHotTracer;
typedef HotSpan<ZipkinV1, ZipkinV2> MyHotSpan;
typedef HotOptions<ZipkinV1, ZipkinV2> MyHotOptions;
typedef HotContext<ZipkinV1, ZipkinV2> MyHotContext;
typedef HotAdapter<ZipkinV1, ZipkinV2> MyHotAdapter;

typedef GenericTracer<MyHotTracer,
                      MyHotSpan,
                      MyHotOptions,
                      MyHotContext,
                      MyHotAdapter>
    GlobalTracer;

#include <tracing_cpp11.h>

// Now we "upgrade" the entire system to C++11. Yeiii!
typedef Cpp11Tracer<GlobalTracer> Cpp11GlobalTracer;


#endif // INCLUDED_TRACING_H
