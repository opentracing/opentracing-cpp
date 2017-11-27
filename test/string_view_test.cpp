#include "assert.h"

#include <opentracing/string_view.h>  // test include guard
#include <opentracing/string_view.h>

using namespace opentracing;

static void test_empty() {
  string_view ref;
  ASSERT(0 == ref.data());
  ASSERT(0 == ref.length());
}

static void test_cstring() {
  const char* val = "hello world";

  string_view ref(val);

  ASSERT(val == ref.data());
  ASSERT(std::strlen(val) == ref.length());
}

static void test_std_string() {
  const std::string val = "hello world";

  string_view ref(val);

  ASSERT(val == ref.data());
  ASSERT(val.length() == ref.length());
}

static void test_copy() {
  const std::string val = "hello world";

  string_view ref(val);
  string_view cpy(ref);

  ASSERT(val == cpy.data());
  ASSERT(val.length() == cpy.length());
}

int main() {
  test_empty();
  test_cstring();
  test_std_string();
  test_copy();
  return 0;
}
