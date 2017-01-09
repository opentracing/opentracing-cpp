#include "carriers.h"
#include "unittest.h"

TEST(Carriers, TextMapWriter)
{
    TestTextWriter imp;
    TextWriter&    t = imp;

    std::vector<TextMapPair> v;
    v.push_back(TextMapPair("animal", "dog"));
    v.push_back(TextMapPair("fruit", "apple"));
    v.push_back(TextMapPair("veggie", "carrot"));

    int rc = t.inject(v);

    ASSERT_EQ(0, rc);

    ASSERT_EQ(3u, imp.pairs.size());

    ASSERT_EQ(imp.pairs[0].m_name, "animal");
    ASSERT_EQ(imp.pairs[0].m_value, "dog");

    ASSERT_EQ(imp.pairs[1].m_name, "fruit");
    ASSERT_EQ(imp.pairs[1].m_value, "apple");

    ASSERT_EQ(imp.pairs[2].m_name, "veggie");
    ASSERT_EQ(imp.pairs[2].m_value, "carrot");
}

TEST(Carriers, TextMapReader)
{
    TestTextReader imp;

    std::vector<TextMapPair>& v = imp.pairs;
    v.push_back(TextMapPair("animal", "dog"));
    v.push_back(TextMapPair("fruit", "apple"));
    v.push_back(TextMapPair("veggie", "carrot"));

    TextReader& t = imp;

    std::vector<TextMapPair> o;

    int rc = t.extract(&o);

    ASSERT_EQ(0, rc);

    ASSERT_EQ(3u, o.size());

    ASSERT_EQ(o[0].m_name, "animal");
    ASSERT_EQ(o[0].m_value, "dog");

    ASSERT_EQ(o[1].m_name, "fruit");
    ASSERT_EQ(o[1].m_value, "apple");

    ASSERT_EQ(o[2].m_name, "veggie");
    ASSERT_EQ(o[2].m_value, "carrot");
}

TEST(Carriers, BinaryReader)
{
    TestBinaryReader imp;
    imp.m_raw = 0xdeadbeef;

    BinaryReader& t = imp;

    char smallbuf[3]; // too small for int32_t
    size_t written = 0;

    int rc = t.extract(smallbuf, &written, sizeof(smallbuf));
    ASSERT_NE(0, rc);

    char buf[100];
    rc = t.extract(buf, &written, sizeof(buf));
    ASSERT_EQ(0, rc);
    ASSERT_EQ(written, sizeof(imp.m_raw));

    int32_t out = 0;
    std::memcpy(&out, buf, written);

    ASSERT_EQ(out, imp.m_raw);
}

TEST(Carriers, BinaryWriter)
{
    TestBinaryWriter imp;
    BinaryWriter&    t = imp;

    const int64_t tooBig= 0x00000000deadbeef;

    int rc = t.inject(&tooBig, sizeof(tooBig));
    ASSERT_NE(0, rc);

    const int32_t expected = 0xdeadbeef;

    rc = t.inject(&expected, sizeof(expected));
    ASSERT_EQ(0, rc);

    ASSERT_EQ(expected, imp.m_raw);
}

TEST(Carriers, ExplicitWriter)
{
    TestContextImpl context;
    ASSERT_EQ(0, context.setBaggage("animal", "dog"));
    ASSERT_EQ(0, context.setBaggage("fruit", "apple"));
    ASSERT_EQ(0, context.setBaggage("veggie", "carrot"));

    TestWriter imp;
    ExplicitWriter& t = imp;

    int rc = t.inject(context);
    ASSERT_EQ(0, rc);

    ASSERT_EQ(3u, imp.carrier.size());

    TestBaggageContainer::const_iterator it =
        imp.carrier.find("animal");

    ASSERT_NE(imp.carrier.end(), it);
    ASSERT_EQ("dog", it->second);

    it = imp.carrier.find("fruit");
    ASSERT_NE(imp.carrier.end(), it);
    ASSERT_EQ("apple", it->second);

    it = imp.carrier.find("veggie");
    ASSERT_NE(imp.carrier.end(), it);
    ASSERT_EQ("carrot", it->second);
}

TEST(Carriers, ExplicitReader)
{
    typedef TestBaggageContainer Map;

    TestReader imp;
    imp.carrier.insert(Map::value_type("animal", "dog"));
    imp.carrier.insert(Map::value_type("fruit", "apple"));
    imp.carrier.insert(Map::value_type("veggie", "carrot"));

    ExplicitReader& t = imp;

    TestContextImpl context;
    int rc = t.extract(&context);
    ASSERT_EQ(0, rc);

    TestBaggageContainer& m = context.baggageMap();

    ASSERT_EQ(3u, m.size());

    TestBaggageContainer::const_iterator it = m.find("animal");

    ASSERT_NE(m.end(), it);
    ASSERT_EQ(it->second, "dog");

    it = m.find("fruit");
    ASSERT_NE(m.end(), it);
    ASSERT_EQ(it->second, "apple");

    it = m.find("veggie");
    ASSERT_NE(m.end(), it);
    ASSERT_EQ(it->second, "carrot");
}
