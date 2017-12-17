// Make sure assert is defined.
#undef NDEBUG

#include "../src/in_memory_stream.h"
#include <opentracing/string_view.h>
#include <cassert>
using namespace opentracing;

static void test_empty_stream() {
  InMemoryStream stream{nullptr, 0};
  std::string s;
  stream >> s;
  assert(s.empty());
  assert(stream.eof());
}

static void test_read_from_nonempty_stream() {
  string_view data = "123";
  InMemoryStream stream{data.data(), data.size()};
  int x;
  stream >> x;
  assert(x == 123);
  assert(stream.eof());
}

int main() {
  test_empty_stream();
  test_read_from_nonempty_stream();
  return 0;
}
