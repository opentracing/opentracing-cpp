#include "unittest.h"

#include <opentracing/stringref.h> // test include guard
#include <opentracing/stringref.h>

TEST(StringRef, CString)
{
    const char * val = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.data());
    ASSERT_EQ(std::strlen(val), ref.length());
}

TEST(StringRef, CStringArray)
{
    const char val[] = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.data());
    ASSERT_EQ(std::strlen(val), ref.length());
}

TEST(StringRef, StdString)
{
    const std::string val = "hello world";

    StringRef ref(val);

    ASSERT_EQ(val, ref.data());
    ASSERT_EQ(val.length(), ref.length());
}

TEST(StringRef, Copy)
{
    const std::string val = "hello world";

    StringRef ref(val);
    StringRef cpy(ref);

    ASSERT_EQ(val, cpy.data());
    ASSERT_EQ(val.length(), cpy.length());
}
