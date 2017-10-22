#include <opentracing/c.h>
#include <opentracing/value.h>

struct opentracing_value_t {
    opentracing::Value impl;
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

}  // anonymous namespace

extern "C" opentracing_value_t* opentracing_new_value()
{
    return new opentracing_value_t();
}

extern "C" opentracing_valuetype_t opentracing_value_type(
    const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    ValueTypeVisitor visitor;
    return opentracing::util::apply_visitor(visitor, impl);
}

extern "C" bool opentracing_get_value_bool(
    const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    return impl.get<bool>();
}

extern "C" double opentracing_get_value_double(
    const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    return impl.get<double>();
}

extern "C" int64_t opentracing_get_value_int64(const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    return impl.get<int64_t>();
}

extern "C" uint64_t opentracing_get_value_uint64(
    const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    return impl.get<uint64_t>();
}

extern "C" const char* opentracing_get_value_string(
    const opentracing_value_t* value)
{
    const opentracing::Value& impl = value->impl;
    if (impl.is<std::string>()) {
        return impl.get<std::string>().c_str();
    }
    return impl.get<const char*>();
}

extern "C" void opentracing_set_value_bool(opentracing_value_t* value, bool val)
{
    opentracing::Value& impl = value->impl;
    impl = val;
}

extern "C" void opentracing_set_value_double(
    opentracing_value_t* value, double val)
{
    opentracing::Value& impl = value->impl;
    impl = val;
}

extern "C" void opentracing_set_value_int64(
    opentracing_value_t* value, int64_t val)
{
    opentracing::Value& impl = value->impl;
    impl = val;
}

extern "C" void opentracing_set_value_uint64(
    opentracing_value_t* value, uint64_t val)
{
    opentracing::Value& impl = value->impl;
    impl = val;
}

extern "C" void opentracing_set_value_string(
    opentracing_value_t* value, const char* val)
{
    opentracing::Value& impl = value->impl;
    impl = val;
}
