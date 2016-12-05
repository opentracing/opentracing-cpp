#include <opentracing/tracer.h>
#include <opentracing/span.h>

#include <gtest/gtest.h>

#include <stdexcept>

#include <map>

using namespace opentracing;

TEST(opentracing, init)
{
    // Get the default tracer.
    Tracer * tracer = globalTracer();

    // The default tracer is non-NULL.
    ASSERT_TRUE(tracer);

    // We aren't allowed to set the value to NULL.
    ASSERT_THROW(initGlobalTracer(NULL), std::runtime_error);

    // Create a local noop Tracer.
    NoopTracer localTracer;

    // It's previous value should be the default tracer
    // thus null is returned. We assign to a random value
    ASSERT_FALSE(initGlobalTracer(&localTracer));

    // Note - at this point the globalTracer() points to a completely
    // random address. Any operation (apart from retrieval) would be
    // expected to fail.

    // This should return the value we just set.
    ASSERT_EQ(&localTracer, globalTracer());

    // Restore tracer (to the default tracer). The value returned should
    // be the one we set last. Here the application might be required to
    // perform finalizations and/or `delete` the old `tracer` object.
    ASSERT_EQ(&localTracer, initGlobalTracer(tracer));

    // Once again, this should return the value we just set.
    ASSERT_EQ(tracer, globalTracer());
}

TEST(opentracing, creation)
{
    // Accidentaly forget to assign it anywhere. Doesn't matter. Auto-deletes.
    globalTracer()->startSpan("test()");

    // You can create it and store it.
    Span * parent(globalTracer()->startSpan("test()"));

    ASSERT_TRUE(parent);
    ASSERT_EQ(&parent->getTracer(), globalTracer());

    // Or you can create it and use the builder to populate it.
    Span * child(globalTracer()->startSpan("test()", StartSpanOptions(parent, 100))
            ->setOperationName("update()")
            ->setTag("stringValue", "test")
            ->setTag("boolValue", true)
            ->setTag("doubleValue", 3.14)
            ->setTag("intValue", 12));

    ASSERT_TRUE(child);
    ASSERT_EQ(&child->getTracer(), globalTracer());

    delete child;
    delete parent;
}

TEST(opentracing, span_ops)
{
    // Create a Span
    Span * span = globalTracer()->startSpan("test()");

    ASSERT_TRUE(span);

    // Set the operation name
    span->setOperationName("newName()");

    // Exercise various setTag() methods
    span->setTag("stringValue", "test");
    span->setTag("boolValue", true);
    span->setTag("doubleValue", 3.14);
    span->setTag("intValue", 12);

    // Exercise different log() cases
    LogData tmp;
    span->log(tmp);
    span->log(LogData("ready"));
    span->log(LogData("ready", "{\"j\":\"son\"}"));

    uint64_t now(100);
    span->log(LogData(now));
    span->log(LogData(now, "ready"));
    span->log(LogData(now, "ready", "{\"j\":\"son\"}"));

    // Exercise bulk log() case
    std::vector<LogData> bulk;
    bulk.push_back(tmp);
    bulk.push_back(tmp);
    span->log(bulk);

    // Exercise baggage functions
    span->setBaggageItem("key1", "value1")
            ->setBaggageItem("key2", "value2")
            ->setBaggageItem("key3", "value3");

    // No-op implementation shouldn't incur any baggage overhead i.e.
    // it should ignore the values despite the fact they're set with
    // the previous statement
    std::string target;
    ASSERT_FALSE(span->getBaggageItem("key1"));
    ASSERT_FALSE(span->getBaggageItem("key2"));
    ASSERT_FALSE(span->getBaggageItem("key3", &target));

    // One can print trace identifiers by using the `print()` method.
    std::ostringstream oss;
    span->print(oss);
    ASSERT_EQ("NoopSpan: N/A", oss.str());


    // Exercise getTracer()
    ASSERT_EQ(&span->getTracer(), globalTracer());

    // Exercise finish()
    span->finish();

    delete span;
}

TEST(opentracing, canonicalize_baggage_key)
{
    std::string badKey("some-weird-sign!#");
    ASSERT_FALSE(canonicalizeBaggageKey(badKey));

    std::string badKey2("-another-sign");
    ASSERT_FALSE(canonicalizeBaggageKey(badKey2));

    std::string goodKey("000-Capitalized-9");
    ASSERT_TRUE(canonicalizeBaggageKey(goodKey));
    ASSERT_EQ("000-capitalized-9", goodKey);
}

namespace
{

template <typename T>
class MapTextAdapter : public TextMapWriter, public TextMapReader
{
public:
    MapTextAdapter(T & carrier)
        : m_carrier(carrier)
    {
    }

    virtual ~MapTextAdapter()
    {
    }

    virtual void set(const std::string& key, const std::string& value, bool isBaggage) const
    {
        m_carrier.insert(typename T::value_type(key, std::make_pair(value, isBaggage)));
    }

    virtual void forEachPair(const ReadCallback & callback) const
    {
        for (typename T::const_iterator i = m_carrier.begin(); i != m_carrier.end(); ++i)
        {
            callback(i->first, i->second.first, i->second.second);
        }
    }

private:
    T & m_carrier;
};    

}

typedef std::map<std::string, std::pair<std::string, bool> > CarrierT;

TEST(opentracing, propagation_inject)
{
    Span * span = globalTracer()->startSpan("test()");

    ASSERT_TRUE(span);

    CarrierT carrier;
    std::string error;

    Tracer::Result result = globalTracer()->inject(
            *span,
            MapTextAdapter<CarrierT>(carrier),
            error);

    ASSERT_EQ(Tracer::Success, result);

    span->finish();

    delete span;
}

TEST(opentracing, propagation_join)
{
    CarrierT carrier;
    std::string error;

    Span * span(0);
    Tracer::Result result = globalTracer()->join(
            &span,
            "operation()",
            MapTextAdapter<CarrierT>(carrier),
            error);

    ASSERT_EQ(Tracer::ErrTraceNotFound, result);
    ASSERT_EQ(0, span);
}
