#ifndef OPENTRACING_SYMBOLS_H
#define OPENTRACING_SYMBOLS_H

#include <opentracing/config.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#ifdef _MSC_VER

#define OPEN_TRACING_EXPORT __declspec(dllexport)

// Export if this is our own source, otherwise import:
#ifndef OPENTRACING_STATIC
#ifdef OPENTRACING_EXPORTS
#define OPEN_TRACING_API __declspec(dllexport)
#else
#define OPEN_TRACING_API __declspec(dllimport)
#endif
#define OPEN_TRACING_ATT
#endif
#endif  // _MSC_VER

#ifndef OPEN_TRACING_EXPORT
#define OPEN_TRACING_EXPORT
#endif

#ifndef OPEN_TRACING_API
#define OPEN_TRACING_API
#ifdef _MSC_VER
#define OPEN_TRACING_ATT
#else
#define OPEN_TRACING_ATT __attribute((weak))
#endif
#endif

#endif  // OPENTRACING_SYMBOLS_H
