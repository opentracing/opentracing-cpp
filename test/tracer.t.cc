#include "unittest.h"
#include "tracer.h"

#include "carriers.h"

struct SpanGuard {
    SpanGuard(Tracer::Span* s) : sp(s) {}
    ~SpanGuard() { delete sp; }
    Tracer::Span * sp;
};

struct SpanContextGuard {
    SpanContextGuard(Tracer::SpanContext* s) : sp(s) {}
    ~SpanContextGuard() { delete sp; }
    Tracer::SpanContext * sp;
};

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

    SpanGuard guard(t.start("hello"));
    EXPECT_TRUE(guard.sp);

    int rc = 0;

    rc = guard.sp->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    StringRef ref;
    rc = guard.sp->context().getBaggage(&ref, "apple");

    ASSERT_EQ(0, rc);
    ASSERT_STREQ("banana", ref);
}

TEST(Tracer, StartWithOpAndTsp)
{
    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(t.start("hello", 125125));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, StartWithOpAndParent)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'TestContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    TestContextImpl contextImp;
    TestContext& context = contextImp;

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(t.start("hello", context));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, StartWithOpAndParentAndTsp)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'TestContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    TestContextImpl contextImp;
    TestContext& context = contextImp;

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(t.start("hello", context, 125125));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, StartWithOpAndMultipleParents)
{
    TestContextImpl contextImp1;
    TestContextImpl contextImp2;
    TestContextImpl contextImp3;

    std::vector<TestContextImpl> contexts(3, TestContextImpl());

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(t.start("hello", contexts.begin(), contexts.end()));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, StartWithOpAndMultipleParentsAndTsp)
{
    TestContextImpl contextImp1;
    TestContextImpl contextImp2;
    TestContextImpl contextImp3;

    std::vector<TestContextImpl> contexts(3, TestContextImpl());

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard guard(t.start("hello", contexts.begin(), contexts.end(), 125125));
    EXPECT_TRUE(guard.sp);
}

TEST(Tracer, InjectText)
{
    TestTextWriter writer;
    TestTracerImpl imp;

    Tracer& t = imp;

    SpanGuard g(t.start("op"));
    ASSERT_TRUE(g.sp);

    g.sp->context().setBaggage("animal", "tiger");
    g.sp->context().setBaggage("animal", "cat");

    int rc = t.inject(&writer, g.sp->context());
    ASSERT_EQ(0, rc);

    ASSERT_EQ(1u, writer.pairs.size());
    // Note there is no requirement that implementation make baggage keys
    // unique, but the test suite does so its nice to verify that.
    ASSERT_EQ(writer.pairs[0].m_name, "animal");
    ASSERT_EQ(writer.pairs[0].m_value, "cat");
}

TEST(Tracer, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    TestTracerImpl imp;

    Tracer& t = imp;

    SpanContextGuard g(t.extract(reader));
    ASSERT_TRUE(g.sp);

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for(Tracer::SpanContext::const_iterator it = g.sp->begin(); it != g.sp->end(); ++it)
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

    SpanGuard g(t.start("op"));
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
    SpanContextGuard g(t.extract(reader));
    ASSERT_TRUE(g.sp);
}

TEST(Tracer, InjectExplicit)
{
    TestWriter     w;
    TestTracerImpl imp;
    Tracer& t = imp;

    SpanGuard g(t.start("span"));
    ASSERT_TRUE(g.sp);

    g.sp->context().setBaggage("animal", "tiger");
    g.sp->context().setBaggage("fruit", "apple");

    int rc = t.inject(&w, g.sp->context());
    ASSERT_EQ(0, rc);

    ASSERT_EQ(2u, w.carrier.size());
    ASSERT_EQ(w.carrier["animal"], "tiger");
    ASSERT_EQ(w.carrier["fruit"], "apple");
}

TEST(Tracer, ExtractExplicit)
{
    TestReader reader;
    reader.carrier["animal"] = "tiger";
    reader.carrier["fruit"]  = "apple";

    TestTracerImpl imp;
    Tracer& t = imp;

    SpanContextGuard g(t.extract(reader));
    ASSERT_TRUE(g.sp);
}
