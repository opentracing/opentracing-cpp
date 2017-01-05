#include "unittest.h"

#include <opentracing/baggage.h> // test include guard
#include <opentracing/baggage.h>

TEST(StringRef, CString)
{
    const char * val = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.m_string);
    ASSERT_EQ(std::strlen(val), ref.m_length);
}

TEST(StringRef, CStringArray)
{
    const char val[] = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.m_string);
    ASSERT_EQ(std::strlen(val), ref.m_length);
}

TEST(StringRef, StdString)
{
    const std::string val = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.m_string);
    ASSERT_EQ(val.length(), ref.m_length);
}

