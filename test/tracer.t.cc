#include "unittest.h"
#include "tracer.h"

#include "carriers.h"

TestTracerImpl* TestTracerImpl::s_tracer = 0;

class TracerEnv : public ::testing::Test {
  public:
    virtual void
    SetUp()
    {
        Tracer::install(&imp);
        tracer = &imp;
    };

    virtual void
    TearDown()
    {
        Tracer::uninstall();
    };

    TestTracerImpl imp;
    Tracer * tracer;
};

TEST_F(TracerEnv, SingletonTests)
{
    ASSERT_EQ(&imp, Tracer::instance());
    Tracer::uninstall();
    ASSERT_EQ(0, Tracer::instance());
}

TEST_F(TracerEnv, StartWithOp)
{
    Tracer::Span * span = tracer->start("hello");
    ASSERT_TRUE(span);

    int rc = 0;

    rc = span->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = span->setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = span->setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    std::vector<std::string> vals;
    rc = span->getBaggage("apple", &vals);

    ASSERT_EQ(0, rc);
    ASSERT_EQ(1u, vals.size());
    ASSERT_EQ("banana", vals[0]);

    std::string val;
    rc = span->getBaggage("apple", &val);

    ASSERT_EQ(0, rc);
    ASSERT_EQ("banana", val);

    tracer->cleanup(span);
}

TEST_F(TracerEnv, InjectText)
{
    TestTextWriter writer;

    Tracer::Span* span(tracer->start("op"));
    ASSERT_TRUE(span);

    span->setBaggage("animal", "tiger");
    span->setBaggage("animal", "cat");

    const Tracer::SpanContext * context = span->context();

    int rc = tracer->inject(&writer, *context);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, writer.pairs.size());

    ASSERT_EQ(writer.pairs[0].m_key, "animal");
    ASSERT_EQ(writer.pairs[1].m_key, "animal");

    ASSERT_TRUE(writer.pairs[0].m_value == "cat" || writer.pairs[0].m_value == "tiger");
    ASSERT_TRUE(writer.pairs[1].m_value == "cat" || writer.pairs[1].m_value == "tiger");

    tracer->cleanup(span);
    tracer->cleanup(context);
}

TEST_F(TracerEnv, SpanInjectText)
{
    TestTextWriter writer;

    Tracer::Span* span(tracer->start("op"));
    ASSERT_TRUE(span);

    span->setBaggage("animal", "tiger");
    span->setBaggage("animal", "cat");

    int rc = tracer->inject(&writer, *span);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, writer.pairs.size());

    ASSERT_EQ(writer.pairs[0].m_key, "animal");
    ASSERT_EQ(writer.pairs[1].m_key, "animal");

    ASSERT_TRUE(writer.pairs[0].m_value == "cat" || writer.pairs[0].m_value == "tiger");
    ASSERT_TRUE(writer.pairs[1].m_value == "cat" || writer.pairs[1].m_value == "tiger");

    tracer->cleanup(span);
}

TEST_F(TracerEnv, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    const Tracer::SpanContext* context(tracer->extract(reader));
    ASSERT_TRUE(context);

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for (Tracer::SpanContext::BaggageIterator it = context->baggageBegin();
         it != context->baggageEnd();
         ++it)
    {
        ASSERT_STREQ(names[index], it.ref().key());
        ASSERT_STREQ(values[index], it.ref().value());
        ++index;
    }

    tracer->cleanup(context);
}

TEST_F(TracerEnv, InjectBinary)
{
    TestBinaryWriter writer;

    Tracer::Span* span(tracer->start("op"));
    ASSERT_TRUE(span);

    const Tracer::SpanContext * context = span->context();

    int rc = tracer->inject(&writer, *context);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(0xdeadbeef, writer.m_raw);

    tracer->cleanup(context);
    tracer->cleanup(span);
}

TEST_F(TracerEnv, SpanInjectBinary)
{
    TestBinaryWriter writer;

    Tracer::Span* span(tracer->start("op"));
    ASSERT_TRUE(span);

    int rc = tracer->inject(&writer, *span);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(0xdeadbeef, writer.m_raw);

    tracer->cleanup(span);
}

TEST_F(TracerEnv, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;

    const Tracer::SpanContext* context(tracer->extract(reader));
    ASSERT_TRUE(context);
    tracer->cleanup(context);
}


TEST_F(TracerEnv, InjectExplicit)
{
    TestWriter w;

    Tracer::Span* span(tracer->start("span"));
    ASSERT_TRUE(span);

    span->setBaggage("animal", "tiger");
    span->setBaggage("fruit", "apple");

    const Tracer::SpanContext * const context = span->context();

    int rc = tracer->inject(&w, *context);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, w.carrier.size());

    TestBaggageContainer::const_iterator cit = w.carrier.find("animal");

    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("tiger", cit->second);

    cit = w.carrier.find("fruit");
    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("apple", cit->second);

    tracer->cleanup(span);
    tracer->cleanup(context);
}

TEST_F(TracerEnv, SpanInjectExplicit)
{
    TestWriter w;

    Tracer::Span* span(tracer->start("span"));
    ASSERT_TRUE(span);

    span->setBaggage("animal", "tiger");
    span->setBaggage("fruit", "apple");

    int rc = tracer->inject(&w, *span);
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, w.carrier.size());

    TestBaggageContainer::const_iterator cit = w.carrier.find("animal");

    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("tiger", cit->second);

    cit = w.carrier.find("fruit");
    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("apple", cit->second);

    tracer->cleanup(span);
}

TEST_F(TracerEnv, ExtractExplicit)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    const Tracer::SpanContext* context(tracer->extract(reader));
    ASSERT_TRUE(context);
    tracer->cleanup(context);
}

TEST_F(TracerEnv, StartWithOptions)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    const Tracer::SpanContext* otherContext(tracer->extract(reader));
    ASSERT_TRUE(otherContext);

    Tracer::SpanOptions* opts(tracer->makeSpanOptions());
    ASSERT_TRUE(opts);

    opts->setOperation("test");
    opts->setStartTime(12414);
    opts->setReference(SpanReferenceType::e_ChildOf, *otherContext);
    opts->setTag("hello", "world");

    Tracer::Span* span(tracer->start(*opts));
    ASSERT_TRUE(span);

    tracer->cleanup(otherContext);
    tracer->cleanup(opts);
    tracer->cleanup(span);
}
