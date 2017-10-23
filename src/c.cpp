#include <opentracing/c.h>

#include <cstring>
#include <memory>
#include <sstream>

#include <opentracing/span.h>
#include <opentracing/tracer.h>
#include <opentracing/value.h>

struct opentracing_value_t {
    opentracing::Value impl_;
};

struct opentracing_spancontext_t {
    const opentracing::SpanContext* impl_;
};

struct opentracing_span_t {
    opentracing::Span* impl_;
};

struct opentracing_tracer_t {
    const opentracing::Tracer* impl_;
};

namespace {

struct ValueTypeVisitor {
    using result_type = opentracing_valuetype_t;

    opentracing_valuetype_t operator()(bool /* unused */) const
    {
        return opentracing_valuetype_bool;
    }

    opentracing_valuetype_t operator()(double /* unused */) const
    {
        return opentracing_valuetype_double;
    }

    opentracing_valuetype_t operator()(int64_t /* unused */) const
    {
        return opentracing_valuetype_int64;
    }

    opentracing_valuetype_t operator()(uint64_t /* unused */) const
    {
        return opentracing_valuetype_uint64;
    }

    opentracing_valuetype_t operator()(const std::string& /* unused */) const
    {
        return opentracing_valuetype_string;
    }

    opentracing_valuetype_t operator()(const char* /* unused */) const
    {
        return opentracing_valuetype_string;
    }

    template <typename UnknownType>
    opentracing_valuetype_t operator()(UnknownType&& /* unused */) const
    {
        return opentracing_valuetype_unknown;
    }
};

template <typename Clock>
typename Clock::time_point timespecToTimePoint(const struct timespec& time)
{
    return typename Clock::time_point() +
           std::chrono::duration_cast<typename Clock::duration>(
                std::chrono::seconds(time.tv_sec)) +
           std::chrono::duration_cast<typename Clock::duration>(
                std::chrono::nanoseconds(time.tv_nsec));
}

}  // anonymous namespace

extern "C" opentracing_value_t* opentracing_new_value()
{
    return new opentracing_value_t();
}

extern "C" void opentracing_free_value(opentracing_value_t* value)
{
    delete value;
}

extern "C" opentracing_valuetype_t opentracing_value_type(
    const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    ValueTypeVisitor visitor;
    return opentracing::util::apply_visitor(visitor, impl);
}

extern "C" bool opentracing_get_value_bool(
    const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    return impl.get<bool>();
}

extern "C" double opentracing_get_value_double(
    const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    return impl.get<double>();
}

extern "C" int64_t opentracing_get_value_int64(const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    return impl.get<int64_t>();
}

extern "C" uint64_t opentracing_get_value_uint64(
    const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    return impl.get<uint64_t>();
}

extern "C" const char* opentracing_get_value_string(
    const opentracing_value_t* value)
{
    const auto& impl = value->impl_;
    if (impl.is<std::string>()) {
        return impl.get<std::string>().c_str();
    }
    return impl.get<const char*>();
}

extern "C" void opentracing_set_value_bool(opentracing_value_t* value, bool val)
{
    auto& impl = value->impl_;
    impl = val;
}

extern "C" void opentracing_set_value_double(
    opentracing_value_t* value, double val)
{
    auto& impl = value->impl_;
    impl = val;
}

extern "C" void opentracing_set_value_int64(
    opentracing_value_t* value, int64_t val)
{
    auto& impl = value->impl_;
    impl = val;
}

extern "C" void opentracing_set_value_uint64(
    opentracing_value_t* value, uint64_t val)
{
    auto& impl = value->impl_;
    impl = val;
}

extern "C" void opentracing_set_value_string(
    opentracing_value_t* value, const char* val)
{
    auto& impl = value->impl_;
    impl = val;
}

extern "C" void opentracing_foreach_baggage_item(
    const opentracing_spancontext_t* span_context,
    bool (*callback_fn)(const char*, const char*, void*),
    void* context)
{
    const auto* impl = span_context->impl_;
    impl->ForeachBaggageItem([callback_fn,
                              context](const std::string& key,
                                       const std::string& value) {
        return callback_fn(key.c_str(), value.c_str(), context);
    });
}

extern "C" void opentracing_finish_span_with_options(
    opentracing_span_t* span,
    const opentracing_finishspanoptions_t* options)
{
    opentracing::FinishSpanOptions cppOptions;
    cppOptions.finish_steady_timestamp =
        timespecToTimePoint<opentracing::SteadyClock>(
            options->finish_timestamp);
    span->impl_->FinishWithOptions(cppOptions);
}

extern "C" void opentracing_finish_span(opentracing_span_t* span)
{
    span->impl_->Finish();
}

extern "C" void opentracing_set_operation_name(
    opentracing_span_t* span, const char* operation_name)
{
    span->impl_->SetOperationName(operation_name);
}

extern "C" void opentracing_set_tag(
    opentracing_span_t* span,
    const char* key,
    const opentracing_value_t* value)
{
    span->impl_->SetTag(key, value->impl_);
}

extern "C" void opentracing_set_baggage_item(
    opentracing_span_t* span,
    const char* key,
    const char* value)
{
    span->impl_->SetBaggageItem(key, value);
}

extern "C" const char* opentracing_baggage_item(
    opentracing_span_t* span,
    const char* key)
{
    return span->impl_->BaggageItem(key).c_str();
}

/* TODO
extern "C" void opentracing_log(
    opentracing_span_t* span,
    const opentracing_tag_t fields[])
{
    span->impl_->Log(fields);
}*/

extern "C" void opentracing_context_from_span(
    opentracing_spancontext_t* span_context,
    const opentracing_span_t* span)
{
    span_context->impl_ = &span->impl_->context();
}

extern "C" void opentracing_tracer_from_span(
    opentracing_tracer_t* tracer,
    const opentracing_span_t* span)
{
    tracer->impl_ = &span->impl_->tracer();
}

extern "C" opentracing_span_t* opentracing_start_span(
    opentracing_tracer_t* tracer,
    const char* operation_name)
{
    std::unique_ptr<opentracing_span_t> span(new opentracing_span_t());
    span->impl_ = tracer->impl_->StartSpan(operation_name).release();
    return span.release();
}

extern "C" opentracing_span_t* opentracing_start_span_with_options(
    opentracing_tracer_t* tracer,
    const char* operation_name,
    const opentracing_startspanoptions_t* options)
{
    opentracing::StartSpanOptions cppOptions;
    cppOptions.start_steady_timestamp =
        timespecToTimePoint<opentracing::SteadyClock>(
            options->start_steady_timestamp);
    cppOptions.start_system_timestamp =
        timespecToTimePoint<opentracing::SystemClock>(
            options->start_system_timestamp);

    for (auto i = 0; i < options->num_references; ++i) {
        const auto* ref = options->references[i];
        cppOptions.references.emplace_back(
                static_cast<opentracing::SpanReferenceType>(ref->type),
                ref->context->impl_);
    }

    for (auto i = 0; i < options->num_tags; ++i) {
        const auto* tag = options->tags[i];
        cppOptions.tags.emplace_back(tag->key, tag->value->impl_);
    }

    std::unique_ptr<opentracing_span_t> span(new opentracing_span_t());
    span->impl_ = tracer->impl_->StartSpanWithOptions(
                    operation_name, cppOptions).release();
    return span.release();
}

extern "C" void opentracing_free_span(opentracing_span_t* span)
{
    delete span;
}

/* TODO:
extern "C" int opentracing_inject_binary(
    opentracing_tracer_t* tracer,
    const opentracing_spancontext_t* span_context,
    char* buffer,
    int bufferSize)
{
    std::ostringstream oss;
    tracer->impl_->Inject(*span_context->impl_, oss);
    const auto data = oss.str();
    if (static_cast<int>(data.size()) > bufferSize) {
        return -data.size();
    }
    std::memcpy(buffer, data.c_str(), data.size());
    return data.size();
}

extern "C" int opentracing_inject_text(
    opentracing_tracer_t* tracer,
    int (*writer_fn)(const char*, const char*, void*),
    void* context);

extern "C" int opentracing_inject_http(
    opentracing_tracer_t* tracer,
    int (*writer_fn)(const char*, const char*, void*),
    void* context);

extern "C" int opentracing_inject_custom(
    opentracing_tracer_t* tracer,
    int (*writer_fn)(const opentracing_tracer_t*,
                     const opentracing_spancontext_t*,
                     void*),
    void* context);

extern "C" int opentracing_extract_binary(
    opentracing_spancontext_t* sc,
    const opentracing_tracer_t* tracer,
    int (*reader_fn)(char*, int, void*),
    void* context);

extern "C" int opentracing_extract_text(
    opentracing_spancontext_t* sc,
    const opentracing_tracer_t* tracer,
    int (*reader_fn)(const char*, const char*, void*),
    void* context);

extern "C" int opentracing_extract_http(
    opentracing_spancontext_t* sc,
    const opentracing_tracer_t* tracer,
    int (*reader_fn)(const char*, const char*, void*),
    void* context);

extern "C" int opentracing_extract_custom(
    const opentracing_tracer_t* tracer,
    int (*reader_fn)(const opentracing_tracer_t*,
                     const opentracing_spancontext_t*,
                     void*),
    void* context);
*/

extern "C" void opentracing_close_tracer(opentracing_tracer_t* tracer)
{
    auto* impl = const_cast<opentracing::Tracer*>(tracer->impl_);
    impl->Close();
}

extern "C" void opentracing_global_tracer(opentracing_tracer_t* tracer)
{
    tracer->impl_ = opentracing::Tracer::Global().get();
}
