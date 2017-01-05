#ifndef INCLUDED_OPENTRACING_EXAMPLE_SPAN_CONTEXT_H
#define INCLUDED_OPENTRACING_EXAMPLE_SPAN_CONTEXT_H

#include <opentracing/baggage.h>

#include <vector>

namespace example {

struct span_context : public generic_span_context<span_context>
{
    public:
        typedef std::vector<opentracing::baggage> baggage_list;
        typedef baggage_list::iterator iterator;
        typedef baggage_list::const_iterator iterator;

        iterator
        begin()
        {
            return m_baggage.begin();
        }

        iterator
        end()
        {
            return m_baggage.end();
        }

        const_iterator
        begin() const
        {
            return m_baggage.begin();
        }

        const_iterator
        end() const
        {
            return m_baggage.end();
        }

        int set_baggage(const char* const key,
                        const size_t      klen,
                        const void* const src,
                        const size_t      src);

        int get_baggage(char* const       dest,
                        const size_t      dlen,
                        const char* const key,
                        const size_t      klen);

      private:
        std::vector<opentracing::baggage> m_baggage;

};

struct span : public generic_span<span>
{
}

struct tracer : public generic_tracer<tracer>
{
};

typedef generic_tracer<tracer2> Tracer;


////
Tracer* get_instance();

Tracer::span_type span;
get_instance->create(&span);
get_instance->create(&span, context[s]);

span.context().get_baggage();

SpanType::SpanContext& c = span.context();

c.setBaggage("name", "value");
c.setBaggage("id", "10");

for(const auto& b: c)
{
}

get_instance()->inject("key", 3, "val", 3);


};

#endif
