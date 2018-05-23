#ifndef OPENTRACING_MOCK_TRACER_SYMBOLS_H
#define OPENTRACING_MOCK_TRACER_SYMBOLS_H

#include <opentracing/config.h>

#ifdef _MSC_VER
// Export if this is our own source, otherwise import:
#ifndef OPENTRACING_MOCK_TRACER_STATIC
#ifdef OPENTRACING_MOCK_TRACER_EXPORTS
#define OPEN_TRACING_MOCK_TRACER_API __declspec(dllexport)
#else
#define OPEN_TRACING_MOCK_TRACER_API __declspec(dllimport)
#endif
#endif
#endif  // _MSC_VER

#ifndef OPEN_TRACING_MOCK_TRACER_API
#define OPEN_TRACING_MOCK_TRACER_API
#endif

#endif  // OPENTRACING_SYMBOLS_H
