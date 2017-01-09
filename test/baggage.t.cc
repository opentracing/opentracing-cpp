#include "unittest.h"

#include <opentracing/baggage.h>  // test include guard
#include <opentracing/baggage.h>

#include <list>
#include <map>
#include <string>

TEST(Baggage, Constructor)
{
    Baggage b;
    ASSERT_EQ("", b.key());
    ASSERT_EQ("", b.value());
}

TEST(Baggage, CopyConstructor)
{
    Baggage b("apple", "banana");
    Baggage cp(b);

    ASSERT_EQ("apple", cp.key());
    ASSERT_EQ("banana", cp.value());
}

TEST(Baggage, Assignment)
{
    Baggage b("apple", "banana");
    Baggage cp;
    cp = b;

    ASSERT_EQ("apple", cp.key());
    ASSERT_EQ("banana", cp.value());
}

TEST(Baggage, Manipulate)
{
    Baggage b("apple", "banana");
    b.key() = "cranberry";
    b.value() = "dates";

    ASSERT_EQ("cranberry", b.key());
    ASSERT_EQ("dates", b.value());
}

TEST(Baggage, ManipulateThroughDeref)
{
    Baggage b("apple", "banana");
    b->key() = "cranberry";
    b->value() = "dates";

    ASSERT_EQ("cranberry", b.key());
    ASSERT_EQ("dates", b.value());
}

TEST(Baggage, ConstBehavior)
{
    const Baggage b("apple", "banana");
    ASSERT_EQ("apple", b.key());
    ASSERT_EQ("banana", b.value());
}

TEST(Baggage, ConstBehaviorThroughDeref)
{
    const Baggage b("apple", "banana");
    ASSERT_EQ("apple", b->key());
    ASSERT_EQ("banana", b->value());
}


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

    Baggage copy(const Map::const_iterator& iter) const
    {
        return Baggage(iter->first, iter->second);
    }
    BaggageRef ref(const Map::const_iterator& iter) const
    {
        return BaggageRef(iter->first, iter->second);
    }
};

TEST(BaggageIterator, CopyAdapter)
{
    MapBaggage::Map m;
    m["animal"] = "dog";
    m["fruit"]  = "apple";
    m["veggie"] = "carrot";

    BaggageIteratorImp<MapBaggage> it(m.begin());
    BaggageIteratorImp<MapBaggage> end(m.end());

    ASSERT_FALSE(end == it);
    Baggage copy = it.copy();;

    ASSERT_EQ("animal", copy.key());
    ASSERT_EQ("dog", copy.value());

    BaggageIteratorImp<MapBaggage> prev = it++;
    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    copy = prev.copy();

    ASSERT_EQ("animal", copy.key());
    ASSERT_EQ("dog", copy.value());

    copy = it.copy();

    ASSERT_EQ("fruit", copy.key());
    ASSERT_EQ("apple", copy.value());

    ++it;
    ASSERT_FALSE(end == it);

    copy = it.copy();

    ASSERT_EQ("veggie", copy.key());
    ASSERT_EQ("carrot", copy.value());

    ++it;

    ASSERT_TRUE(end == it);
}

TEST(BaggageIterator, RefAdapter)
{
    MapBaggage::Map m;
    m["animal"] = "dog";
    m["fruit"]  = "apple";
    m["veggie"] = "carrot";

    BaggageIteratorImp<MapBaggage> it(m.begin());
    BaggageIteratorImp<MapBaggage> end(m.end());

    ASSERT_FALSE(end == it);
    BaggageRef ref = it.ref();

    ASSERT_STREQ("animal", ref.key());
    ASSERT_STREQ("dog", ref.value());

    BaggageIteratorImp<MapBaggage> prev = it++;

    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = prev.ref();

    ASSERT_STREQ("animal", ref.key());
    ASSERT_STREQ("dog", ref.value());

    ref = it.ref();

    ASSERT_STREQ("fruit", ref.key());
    ASSERT_STREQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = it.ref();

    ASSERT_STREQ("veggie", ref.key());
    ASSERT_STREQ("carrot", ref.value());

    ++it;

    ASSERT_TRUE(end == it);
}

TEST(BaggageIterator, Implicit)
{
    MapBaggage::Map m;
    m["animal"] = "dog";
    m["fruit"]  = "apple";
    m["veggie"] = "carrot";

    BaggageIteratorImp<MapBaggage> it(m.begin());
    BaggageIteratorImp<MapBaggage> end(m.end());

    ASSERT_FALSE(end == it);
    BaggageRef ref = *it;

    ASSERT_STREQ("animal", ref.key());
    ASSERT_STREQ("dog", ref.value());

    BaggageIteratorImp<MapBaggage> prev = it++;

    ASSERT_FALSE(prev == end);
    ASSERT_FALSE(it == end);

    ref = *prev;

    ASSERT_STREQ("animal", ref.key());
    ASSERT_STREQ("dog", ref.value());

    ref = *it;

    ASSERT_STREQ("fruit", ref.key());
    ASSERT_STREQ("apple", ref.value());

    ++it;
    ASSERT_FALSE(end == it);

    ref = *it;

    ASSERT_STREQ("veggie", ref.key());
    ASSERT_STREQ("carrot", ref.value());

    ++it;

    ASSERT_TRUE(end == it);
}


TEST(BaggageIterator, EmptyMapBaggage)
{
    MapBaggage::Map m;

    BaggageIteratorImp<MapBaggage> it(m.begin());
    BaggageIteratorImp<MapBaggage> end(m.end());

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

    for (BaggageIteratorImp<MapBaggage> it = m.begin(), end = m.end(); it != end;
         ++it, ++index)
    {
	// Test the -> syntax works
        ASSERT_STREQ(expectedKeys[index], it->key());
        ASSERT_STREQ(expectedVals[index], it->value());
    }
}
