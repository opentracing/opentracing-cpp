#ifndef OPENTRACING_C_H
#define OPENTRACING_C_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct opentracing_tracer_t opentracing_tracer_t;

typedef enum opentracing_span_reference_type_t {
    opentracing_span_reference_child_of_ref = 1,
    opentracing_span_reference_follows_from_ref = 2
} opentracing_span_reference_type_t;

typedef enum opentracing_error_code_t {
    opentracing_invalid_span_context_error = 1,
    opentracing_invalid_carrier_context_error = 2,
    opentracing_span_context_corrupted_error = 3
} opentracing_error_code_t;

typedef struct opentracing_string_buffer_t {
    char* data;
    int size;
    int capacity;
} opentracing_string_buffer_t;

typedef struct opentracing_string_t {
    const char* data;
    int size;
} opentracing_string_t;

typedef enum opentracing_value_index_t {
    opentracing_value_index_bool,
    opentracing_value_index_double,
    opentracing_value_index_int64,
    opentracing_value_index_uint64,
    opentracing_value_index_string_buffer,
    opentracing_value_index_string,
    opentracing_value_index_values,
    opentracing_value_index_dictionary,
    opentracing_value_index_null = -1
} opentracing_value_index_t;

typedef struct opentracing_values_t opentracing_values_t;

typedef struct opentracing_dictionary_t opentracing_dictionary_t;

typedef struct opentracing_value_t {
    opentracing_value_index_t value_index;
    union {
        bool bool_value;
        double double_value;
        int64_t int64_value;
        uint64_t uint64_value;
        opentracing_string_buffer_t string_buffer_value;
        opentracing_string_t string_value;
        opentracing_values_t* list_value;
        opentracing_dictionary_t* dict_value;
    } data;
} opentracing_value_t;

struct opentracing_values_t {
    opentracing_value_t value;
    opentracing_values_t* next;
};

struct opentracing_dictionary_t {
    opentracing_string_t key;
    opentracing_value_t value;
    opentracing_dictionary_t* next;
};

typedef bool(*opentracing_foreach_key_value_callback_t)(
    const opentracing_string_t* key, const opentracing_string_t* value);

typedef struct opentracing_span_context_t {
    void (*destructor)(void* self);
    void (*foreach_baggage_item)(void* self,
                                 opentracing_foreach_key_value_callback_t f);
} opentracing_span_context_t;

typedef struct opentracing_finish_span_options_t {
    struct timespec finish_timestamp;
} opentracing_finish_span_options_t;

typedef struct opentracing_span_t {
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
} opentracing_span_t;

typedef struct opentracing_text_map_reader_t {
    void (*destructor)(void* self);
    int (*foreach_key)(const void* self,
                       opentracing_foreach_key_value_callback_t f);
} opentracing_text_map_reader_t;

typedef struct opentracing_text_map_writer_t {
    void (*destructor)(void* self);
    int (*set)(void* self,
               const opentracing_string_t* key,
               const opentracing_string_t* value);
} opentracing_text_map_writer_t;

typedef struct opentracing_http_headers_reader_t {
    void (*destructor)(void* self);
    int (*foreach_key)(const void* self,
                       opentracing_foreach_key_value_callback_t f);
} opentracing_http_headers_reader_t;

typedef struct opentracing_http_headers_writer_t {
    void (*destructor)(void* self);
    int (*set)(void* self,
               const opentracing_string_t* key,
               const opentracing_string_t* value);
} opentracing_http_headers_writer_t;

typedef struct opentracing_custom_carrier_reader_t {
    void (*destructor)(void* self);
    opentracing_span_context_t* (*extract)(const void* self,
                                           const opentracing_tracer_t* tracer);
} opentracing_custom_carrier_reader_t;

typedef struct opentracing_custom_carrier_writer_t {
    void (*destructor)(void* self);
    opentracing_span_context_t* (*inject)(const void* self,
                                          const opentracing_tracer_t* tracer,
                                          const opentracing_span_context_t* sc);
} opentracing_custom_carrier_writer_t;

typedef struct opentracing_span_references_t {
    opentracing_span_reference_type_t type;
    const opentracing_span_context_t* context;
    struct opentracing_span_references_t* next;
} opentracing_span_references_t;

typedef opentracing_dictionary_t opentracing_tags_t;

typedef struct opentracing_start_span_options_t {
    struct timespec start_timestamp;
    const opentracing_span_references_t* references;
    const opentracing_tags_t* tags;
} opentracing_start_span_options_t;

struct opentracing_tracer_t {
    void (*destructor)(void* self);
    opentracing_span_t* (*start_span_with_options)(
        const void* self,
        const opentracing_string_t* operation_name,
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

opentracing_tracer_t* opentracing_global_tracer();

opentracing_tracer_t* opentracing_init_global_tracer(
    opentracing_tracer_t* tracer);

opentracing_tracer_t* opentracing_make_noop_tracer();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* OPENTRACING_C_H */
