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

static void test_cstring_array() {
  const char val[] = "hello world";

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

static void test_reset() {
  StringRef ref;
  assert(0 == ref.data());
  assert(0 == ref.length());

  ref.reset("hello world");

  assert(std::string("hello world") == std::string(ref));
  assert(std::string("hello world") == std::string(ref.data()));
  assert(std::strlen("hello world") == ref.length());

  const char arr[] = "hello world 1";

  ref.reset(arr);
  assert(arr == ref);
  assert(arr == ref.data());
  assert(std::strlen(arr) == ref.length());
  assert(std::string(arr) == std::string(ref));
  assert(std::string(arr) == std::string(ref.data()));

  const char* p = "hello world 2";

  ref.reset(p);
  assert(p == ref);
  assert(p == ref.data());
  assert(std::strlen(p) == ref.length());
  assert(std::string(p) == std::string(ref));
  assert(std::string(p) == std::string(ref.data()));

  const std::string s = "hello world 3";

  ref.reset(s);
  assert(s.data() == ref);
  assert(s.data() == ref.data());
  assert(s.length() == ref.length());
  assert(std::string(s.c_str()) == std::string(ref));
  assert(std::string(s.c_str()) == std::string(ref.data()));

  ref.reset(p, std::strlen(p));
  assert(p == ref);
  assert(p == ref.data());
  assert(std::strlen(p) == ref.length());
  assert(std::string(p) == std::string(ref));
  assert(std::string(p) == std::string(ref.data()));
}

int main() {
  test_empty();
  test_cstring();
  test_cstring_array();
  test_std_string();
  test_copy();
  test_reset();
  return 0;
}
