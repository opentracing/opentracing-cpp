#include "unittest.h"
#include "span.h"


TEST(SpanTests, setOperation)
{
    TestSpanImpl impl;
    TestSpan& t = impl;
    const int rc = t.setOperation("hello world");
    ASSERT_EQ(0, rc);
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
