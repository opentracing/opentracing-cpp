#include "unittest.h"

#include <opentracing/baggage.h>  // test include guard
#include <opentracing/baggage.h>

#include <list>
#include <map>
#include <string>

TEST(BaggageRef, Constructor)
{
    const char n[] = "hello";
    const char v[] = "world";

    BaggageRef ref(n, v);

    ASSERT_EQ(n, ref.name().data());
    ASSERT_EQ(v, ref.value().data());
}

TEST(BaggageRef, CopyConstructor)
{
    const char n[] = "hello";
    const char v[] = "world";

    BaggageRef ref(n, v);
    BaggageRef cpy(ref);

    ASSERT_EQ(n, cpy.name().data());
    ASSERT_EQ(v, cpy.value().data());
}

TEST(BaggageRef, Assignment)
{
    const char n[] = "hello";
    const char v[] = "world";

    const char n2[] = "hello";
    const char v2[] = "world";

    BaggageRef ref(n, v);
    BaggageRef ref2(n2, v2);

    ref = ref2;

    ASSERT_EQ(n2, ref.name().data());
    ASSERT_EQ(v2, ref.value().data());
}

// BaggageIterator adapter for map<string, string>
struct MapBaggage {
    typedef std::map<std::string, std::string> Map;
    typedef Map::iterator iterator;
    typedef Map::const_iterator const_iterator;

    BaggageRef
    operator()(const Map::const_iterator& iter) const
    {
        return BaggageRef(iter->first, iter->second);
    }
};

TEST(BaggageIterator, MapBaggageAdapter)
{
    MapBaggage::Map m;
    m["animal"] = "dog";
    m["fruit"]  = "apple";
    m["veggie"] = "carrot";

    BaggageIterator<MapBaggage> it(m.begin());
    BaggageIterator<MapBaggage> end(m.end());

    ASSERT_FALSE(end == it);
    BaggageRef ref = *it;

    ASSERT_STREQ("animal", ref.name());
    ASSERT_STREQ("dog", ref.value());

    BaggageIterator<MapBaggage> prev = it++;
    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = *prev;

    ASSERT_STREQ("animal", ref.name());
    ASSERT_STREQ("dog", ref.value());

    ref = *it;

    ASSERT_STREQ("fruit", ref.name());
    ASSERT_STREQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = *it;

    ASSERT_STREQ("veggie", ref.name());
    ASSERT_STREQ("carrot", ref.value());

    ++it;

    ASSERT_TRUE(end == it);
}

TEST(BaggageIterator, EmptyMapBaggage)
{
    MapBaggage::Map m;

    BaggageIterator<MapBaggage> it(m.begin());
    BaggageIterator<MapBaggage> end(m.end());

    ASSERT_TRUE(end == it);
}

TEST(BaggageIterator, ForLoopSyntax)
{
    MapBaggage::Map m;
    m["animal"] = "dog";
    m["fruit"]  = "apple";
    m["veggie"] = "carrot";

    const char * expectedKeys[] = {"animal", "fruit", "veggie"};
    const char * expectedVals[] = {"dog", "apple", "carrot"};
    size_t index = 0;

    for (BaggageIterator<MapBaggage> it = m.begin(), end = m.end(); it != end;
         ++it, ++index)
    {
	// Test the -> syntax works
        ASSERT_STREQ(expectedKeys[index], it->name());
        ASSERT_STREQ(expectedVals[index], it->value());
    }
}

// Adapter for BaggageIterator, wrapping a stl list
struct ListBaggageAdapter {
    struct KVP {
        std::string key;
        std::string val;
    };

    typedef std::list<KVP>       List;
    typedef List::iterator       iterator;
    typedef List::const_iterator const_iterator;

    BaggageRef
    operator()(const const_iterator& iter) const
    {
        return BaggageRef(iter->key, iter->val);
    }
};

TEST(BaggageIterator, ListBaggageAdapter)
{
    ListBaggageAdapter::List seq;

    ListBaggageAdapter::KVP tmp;
    tmp.key = "animal";
    tmp.val = "dog";
    seq.push_back(tmp);

    tmp.key = "fruit";
    tmp.val = "apple";
    seq.push_back(tmp);

    tmp.key = "veggie";
    tmp.val = "carrot";
    seq.push_back(tmp);

    BaggageIterator<ListBaggageAdapter> it(seq.begin());
    BaggageIterator<ListBaggageAdapter> end(seq.end());

    ASSERT_FALSE(end == it);
    BaggageRef ref = *it;

    ASSERT_STREQ("animal", ref.name());
    ASSERT_STREQ("dog", it->value());

    BaggageIterator<ListBaggageAdapter> prev = it++;
    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = *prev;

    ASSERT_STREQ("animal", ref.name());
    ASSERT_STREQ("dog", ref.value());

    ref = *it;

    ASSERT_STREQ("fruit", ref.name());
    ASSERT_STREQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = *it;

    ASSERT_STREQ("veggie", ref.name());
    ASSERT_STREQ("carrot", ref.value());

    ++it;

    ASSERT_TRUE(end == it);
}
