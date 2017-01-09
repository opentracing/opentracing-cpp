#include "unittest.h"
#include "tracer.h"

#include "carriers.h"

typedef GenericSpanGuard<Tracer> SpanGuard;
typedef GenericSpanContextGuard<Tracer> SpanContextGuard;

TEST(Tracer, SingletonTests)
{
    TestTracerImpl imp;
    Tracer::install(&imp);
    ASSERT_EQ(&imp, Tracer::instance());

    Tracer::uninstall();
    ASSERT_EQ(0, Tracer::instance());
}

TEST(Tracer, StartWithOp)
{
    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(&t, t.start("hello"));
    EXPECT_TRUE(guard.sp);

    int rc = 0;

    rc = guard.sp->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    Tracer::SpanContext::BaggageValues vals;
    rc = guard.sp->context().getBaggage(&vals, "apple");

    ASSERT_EQ(0, rc);
    ASSERT_EQ(1u, vals.size());
    ASSERT_EQ("banana", vals[0]);

    Tracer::SpanContext::BaggageValue val;
    rc = guard.sp->context().getBaggage(&val, "apple");

    ASSERT_EQ(0, rc);
    ASSERT_EQ("banana", val);
}

TEST(Tracer, StartWithOptions)
{
    TestContextImpl otherContext;

    TestOptionsImpl optsImp;

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanOptions& opts = optsImp;

    opts.setOperation("test");
    opts.setStartTime(12414);
    opts.addReference(SpanRelationship::e_ChildOf, otherContext);

    SpanGuard guard(&t, t.start("hello"));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, InjectText)
{
    TestTextWriter writer;
    TestTracerImpl imp;

    Tracer& t = imp;

    SpanGuard g(&t, t.start("op"));
    ASSERT_TRUE(g.sp);

    g.sp->context().setBaggage("animal", "tiger");
    g.sp->context().setBaggage("animal", "cat");

    int rc = t.inject(&writer, g.sp->context());
    ASSERT_EQ(0, rc);

    ASSERT_EQ(2u, writer.pairs.size());

    ASSERT_EQ(writer.pairs[0].m_name, "animal");
    ASSERT_EQ(writer.pairs[1].m_name, "animal");

    ASSERT_TRUE(writer.pairs[0].m_value == "cat" || writer.pairs[0].m_value == "tiger");
    ASSERT_TRUE(writer.pairs[1].m_value == "cat" || writer.pairs[1].m_value == "tiger");
}

TEST(Tracer, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    TestTracerImpl imp;

    Tracer& t = imp;

    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for (Tracer::SpanContext::BaggageIterator it = g.sp->baggageBegin();
         it != g.sp->baggageEnd();
         ++it)
    {
        ASSERT_STREQ(names[index], it.ref().key());
        ASSERT_STREQ(values[index], it.ref().value());
        ++index;
    }
}

TEST(Tracer, InjectBinary)
{
    TestBinaryWriter writer;
    TestTracerImpl imp;

    Tracer& t = imp;

    SpanGuard g(&t, t.start("op"));
    ASSERT_TRUE(g.sp);

    int rc = t.inject(&writer, g.sp->context());
    ASSERT_EQ(0, rc);
    ASSERT_EQ(0xdeadbeef, writer.m_raw);
}

TEST(Tracer, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;

    TestTracerImpl imp;

    Tracer& t = imp;
    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);
}

TEST(Tracer, InjectExplicit)
{
    TestWriter     w;
    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard g(&t, t.start("span"));
    ASSERT_TRUE(g.sp);

    g.sp->context().setBaggage("animal", "tiger");
    g.sp->context().setBaggage("fruit", "apple");

    int rc = t.inject(&w, g.sp->context());
    ASSERT_EQ(0, rc);

    ASSERT_EQ(2u, w.carrier.size());

    TestBaggageContainer::const_iterator cit = w.carrier.find("animal");

    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("tiger", cit->second);

    cit = w.carrier.find("fruit");
    ASSERT_NE(cit, w.carrier.end());
    ASSERT_EQ("apple", cit->second);
}

TEST(Tracer, ExtractExplicit)
{
    TestReader reader;
    reader.carrier.insert(TestBaggageContainer::value_type("animal", "tiger"));
    reader.carrier.insert(TestBaggageContainer::value_type("fruit", "apple"));

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);
}
