#include "unittest.h"

#include <opentracing/stringref.h> // test include guard
#include <opentracing/stringref.h>

TEST(StringRef, Empty)
{
    StringRef ref;

    ASSERT_EQ(0, ref.data());
    ASSERT_EQ(0, ref.length());
}

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

TEST(StringRef, Reset)
{
    StringRef ref;
    ASSERT_EQ(0, ref.data());
    ASSERT_EQ(0, ref.length());

    ref.reset("hello world");

    ASSERT_STREQ("hello world", ref);
    ASSERT_STREQ("hello world", ref.data());
    ASSERT_EQ(std::strlen("hello world"), ref.length());

    const char arr[] = "hello world 1";

    ref.reset(arr);
    ASSERT_EQ(arr, ref);
    ASSERT_EQ(arr, ref.data());
    ASSERT_EQ(std::strlen(arr), ref.length());
    ASSERT_STREQ(arr, ref);
    ASSERT_STREQ(arr, ref.data());

    const char * p = "hello world 2";

    ref.reset(p);
    ASSERT_EQ(p, ref);
    ASSERT_EQ(p, ref.data());
    ASSERT_EQ(std::strlen(p), ref.length());
    ASSERT_STREQ(p, ref);
    ASSERT_STREQ(p, ref.data());

    const std::string s = "hello world 3";

    ref.reset(s);
    ASSERT_EQ(s.data(), ref);
    ASSERT_EQ(s.data(), ref.data());
    ASSERT_EQ(s.length(), ref.length());
    ASSERT_STREQ(s.c_str(), ref);
    ASSERT_STREQ(s.c_str(), ref.data());

    ref.reset(p, std::strlen(p));
    ASSERT_EQ(p, ref);
    ASSERT_EQ(p, ref.data());
    ASSERT_EQ(std::strlen(p), ref.length());
    ASSERT_STREQ(p, ref);
    ASSERT_STREQ(p, ref.data());
}
