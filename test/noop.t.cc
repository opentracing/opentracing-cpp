#include "unittest.h"

#include "carriers.h"

#include <opentracing/noop.h>
#include <opentracing/noop.h> // test include guard

// These run the same tests as the tracer.t.cc. The interface should be the
// same, however, there may be some differences in return codes due
// to the differing implementations.

typedef GenericTracer<NoopTracer,
                      NoopSpan,
                      NoopOptions,
                      NoopContext,
                      NoopAdapter>
    GlobalTracer;

class NoopTracerEnv : public ::testing::Test {
  public:
    virtual void
    SetUp()
    {
        GlobalTracer::install(&imp);
    };

    virtual void
    TearDown()
    {
        GlobalTracer::uninstall();
    };

    NoopTracer imp;
};

TEST_F(NoopTracerEnv, Intantiation){
    NoopTracer noop;

    GlobalTracer::install(&noop);
    ASSERT_EQ(GlobalTracer::instance(), &noop);

    GlobalTracer::uninstall();
    ASSERT_FALSE(GlobalTracer::instance());
}

TEST_F(NoopTracerEnv, StartWithOp)
{
    GlobalTracer::SpanGuard guard(GlobalTracer::instance()->start("hello"));
    EXPECT_TRUE(guard.get());

    int rc = 0;

    rc = guard->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = guard->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = guard->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    GlobalTracer::SpanContext::BaggageValue val;
    rc = guard->context().getBaggage(&val, "apple");

    ASSERT_NE(0, rc);
}

TEST_F(NoopTracerEnv, StartWithOpAndParent)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'GlobalContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    GlobalTracer::SpanGuard guard(GlobalTracer::instance()->start("hello"));
    EXPECT_TRUE(guard.get());
}

TEST_F(NoopTracerEnv, InjectText)
{
    TestTextWriter writer;

    GlobalTracer::SpanGuard g(GlobalTracer::instance()->start("op"));
    ASSERT_TRUE(g.get());

    g->context().setBaggage("animal", "tiger");
    g->context().setBaggage("animal", "cat");

    int rc = GlobalTracer::instance()->inject(&writer, g->context());
    ASSERT_EQ(0, rc);
}

TEST_F(NoopTracerEnv, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    GlobalTracer::SpanContextGuard g(GlobalTracer::instance()->extract(reader));
    ASSERT_TRUE(g.get());

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for (GlobalTracer::SpanContext::BaggageIterator it = g->baggageBegin();
         it != g->baggageEnd();
         ++it)
    {
        ASSERT_STREQ(names[index], it.ref().key());
        ASSERT_STREQ(values[index], it.ref().value());
        ++index;
    }
}

TEST_F(NoopTracerEnv, InjectBinary)
{
    TestBinaryWriter writer;
    GlobalTracer::SpanGuard g(GlobalTracer::instance()->start("op"));
    ASSERT_TRUE(g.get());
    int rc = GlobalTracer::instance()->inject(&writer, g->context());
    ASSERT_EQ(0, rc);
}

TEST_F(NoopTracerEnv, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;
    GlobalTracer::SpanContextGuard g(GlobalTracer::instance()->extract(reader));
    ASSERT_TRUE(g.get());
}

struct NoopWriter: public GenericWriter<NoopWriter, NoopContext>
{
    int injectImp(const NoopContext&)
    {
        return 0;
    }
};

TEST_F(NoopTracerEnv, InjectExplicit)
{
    NoopWriter w;
    GlobalTracer::SpanGuard g(GlobalTracer::instance()->start("span"));
    ASSERT_TRUE(g.get());
    int rc = GlobalTracer::instance()->inject(&w, g->context());
    ASSERT_EQ(0, rc);
}

struct NoopReader : public GenericReader<NoopReader, NoopContext>
{
    int injectImp(const NoopContext&)
    {
        return 0;
    }
};

TEST_F(NoopTracerEnv, ExtractExplicit)
{
    NoopReader r;
    GlobalTracer::SpanContextGuard g(GlobalTracer::instance()->extract(r));
    ASSERT_TRUE(g.get());
}

TEST_F(NoopTracerEnv, StartWithOpts)
{
    NoopReader r;
    GlobalTracer::SpanContextGuard otherContext(GlobalTracer::instance()->extract(r));
    ASSERT_TRUE(otherContext.get());

    GlobalTracer::SpanOptionsGuard opts(GlobalTracer::instance()->makeSpanOptions());

    opts->setOperation("hello");
    opts->setStartTime(1251251);
    opts->addReference(SpanRelationship::e_FollowsFrom, *otherContext);

    GlobalTracer::SpanGuard guard(GlobalTracer::instance()->start("hello"));
    EXPECT_TRUE(guard.get());
}

template<typename T>
class NoopSpanTypeTests : public ::testing::Test {
  public:
    virtual void
    SetUp()
    {
        GlobalTracer::install(&imp);
    };

    virtual void
    TearDown()
    {
        GlobalTracer::uninstall();
    };

    NoopTracer imp;
};

typedef ::testing::Types<StringRef,
                         bool,
                         float,
                         double,
                         long double,
                         char,
                         signed char,
                         unsigned char,
                         wchar_t,
                         int16_t,
                         int32_t,
                         int64_t,
                         uint16_t,
                         uint32_t,
                         uint64_t>
    OverloadedTypes;

TYPED_TEST_CASE(NoopSpanTypeTests, OverloadedTypes);

TYPED_TEST(NoopSpanTypeTests, TagInterface)
{
    GlobalTracer::SpanGuard span(GlobalTracer::instance()->start("hello world"));
    int rc = span->tag("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(NoopSpanTypeTests, LogInterface)
{
    GlobalTracer::SpanGuard span(GlobalTracer::instance()->start("hello world"));
    int rc = span->log("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(NoopSpanTypeTests, LogTspInterface)
{
    GlobalTracer::SpanGuard span(GlobalTracer::instance()->start("hello world"));
    int rc = span->log("key", TypeParam(), 0);
    ASSERT_EQ(0, rc);
}
