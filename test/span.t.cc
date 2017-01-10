#include "unittest.h"
#include "span.h"

TEST(Span, BasicTests)
{
    TestSpanImpl impl;
    TestSpan& t = impl;

    int rc = impl.context().setBaggage("hello", "world");

    {
        // Make sure const references work
        const TestSpan& tc = impl;

        std::vector<std::string> vals;
        rc = tc.context().getBaggage("hello", &vals);

        ASSERT_EQ(0, rc);
        ASSERT_EQ(1u, vals.size());
        ASSERT_EQ("world", vals[0]);

        const TestSpan::SpanContext& cc = tc.context();
        rc  = cc.getBaggage("miss", &vals);
        ASSERT_NE(0, rc);

        TestSpan::SpanContext::BaggageIterator it = cc.baggageBegin();
        ASSERT_FALSE(it == cc.baggageEnd());
    }

    t.finish();
    t.finish(1234567);
}

template<typename T>
class SpanTypeTests : public ::testing::Test {
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

TYPED_TEST_CASE(SpanTypeTests, OverloadedTypes);

TYPED_TEST(SpanTypeTests, TagInterface)
{
    TestSpanImpl impl;
    TestSpan& t = impl;
    int rc = t.tag("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(SpanTypeTests, LogInterface)
{
    TestSpanImpl impl;
    TestSpan& t = impl;
    int rc = t.log("key", TypeParam());
    ASSERT_EQ(0, rc);
}

TYPED_TEST(SpanTypeTests, LogTspInterface)
{
    TestSpanImpl impl;
    TestSpan& t = impl;
    int rc = t.log("key", TypeParam(), 0);
    ASSERT_EQ(0, rc);
}
