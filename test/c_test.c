#include <opentracing/c.h>

#include <assert.h>
#include <stdlib.h>

static void test_c_interface()
{
    opentracing_tracer_t* tracer = opentracing_make_noop_tracer();

    const opentracing_string_t operation_name1 = {
        .data = "a",
        .size = 1
    };
    opentracing_start_span_options_t options1;
    timespec_get(&options1.start_timestamp, TIME_UTC);
    options1.references = NULL;
    options1.tags = NULL;
    opentracing_span_t* span1 =
        tracer->start_span_with_options(tracer, &operation_name1, &options1);
    assert(span1);
    assert(span1->get_tracer(span1) == tracer);

    const opentracing_string_t operation_name2 = {
        .data = "b",
        .size = 1
    };
    opentracing_start_span_options_t options2;
    timespec_get(&options2.start_timestamp, TIME_UTC);
    options2.references = NULL;
    options2.tags = NULL;
    opentracing_span_t* span2 =
        tracer->start_span_with_options(tracer, &operation_name2, &options2);
    assert(span2);

    const opentracing_string_t operation_name3 = {
        .data = "b1",
        .size = 2
    };
    span2->set_operation_name(span2, &operation_name3);

    const opentracing_string_t tag_name = {
        .data = "x",
        .size = 1
    };
    const opentracing_value_t tag_value = {
        .value_index = opentracing_value_index_uint64,
        .data = {
            .uint64_value = 1
        }
    };
    span2->set_tag(span2, &tag_name, &tag_value);
    const opentracing_string_t baggage_item_key = { .data = "y", .size = 1 };
    assert(span2->baggage_item(span2, &baggage_item_key) == NULL);

    opentracing_dictionary_t* fields =
        (opentracing_dictionary_t*) malloc(sizeof(opentracing_dictionary_t));
    assert(fields);
    fields->key = (opentracing_string_t) { .data = "event", .size = 5 };
    fields->value =
        (opentracing_value_t) { .value_index = opentracing_value_index_string,
                                .data = {
                                    .string_value = {
                                        .data = "xyz",
                                        .size = 3 } } };
    fields->next =
        (opentracing_dictionary_t*) malloc(sizeof(opentracing_dictionary_t));
    fields->next->key =
        (opentracing_string_t) { .data = "abc", .size = 3 };
    fields->next->value =
        (opentracing_value_t) { .value_index = opentracing_value_index_int64,
                                .data = { .int64_value = 123 } };
    span2->log(span2, fields);
    free(fields->next);
    free(fields);

    span2->destructor(span2);
    free(span2);
    span1->destructor(span1);
    free(span1);
    tracer->destructor(tracer);
    free(tracer);
}

int main()
{
    test_c_interface();
    return 0;
}
