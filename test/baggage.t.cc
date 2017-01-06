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

    ASSERT_EQ(n, ref.key().data());
    ASSERT_EQ(v, ref.value().data());
}

TEST(BaggageRef, CopyConstructor)
{
    const char n[] = "hello";
    const char v[] = "world";

    BaggageRef ref(n, v);
    BaggageRef cpy(ref);

    ASSERT_EQ(n, cpy.key().data());
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

    ASSERT_EQ(n2, ref.key().data());
    ASSERT_EQ(v2, ref.value().data());
}

// BaggageIterator adapter for map<string, string>
class MapBaggage {
  public:
    typedef std::map<std::string, std::string> Map;
    typedef Map::iterator       iterator;
    typedef Map::const_iterator const_iterator;

    Baggage narrow(const Map::const_iterator& iter) const
    {
        return Baggage(iter->first, iter->second);
    }
    BaggageWide wide(const Map::const_iterator& iter) const
    {
        std::wstring first;
        std::wstring second;

        test_widen(&first, iter->first);
        test_widen(&second, iter->second);

        return BaggageWide(first, second);
    }
    BaggageRef ref(const Map::const_iterator& iter) const
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
    Baggage ref = *it;

    ASSERT_EQ("animal", ref.key());
    ASSERT_EQ("dog", ref.value());

    BaggageIterator<MapBaggage> prev = it++;
    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = *prev;

    ASSERT_EQ("animal", ref.key());
    ASSERT_EQ("dog", ref.value());

    ref = *it;

    ASSERT_EQ("fruit", ref.key());
    ASSERT_EQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = *it;

    ASSERT_EQ("veggie", ref.key());
    ASSERT_EQ("carrot", ref.value());

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
        ASSERT_EQ(expectedKeys[index], it->key());
        ASSERT_EQ(expectedVals[index], it->value());
    }
}

// Adapter for BaggageIterator, wrapping a stl list
class ListBaggageAdapter {
  public:
    struct KVP {
        std::string key;
        std::string val;
    };

    typedef std::list<KVP>       List;
    typedef List::iterator       iterator;
    typedef List::const_iterator const_iterator;

    Baggage narrow(const List::const_iterator& iter) const
    {
        return Baggage(iter->key, iter->val);
    }
    BaggageWide wide(const List::const_iterator& iter) const
    {
        std::wstring first;
        std::wstring second;

        test_widen(&first, iter->key);
        test_widen(&second, iter->val);

        return BaggageWide(first, second);
    }
    BaggageRef ref(const List::const_iterator& iter) const
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
    Baggage ref = *it;

    ASSERT_EQ("animal", ref.key());
    ASSERT_EQ("dog", it->value());

    BaggageIterator<ListBaggageAdapter> prev = it++;
    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = *prev;

    ASSERT_EQ("animal", ref.key());
    ASSERT_EQ("dog", ref.value());

    ref = *it;

    ASSERT_EQ("fruit", ref.key());
    ASSERT_EQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = *it;

    ASSERT_EQ("veggie", ref.key());
    ASSERT_EQ("carrot", ref.value());

    ++it;

    ASSERT_TRUE(end == it);
}
