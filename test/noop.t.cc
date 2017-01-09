#include "unittest.h"

#include "carriers.h"

#include <opentracing/noop.h>
#include <opentracing/noop.h> // test include guard

// These run the same tests as the tracer.t.cc. The interface should be the
// same, however, there may be some differences in return codes due
// to the differing implementations.

typedef GenericSpanContext<NoopContext, NoopAdapter> GlobalContext;
typedef GenericSpan<NoopSpan, NoopContext, NoopAdapter> GlobalSpan;
typedef GenericSpanOptions<NoopOptions, NoopContext, NoopAdapter> GlobalOptions;
typedef GenericTracer<NoopTracer,
                      NoopSpan,
                      NoopOptions,
                      NoopContext,
                      NoopAdapter>
    GlobalTracer;

typedef GenericSpanGuard<GlobalTracer> SpanGuard;
typedef GenericSpanOptionsGuard<GlobalTracer> SpanOptionsGuard;
typedef GenericSpanContextGuard<GlobalTracer> SpanContextGuard;

TEST(NoopTracer, Intantiation){
    NoopTracer noop;

    GlobalTracer::install(&noop);
    ASSERT_EQ(GlobalTracer::instance(), &noop);

    GlobalTracer::uninstall();
    ASSERT_FALSE(GlobalTracer::instance());
}

TEST(NoopTracer, StartWithOp)
{
    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard guard(&t, t.start("hello"));
    EXPECT_TRUE(guard.sp);

    int rc = 0;

    rc = guard.sp->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = guard.sp->context().setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    GlobalTracer::SpanContext::BaggageValue val;
    rc = guard.sp->context().getBaggage(&val, "apple");

    ASSERT_NE(0, rc);
}

TEST(NoopTracer, StartWithOpAndTsp)
{
    NoopContext otherContextImp;
    GlobalContext& othercontext = otherContextImp;

    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanOptionsGuard optsguard(&t, t.makeSpanOptions());

    optsguard.sp->setOperation("hello");
    optsguard.sp->setStartTime(1251251);
    optsguard.sp->addReference(SpanRelationship::e_FollowsFrom, othercontext);

    SpanGuard guard(&t, t.start("hello"));
    EXPECT_TRUE(guard.sp);
}

/*
TEST(NoopTracer, StartWithOpAndParent)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'GlobalContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    NoopContext contextImp;
    GlobalContext& context = contextImp;

    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard guard(&t, t.start("hello", context));
    EXPECT_TRUE(guard.sp);
}

TEST(NoopTracer, StartWithOpAndParentAndTsp)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'GlobalContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    NoopContext contextImp;
    GlobalContext& context = contextImp;

    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard guard(&t, t.start("hello", context, 125125));
    EXPECT_TRUE(guard.sp);
}

TEST(NoopTracer, StartWithOpAndMultipleParents)
{
    NoopContext contextImp1;
    NoopContext contextImp2;
    NoopContext contextImp3;

    std::vector<NoopContext> contexts(3, NoopContext());

    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard guard(&t, t.start("hello", contexts.begin(), contexts.end()));
    EXPECT_TRUE(guard.sp);
}

TEST(NoopTracer, StartWithOpAndMultipleParentsAndTsp)
{
    NoopContext contextImp1;
    NoopContext contextImp2;
    NoopContext contextImp3;

    std::vector<NoopContext> contexts(3, NoopContext());

    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard guard(&t, t.start("hello", contexts.begin(), contexts.end(), 125125));
    EXPECT_TRUE(guard.sp);
}

TEST(NoopTracer, InjectText)
{
    TestTextWriter writer;
    NoopTracer imp;

    GlobalTracer& t = imp;

    SpanGuard g(&t, t.start("op"));
    ASSERT_TRUE(g.sp);

    g.sp->context().setBaggage("animal", "tiger");
    g.sp->context().setBaggage("animal", "cat");

    int rc = t.inject(&writer, g.sp->context());
    ASSERT_EQ(0, rc);
}

TEST(NoopTracer, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    NoopTracer imp;

    GlobalTracer& t = imp;

    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for (GlobalTracer::SpanContext::const_iterator it = g.sp->begin();
         it != g.sp->end();
         ++it)
    {
        ASSERT_STREQ(names[index], it.ref().key());
        ASSERT_STREQ(values[index], it.ref().value());
        ++index;
    }
}

TEST(NoopTracer, InjectBinary)
{
    TestBinaryWriter writer;
    NoopTracer imp;

    GlobalTracer& t = imp;

    SpanGuard g(&t, t.start("op"));
    ASSERT_TRUE(g.sp);

    int rc = t.inject(&writer, g.sp->context());
    ASSERT_EQ(0, rc);
}

TEST(NoopTracer, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;

    NoopTracer imp;

    GlobalTracer& t = imp;
    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);
}

struct NoopWriter: public GenericWriter<NoopWriter, NoopContext>
{
    int injectImp(const NoopContext&)
    {
        return 0;
    }
};

TEST(NoopTracer, InjectExplicit)
{
    NoopWriter w;
    NoopTracer imp;
    GlobalTracer& t = imp;

    SpanGuard g(&t, t.start("span"));
    ASSERT_TRUE(g.sp);

    int rc = t.inject(&w, g.sp->context());
    ASSERT_EQ(0, rc);
}

struct NoopReader : public GenericReader<NoopReader, NoopContext>
{
    int injectImp(const NoopContext&)
    {
        return 0;
    }
};

TEST(NoopTracer, ExtractExplicit)
{
    NoopReader reader;
    NoopTracer imp;
    GlobalTracer& t = imp;
    SpanContextGuard g(&t, t.extract(reader));
    ASSERT_TRUE(g.sp);
}

template<typename T>
class NoopSpanTypeTests : public ::testing::Test {
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
    NoopSpan impl;
    GlobalSpan& t = impl;
    int rc = t.tag("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(NoopSpanTypeTests, LogInterface)
{
    NoopSpan impl;
    GlobalSpan& t = impl;
    int rc = t.log("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(NoopSpanTypeTests, LogTspInterface)
{
    NoopSpan impl;
    GlobalSpan& t = impl;
    int rc = t.log("key", TypeParam(), 0);
    ASSERT_EQ(0, rc);
}
*/
