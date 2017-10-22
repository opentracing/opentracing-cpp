#ifndef OPENTRACING_C_H
#define OPENTRACING_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

typedef struct opentracing_tracer_t opentracing_tracer_t;
typedef struct opentracing_span_t opentracing_span_t;
typedef struct opentracing_spancontext_t opentracing_spancontext_t;
typedef struct opentracing_finishspanoptions_t opentracing_finishspanoptions_t;
typedef struct opentracing_value_t opentracing_value_t;

void opentracing_foreach_baggage_item(
    const opentracing_spancontext_t* span_context,
    bool (*callback_fn)(const char*, const char*));

void opentracing_finish_span_with_options(
    opentracing_span_t* span,
    const opentracing_finishspanoptions_t* options);

void opentracing_finish_span(opentracing_span_t* span);

void opentracing_set_operation_name(
    opentracing_span_t* span, const char* operation_name);

void opentracing_set_tag(
    opentracing_span_t* span,
    const char* key,
    const opentracing_value_t* value);

void opentracing_set_baggage_item(
    opentracing_span_t* span,
    const char* key,
    const char* value);

const char* opentracing_baggage_item(opentracing_span_t* span, const char* key);

typedef struct opentracing_logfield_t {
    const char* key;
    const opentracing_value_t value;
} opentracing_logfield_t;

void opentracing_log(
    opentracing_span_t* span,
    const opentracing_logfield_t fields[]);

const opentracing_spancontext_t* opentracing_context(
    const opentracing_span_t* span);

const opentracing_tracer_t* opentracing_tracer(
    const opentracing_span_t* span);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* OPENTRACING_C_H */
