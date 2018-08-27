#ifndef OPENTRACING_SYMBOLS_H
#define OPENTRACING_SYMBOLS_H

#include <opentracing/config.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#ifdef _MSC_VER

#define OPENTRACING_EXPORT __declspec(dllexport)

// Export if this is our own source, otherwise import:
#ifndef OPENTRACING_STATIC
#ifdef OPENTRACING_EXPORTS
#define OPENTRACING_API __declspec(dllexport)
#else  // OPENTRACING_STATIC
#define OPENTRACING_API __declspec(dllimport)
#endif  // OPENTRACING_EXPORTS
#endif  // OPENTRACING_STATIC

#endif  // _MSC_VER

#ifndef OPENTRACING_EXPORT
#define OPENTRACING_EXPORT
#endif

#ifndef OPENTRACING_API
#define OPENTRACING_API
#endif

#endif  // OPENTRACING_SYMBOLS_H
