#include "unittest.h"
#include "tracer.h"

#include "carriers.h"

class TracerEnv : public ::testing::Test {
  public:
    virtual void
    SetUp()
    {
        Tracer::install(&imp);
    };

    virtual void
    TearDown()
    {
        Tracer::uninstall();
    };

    TestTracerImpl imp;
};

TEST_F(TracerEnv, SingletonTests)
{
    ASSERT_EQ(&imp, Tracer::instance());
    Tracer::uninstall();
    ASSERT_EQ(0, Tracer::instance());
}

TEST_F(TracerEnv, StartWithOp)
{
    Tracer::SpanGuard guardOrig(Tracer::instance()->start("hello"));
    Tracer::SpanGuard guard(guardOrig.release());

    EXPECT_TRUE(guard.get());

    int rc = 0;

    rc = guard->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = guard->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = guard->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    Tracer::SpanContext::BaggageValues vals;
    rc = guard->context().getBaggage(&vals, "apple");

    ASSERT_EQ(0, rc);
    ASSERT_EQ(1u, vals.size());
    ASSERT_EQ("banana", vals[0]);

    Tracer::SpanContext::BaggageValue val;
    rc = guard->context().getBaggage(&val, "apple");

    ASSERT_EQ(0, rc);
    ASSERT_EQ("banana", val);
}

TEST_F(TracerEnv, InjectText)
{
    TestTextWriter writer;

    Tracer::SpanGuard span(Tracer::instance()->start("op"));
    ASSERT_TRUE(span.get());

    span->context().setBaggage("animal", "tiger");
    span->context().setBaggage("animal", "cat");

    int rc = Tracer::instance()->inject(&writer, span->context());
    ASSERT_EQ(0, rc);

    ASSERT_EQ(2u, writer.pairs.size());

    ASSERT_EQ(writer.pairs[0].m_name, "animal");
    ASSERT_EQ(writer.pairs[1].m_name, "animal");

    ASSERT_TRUE(writer.pairs[0].m_value == "cat" || writer.pairs[0].m_value == "tiger");
    ASSERT_TRUE(writer.pairs[1].m_value == "cat" || writer.pairs[1].m_value == "tiger");
}


TEST_F(TracerEnv, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    Tracer::SpanContextGuard context(Tracer::instance()->extract(reader));
    ASSERT_TRUE(context.get());

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
}

TEST_F(TracerEnv, InjectBinary)
{
    TestBinaryWriter writer;

    Tracer::SpanGuard span(Tracer::instance()->start("op"));
    ASSERT_TRUE(span.get());

    int rc = Tracer::instance()->inject(&writer, span->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(0xdeadbeef, writer.m_raw);
}

TEST_F(TracerEnv, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;

    Tracer::SpanContextGuard context(Tracer::instance()->extract(reader));
    ASSERT_TRUE(context.get());
}


TEST_F(TracerEnv, InjectExplicit)
{
    TestWriter w;

    Tracer::SpanGuard span(Tracer::instance()->start("span"));
    ASSERT_TRUE(span.get());

    span->context().setBaggage("animal", "tiger");
    span->context().setBaggage("fruit", "apple");

    int rc = Tracer::instance()->inject(&w, span->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, w.carrier.size());

    TestBaggageContainer::const_iterator cit = w.carrier.find("animal");

    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("tiger", cit->second);

    cit = w.carrier.find("fruit");
    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("apple", cit->second);
}

TEST_F(TracerEnv, ExtractExplicit)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    Tracer::SpanContextGuard context(Tracer::instance()->extract(reader));
    ASSERT_TRUE(context.get());
}

TEST_F(TracerEnv, StartWithOptions)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    Tracer::SpanContextGuard otherContext(Tracer::instance()->extract(reader));
    ASSERT_TRUE(otherContext.get());

    Tracer::SpanOptionsGuard opts(Tracer::instance()->makeSpanOptions());

    opts->setOperation("test");
    opts->setStartTime(12414);
    opts->addReference(SpanRelationship::e_ChildOf, *otherContext);

    Tracer::SpanGuard span(Tracer::instance()->start(*opts));
    EXPECT_TRUE(span.get());
}
