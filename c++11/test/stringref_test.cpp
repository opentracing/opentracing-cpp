#include <opentracing/stringref.h>  // test include guard
#include <opentracing/stringref.h>
#include <cassert>

using namespace opentracing;

static void test_empty() {
  StringRef ref;
  assert(0 == ref.data());
  assert(0 == ref.length());
}

static void test_cstring() {
  const char* val = "hello world";

  StringRef ref(val);

  assert(val == ref.data());
  assert(std::strlen(val) == ref.length());
}

static void test_std_string() {
  const std::string val = "hello world";

  StringRef ref(val);

  assert(val == ref.data());
  assert(val.length() == ref.length());
}

static void test_copy() {
  const std::string val = "hello world";

  StringRef ref(val);
  StringRef cpy(ref);

  assert(val == cpy.data());
  assert(val.length() == cpy.length());
}

int main() {
  test_empty();
  test_cstring();
  test_std_string();
  test_copy();
  return 0;
}
