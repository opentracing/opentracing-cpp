#include "unittest.h"
#include "spancontext.h"

#include <opentracing/spancontext.h>  // test include guard
#include <opentracing/spancontext.h>

TEST(GenericSpanContext, Baggage)
{
    TestContextImpl imp;
    TestContext&    t = imp;

    int rc = t.setBaggage("hello", "world");

    StringRef ref;
    rc = t.getBaggage(&ref, "hello");

    ASSERT_EQ(0, rc);
    ASSERT_STREQ("world", ref.data());

    rc = t.getBaggage(&ref, "unknown");
    ASSERT_NE(0, rc);

    ASSERT_TRUE(t.begin() != t.end());

    TestContext::const_iterator it = t.begin();

    ASSERT_STREQ("hello", it->key().data());
    ASSERT_STREQ("world", it->value().data());

    ++it;

    ASSERT_TRUE(it == t.end());
}

TEST(GenericSpanContext, CopyConstructor)
{
    TestContextImpl impl;
    TestContext& t = impl;

    int rc = t.setBaggage("hello", "world");
    ASSERT_EQ(0, rc);

    TestContextImpl implCopy = impl;
    TestContext& tc = implCopy;

    StringRef ref;
    rc = tc.getBaggage(&ref, "hello");

    ASSERT_EQ(0, rc);
    ASSERT_STREQ("world", ref);
}
