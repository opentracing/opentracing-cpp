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
    GlobalTracer::Span* span(GlobalTracer::start("hello"));
    EXPECT_TRUE(span);

    int rc = 0;

    rc = span->log("server", "blahblahblah");
    ASSERT_EQ(0, rc);

    rc = span->setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    rc = span->setBaggage("apple", "banana");
    ASSERT_EQ(0, rc);

    std::string val;
    rc = span->getBaggage("apple", &val);
    ASSERT_NE(0, rc);

    std::vector<std::string> vals;
    rc = span->getBaggage("apple", &vals);
    ASSERT_NE(0, rc);

    GlobalTracer::cleanup(span);
}

TEST_F(NoopTracerEnv, StartWithOpAndParent)
{
    // We create spans, which have contexts or create contexts directly
    // through the 'extract' interface. Either way, clients only deal
    // with the 'GlobalContext' type. Rather than make this simple start
    // test dependent on those other interfaces, I'm going to just pull
    // a context out of thin air to test the interface.

    GlobalTracer::Span* guard(GlobalTracer::start("hello"));
    EXPECT_TRUE(guard);
}

TEST_F(NoopTracerEnv, InjectText)
{
    TestTextWriter writer;

    GlobalTracer::Span* span(GlobalTracer::start("op"));
    ASSERT_TRUE(span);

    span->setBaggage("animal", "tiger");
    span->setBaggage("animal", "cat");

    const GlobalTracer::SpanContext * context = span->context();

    int rc = GlobalTracer::inject(&writer, *context);
    ASSERT_EQ(0, rc);

    GlobalTracer::cleanup(span);
    GlobalTracer::cleanup(context);
}

TEST_F(NoopTracerEnv, ExtractText)
{
    TestTextReader reader;
    reader.pairs.push_back(TextMapPair("animal", "tiger"));
    reader.pairs.push_back(TextMapPair("fruit", "apple"));
    reader.pairs.push_back(TextMapPair("veggie", "carrot"));

    const GlobalTracer::SpanContext* context(GlobalTracer::extract(reader));
    ASSERT_TRUE(context);

    size_t index = 0;

    const char * names[] = {"animal", "fruit", "veggie"};
    const char * values[] = {"tiger", "apple", "carrot"};

    for (GlobalTracer::SpanContext::BaggageIterator it = context->baggageBegin();
         it != context->baggageEnd();
         ++it)
    {
        ASSERT_STREQ(names[index], it.ref().key());
        ASSERT_STREQ(values[index], it.ref().value());
        ++index;
    }

    GlobalTracer::cleanup(context);
}

TEST_F(NoopTracerEnv, InjectBinary)
{
    TestBinaryWriter writer;
    GlobalTracer::Span* span = GlobalTracer::start("op");
    ASSERT_TRUE(span);

    const GlobalTracer::SpanContext * context = span->context();

    int rc = GlobalTracer::inject(&writer, *context);
    ASSERT_EQ(0, rc);
    GlobalTracer::cleanup(span);
    GlobalTracer::cleanup(context);
}

TEST_F(NoopTracerEnv, ExtractBinary)
{
    TestBinaryReader reader;
    reader.m_raw = 0xdeadbeef;
    const GlobalTracer::SpanContext* context(GlobalTracer::extract(reader));
    ASSERT_TRUE(context);
    GlobalTracer::cleanup(context);
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
    GlobalTracer::Span* span(GlobalTracer::start("span"));
    ASSERT_TRUE(span);

    const GlobalTracer::SpanContext * context = span->context();

    int rc = GlobalTracer::inject(&w, *context);
    ASSERT_EQ(0, rc);

    GlobalTracer::cleanup(span);
    GlobalTracer::cleanup(context);
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
    const GlobalTracer::SpanContext* context(GlobalTracer::extract(r));
    ASSERT_TRUE(context);
    GlobalTracer::cleanup(context);
}

TEST_F(NoopTracerEnv, StartWithOpts)
{
    NoopReader r;
    const GlobalTracer::SpanContext* otherContext(GlobalTracer::extract(r));
    ASSERT_TRUE(otherContext);

    GlobalTracer::SpanOptions* opts(GlobalTracer::makeSpanOptions());

    opts->setOperation("hello");
    opts->setStartTime(1251251);
    opts->setReference(SpanReferenceType::e_FollowsFrom, *otherContext);
    opts->setTag("hello", 125);

    GlobalTracer::Span* span(GlobalTracer::start("hello"));
    EXPECT_TRUE(span);

    GlobalTracer::cleanup(span);
    GlobalTracer::cleanup(otherContext);
    GlobalTracer::cleanup(opts);
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
    GlobalTracer::Span* span(GlobalTracer::start("hello world"));
    int rc = span->tag("key", TypeParam());
    ASSERT_EQ(0, rc);

    GlobalTracer::cleanup(span);
}

TYPED_TEST(NoopSpanTypeTests, LogInterface)
{
    GlobalTracer::Span* span(GlobalTracer::start("hello world"));
    int rc = span->log("key", TypeParam());
    ASSERT_EQ(0, rc);

    GlobalTracer::cleanup(span);
}

TYPED_TEST(NoopSpanTypeTests, LogTspInterface)
{
    GlobalTracer::Span* span(GlobalTracer::start("hello world"));
    int rc = span->log("key", TypeParam(), 0);
    ASSERT_EQ(0, rc);

    GlobalTracer::cleanup(span);
}
