#ifndef INCLUDED_OPENTRACING_TEST_CARRIERS_H
#define INCLUDED_OPENTRACING_TEST_CARRIERS_H

#include "unittest.h"

#include <opentracing/carriers.h>  // test include guard
#include <opentracing/carriers.h>

#include "spancontext.h"

struct TestTextWriter : public GenericTextWriter<TestTextWriter> {
    int
    injectImp(const std::vector<TextMapPair>& textmap)
    {
        pairs = textmap;
        return 0;
    }

    std::vector<TextMapPair> pairs;
};

struct TestTextReader : public GenericTextReader<TestTextReader> {
    int
    extractImp(std::vector<TextMapPair>* const textmap) const
    {
        *textmap = pairs;
        return 0;
    }

    std::vector<TextMapPair> pairs;
};

struct TestBinaryWriter : public GenericBinaryWriter<TestBinaryWriter> {
    int
    injectImp(const void* buf, const size_t len)
    {
        if (len > sizeof(m_raw))
        {
            return 1;
        }

        std::memcpy(&m_raw, buf, len);
        return 0;
    }

    int32_t m_raw;
};

struct TestBinaryReader : public GenericBinaryReader<TestBinaryReader> {
    int
    extractImp(void* const buf, size_t* const written, const size_t len) const
    {
        if (sizeof(m_raw) > len)
        {
            return 1;
        }

        std::memcpy(buf, &m_raw, sizeof(m_raw));
        *written = sizeof(m_raw);
        return 0;
    }

    int32_t m_raw;
};

struct TestWriter : public GenericWriter<TestWriter, TestContextImpl>
{
    int injectImp(const TestContextImpl& context)
    {
        carrier = context.baggageMap();
        return 0;
    }

    std::map<std::string, std::string> carrier;
};

struct TestReader : public GenericReader<TestReader, TestContextImpl>
{
    int extractImp(TestContextImpl * const context) const
    {
        context->baggageMap() = carrier;
        return 0;
    }

    std::map<std::string, std::string> carrier;
};

typedef GenericTextWriter<TestTextWriter>     TextWriter;
typedef GenericTextReader<TestTextReader>     TextReader;
typedef GenericBinaryWriter<TestBinaryWriter> BinaryWriter;
typedef GenericBinaryReader<TestBinaryReader> BinaryReader;
typedef GenericWriter<TestWriter, TestContextImpl> ExplicitWriter;
typedef GenericReader<TestReader, TestContextImpl> ExplicitReader;

#endif
