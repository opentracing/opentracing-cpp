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
    Tracer::Span * span = Tracer::start("hello");
    ASSERT_TRUE(span);

    int rc = 0;

    rc = span->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = span->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = span->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    std::vector<std::string> vals;
    rc = span->context().getBaggage("apple", &vals);

    ASSERT_EQ(0, rc);
    ASSERT_EQ(1u, vals.size());
    ASSERT_EQ("banana", vals[0]);

    std::string val;
    rc = span->context().getBaggage("apple", &val);

    ASSERT_EQ(0, rc);
    ASSERT_EQ("banana", val);

    Tracer::cleanup(span);
}

TEST_F(TracerEnv, InjectText)
{
    TestTextWriter writer;

    Tracer::Span* span(Tracer::start("op"));
    ASSERT_TRUE(span);

    span->context().setBaggage("animal", "tiger");
    span->context().setBaggage("animal", "cat");

    int rc = Tracer::inject(&writer, span->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, writer.pairs.size());

    ASSERT_EQ(writer.pairs[0].m_name, "animal");
    ASSERT_EQ(writer.pairs[1].m_name, "animal");

    ASSERT_TRUE(writer.pairs[0].m_value == "cat" || writer.pairs[0].m_value == "tiger");
    ASSERT_TRUE(writer.pairs[1].m_value == "cat" || writer.pairs[1].m_value == "tiger");

    Tracer::cleanup(span);
}

TEST_F(TracerEnv, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    const Tracer::SpanContext* context(Tracer::extract(reader));
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

    Tracer::cleanup(context);
}

TEST_F(TracerEnv, InjectBinary)
{
    TestBinaryWriter writer;

    Tracer::Span* span(Tracer::start("op"));
    ASSERT_TRUE(span);

    int rc = Tracer::inject(&writer, span->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(0xdeadbeef, writer.m_raw);
}

TEST_F(TracerEnv, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;

    const Tracer::SpanContext* context(Tracer::extract(reader));
    ASSERT_TRUE(context);

    Tracer::cleanup(context);
}


TEST_F(TracerEnv, InjectExplicit)
{
    TestWriter w;

    Tracer::Span* span(Tracer::start("span"));
    ASSERT_TRUE(span);

    span->context().setBaggage("animal", "tiger");
    span->context().setBaggage("fruit", "apple");

    int rc = Tracer::inject(&w, span->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(2u, w.carrier.size());

    TestBaggageContainer::const_iterator cit = w.carrier.find("animal");

    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("tiger", cit->second);

    cit = w.carrier.find("fruit");
    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("apple", cit->second);

    Tracer::cleanup(span);
}

TEST_F(TracerEnv, ExtractExplicit)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    const Tracer::SpanContext* context(Tracer::extract(reader));
    ASSERT_TRUE(context);

    Tracer::cleanup(context);
}

TEST_F(TracerEnv, StartWithOptions)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));
    const Tracer::SpanContext* otherContext(Tracer::extract(reader));
    ASSERT_TRUE(otherContext);

    Tracer::SpanOptions* opts(Tracer::makeSpanOptions());
    ASSERT_TRUE(opts);

    opts->setOperation("test");
    opts->setStartTime(12414);
    opts->setReference(SpanRelationship::e_ChildOf, *otherContext);

    Tracer::Span* span(Tracer::start(*opts));
    ASSERT_TRUE(span);

    Tracer::cleanup(otherContext);
    Tracer::cleanup(opts);
    Tracer::cleanup(span);
}
