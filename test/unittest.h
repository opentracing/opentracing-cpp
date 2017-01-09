#ifndef INCLUDED_OPENTRACING_UNITTEST_H
#define INCLUDED_OPENTRACING_UNITTEST_H

#include <gtest/gtest.h>
#include <opentracing/config.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include <cstring>
#include <cwchar>
#include <string>

namespace opentracing{
inline void
test_widen(std::wstring* const dest, const std::string& src)
{
    dest->resize(src.size());
    std::mbstowcs(&(*dest)[0], src.c_str(), src.size());
}
}

using namespace opentracing;

#endif
