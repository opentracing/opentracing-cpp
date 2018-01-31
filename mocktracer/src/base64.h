#ifndef OPENTRACING_MOCKTRACER_BASE64_H
#define OPENTRACING_MOCKTRACER_BASE64_H

#include <opentracing/version.h>
#include <cstdint>
#include <string>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
class Base64 {
 public:
  /**
   * Base64 encode an input char buffer with a given length.
   * @param input char array to encode.
   * @param length of the input array.
   */
  static std::string encode(const char* input, uint64_t length);

  /**
   * Base64 decode an input string.
   * @param input char array to decode.
   * @param length of the input array.
   *
   * Note, decoded string may contain '\0' at any position, it should be treated
   * as a sequence of bytes.
   */
  static std::string decode(const char* input, size_t length);

 private:
  /**
   * Helper method for encoding. This is used to encode all of the characters
   * from the input string.
   */
  static void encodeBase(const uint8_t cur_char, uint64_t pos, uint8_t& next_c,
                         std::string& ret);

  /**
   * Encode last characters. It appends '=' chars to the ret if input
   * string length is not divisible by 3.
   */
  static void encodeLast(uint64_t pos, uint8_t last_char, std::string& ret);
};
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_BASE64_H
