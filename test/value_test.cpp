#include <opentracing/value.h>
#include <cassert>
using namespace opentracing;

static void test_implicit_construction() {
  Value v1(123);
  assert(v1.is<int64_t>());

  Value v2(123u);
  assert(v2.is<uint64_t>());

  Value v3(true);
  assert(v3.is<bool>());

  Value v4(1.0);
  assert(v4.is<double>());
  Value v5(1.0f);
  assert(v5.is<double>());

  Value v6(std::string("abc"));
  assert(v6.is<std::string>());

  Value v7("abc");
  assert(v7.is<const char*>());

  Value v8(Values{Value(1), Value(2)});
  (void)v8;

  Value v9(Dictionary{{"abc", Value(123)}});
  (void)v9;
}

int main() {
  test_implicit_construction();
  return 0;
}
