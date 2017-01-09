#include "unittest.h"
#include "spancontext.h"

#include <opentracing/spancontext.h>  // test include guard
#include <opentracing/spancontext.h>

TEST(GenericSpanContext, BaggageCopies)
{
    TestContextImpl imp;
    TestContext&    t = imp;

    int rc = t.setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    TestContext::BaggageValues vals;
    rc = t.getBaggage(&vals, "hello");

    ASSERT_EQ(0, rc);
    ASSERT_EQ(1u, vals.size());
    ASSERT_EQ("world", vals[0]);

    rc = t.getBaggage(&vals, "unknown");
    ASSERT_NE(0, rc);

    ASSERT_TRUE(t.baggageBegin() != t.baggageEnd());

    TestContext::BaggageIterator it = t.baggageBegin();

    Baggage b = it.copy();
    ASSERT_EQ("hello", b.key());
    ASSERT_EQ("world", b.value());

    ++it;

    ASSERT_TRUE(it == t.baggageEnd());

    ASSERT_EQ("hello", b.key());
    ASSERT_EQ("world", b.value());

}

TEST(GenericSpanContext, CopyConstructor)
{
    TestContextImpl impl;
    TestContext& t = impl;

    int rc = t.setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    TestContextImpl implCopy = impl;
    TestContext& tc = implCopy;

    TestContext::BaggageValue val;
    rc = tc.getBaggage(&val, "hello");

    ASSERT_EQ(0, rc);
    ASSERT_EQ("world", val);
}
