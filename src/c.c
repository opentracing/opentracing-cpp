#include <opentracing/c.h>

#include <stdatomic.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static atomic_uintptr_t instance;

opentracing_tracer_t* opentracing_global_tracer()
{
    return (opentracing_tracer_t*) instance;
}

opentracing_tracer_t* opentracing_init_global_tracer(
    opentracing_tracer_t* tracer)
{
    return (opentracing_tracer_t*) atomic_exchange(&instance,
                                                   (uintptr_t) tracer);
}

static void noop_destructor(void* self)
{
}

typedef struct noop_span_context_t {
    void (*destructor)(void* self);
    void (*foreach_baggage_item)(void* self,
                                 opentracing_foreach_key_value_callback_t f);
} noop_span_context_t;

static void noop_span_context_foreach_baggage_item(
    void* self, opentracing_foreach_key_value_callback_t f)
{
}

static void noop_span_context_init(noop_span_context_t* span_context)
{
    span_context->destructor = &noop_destructor;
    span_context->foreach_baggage_item =
        &noop_span_context_foreach_baggage_item;
}

typedef struct noop_tracer_t noop_tracer_t;

typedef struct noop_span_t {
    void (*destructor)(void* self);
    void (*finish)(void* self,
                   const opentracing_finish_span_options_t* options);
    void (*set_operation_name)(void* self,
                               const opentracing_string_t* name);
    void (*set_tag)(void* self,
                    const opentracing_string_t* key,
                    const opentracing_value_t* value);
    void (*set_baggage_item)(void* self,
                             const opentracing_string_t* restricted_key,
                             const opentracing_string_t* value);
    const opentracing_value_t* (*baggage_item)(
        const void* self,
        const opentracing_string_t* restricted_key);
    void (*log)(void* self,
                const opentracing_dictionary_t* fields);
    const opentracing_span_context_t* (*get_context)(const void* self);
    const opentracing_tracer_t* (*get_tracer)(const void* self);

    noop_span_context_t context;
    const noop_tracer_t* tracer;
} noop_span_t;

static void noop_span_finish(
    void* self,
    const opentracing_finish_span_options_t* options)
{
}

static void noop_span_set_operation_name(
    void* self, const opentracing_string_t* name)
{
}

static void noop_span_set_tag(
    void* self,
    const opentracing_string_t* key,
    const opentracing_value_t* value)
{
}

static void noop_span_set_baggage_item(
    void* self,
    const opentracing_string_t* key,
    const opentracing_string_t* value)
{
}

static const opentracing_value_t* noop_span_baggage_item(
    const void* self,
    const opentracing_string_t* restricted_key)
{
    return NULL;
}

static void noop_span_log(
    void* self, const opentracing_dictionary_t* fields)
{
}

static const opentracing_span_context_t* noop_span_get_context(const void* self)
{
    return (const opentracing_span_context_t*) &((noop_span_t*) self)->context;
}

static const opentracing_tracer_t* noop_span_get_tracer(const void* self)
{
    return (const opentracing_tracer_t*) ((noop_span_t*) self)->tracer;
}

static void noop_span_init(noop_span_t* span)
{
    span->destructor = &noop_destructor;
    span->finish = &noop_span_finish;
    span->set_operation_name = &noop_span_set_operation_name;
    span->set_tag = &noop_span_set_tag;
    span->set_baggage_item = &noop_span_set_baggage_item;
    span->baggage_item = &noop_span_baggage_item;
    span->log = &noop_span_log;
    span->get_context = &noop_span_get_context;
    span->get_tracer = &noop_span_get_tracer;
}

struct noop_tracer_t {
    void (*destructor)(void* self);
    opentracing_span_t* (*start_span_with_options)(
        const void* self,
        const opentracing_start_span_options_t* options);
    int (*inject_binary)(const void* self,
                         const opentracing_span_context_t* sc,
                         opentracing_string_buffer_t* writer);
    int (*inject_text_map)(const void* self,
                           const opentracing_span_context_t* sc,
                           opentracing_text_map_writer_t* writer);
    int (*inject_http_headers)(const void* self,
                               const opentracing_span_context_t* sc,
                               opentracing_http_headers_writer_t* writer);
    int (*inject_custom_carrier)(const void* self,
                                 const opentracing_span_context_t* sc,
                                 opentracing_custom_carrier_writer_t* writer);
    opentracing_span_context_t* (*extract_binary)(
        const void* self,
        const opentracing_string_t* reader);
    opentracing_span_context_t* (*extract_text_map)(
        const void* self,
        const opentracing_text_map_reader_t* reader);
    opentracing_span_context_t* (*extract_http_headers)(
        const void* self,
        const opentracing_http_headers_reader_t* reader);
    opentracing_span_context_t* (*extract_custom_carrier)(
        const void* self,
        const opentracing_custom_carrier_reader_t* reader);
    void (*close)(void* self);
};

static opentracing_span_t* noop_tracer_start_span_with_options(
    const void* self, const opentracing_start_span_options_t* option)
{
    noop_span_t* span = (noop_span_t*) malloc(sizeof(noop_span_t));
    if (!span) {
        return (opentracing_span_t*) span;
    }
    noop_span_init(span);
    span->tracer = ((const noop_tracer_t*) self);
}

static int noop_tracer_inject_binary(
    const void* self,
    const opentracing_span_context_t* sc,
    opentracing_string_buffer_t* writer)
{
    return 0;
}

static int noop_tracer_inject_text_map(
    const void* self,
    const opentracing_span_context_t* sc,
    opentracing_text_map_writer_t* writer)
{
    return 0;
}

static int noop_tracer_inject_http_headers(
    const void* self,
    const opentracing_span_context_t* sc,
    opentracing_http_headers_writer_t* writer)
{
    return 0;
}

static int noop_tracer_inject_custom_carrier(
    const void* self,
    const opentracing_span_context_t* sc,
    opentracing_custom_carrier_writer_t* writer)
{
    return 0;
}

static opentracing_span_context_t* noop_tracer_extract_binary(
    const void* self,
    const opentracing_string_t* reader)
{
    noop_span_context_t* span_context =
        (noop_span_context_t*) malloc(sizeof(noop_span_context_t));
    if (!span_context) {
        return (opentracing_span_context_t*) span_context;
    }
    noop_span_context_init(span_context);
    return (opentracing_span_context_t*) span_context;
}

static opentracing_span_context_t* noop_tracer_extract_text_map(
    const void* self,
    const opentracing_text_map_reader_t* reader)
{
    noop_span_context_t* span_context = malloc(sizeof(noop_span_context_t));
    if (!span_context) {
        return (opentracing_span_context_t*) span_context;
    }
    noop_span_context_init(span_context);
    return (opentracing_span_context_t*) span_context;
}

static opentracing_span_context_t* noop_tracer_extract_http_headers(
    const void* self,
    const opentracing_http_headers_reader_t* reader)
{
    noop_span_context_t* span_context = malloc(sizeof(noop_span_context_t));
    if (!span_context) {
        return (opentracing_span_context_t*) span_context;
    }
    noop_span_context_init(span_context);
    return (opentracing_span_context_t*) span_context;
}

static opentracing_span_context_t* noop_tracer_extract_custom_carrier(
    const void* self,
    const opentracing_custom_carrier_reader_t* reader)
{
    noop_span_context_t* span_context = malloc(sizeof(noop_span_context_t));
    if (!span_context) {
        return (opentracing_span_context_t*) span_context;
    }
    noop_span_context_init(span_context);
    return (opentracing_span_context_t*) span_context;
}

static void noop_tracer_close(void* self)
{
}

static void noop_tracer_init(noop_tracer_t* tracer)
{
    tracer->destructor = &noop_destructor;
    tracer->start_span_with_options = &noop_tracer_start_span_with_options;
    tracer->inject_binary = &noop_tracer_inject_binary;
    tracer->inject_text_map = &noop_tracer_inject_text_map;
    tracer->inject_http_headers = &noop_tracer_inject_http_headers;
    tracer->inject_custom_carrier = &noop_tracer_inject_custom_carrier;
    tracer->extract_binary = &noop_tracer_extract_binary;
    tracer->extract_text_map = &noop_tracer_extract_text_map;
    tracer->extract_http_headers = &noop_tracer_extract_http_headers;
    tracer->extract_custom_carrier = &noop_tracer_extract_custom_carrier;
    tracer->close = &noop_tracer_close;
}

opentracing_tracer_t* opentracing_make_noop_tracer()
{
    noop_tracer_t* tracer = (noop_tracer_t*) malloc(sizeof(noop_tracer_t));
    if (!tracer) {
        return (opentracing_tracer_t*) tracer;
    }
    noop_tracer_init(tracer);
    return (opentracing_tracer_t*) tracer;
}

#ifdef __cplusplus
}  /* extern "C" */
#endif
