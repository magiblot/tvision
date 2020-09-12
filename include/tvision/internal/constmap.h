#ifndef TVISION_CONSTMAP_H
#define TVISION_CONSTMAP_H

#include <cstddef>
#include <utility>

template <class Key, class Value, size_t N=(size_t)-1>
class constexpr_map
{

    Key keys[N];
    Value values[N];

public:

    constexpr constexpr_map(const std::pair<Key, Value> (&init) [N]) :
        keys {},
        values {}
    {
        for (size_t i = 0; i < N; ++i) {
            keys[i] = std::move(init[i].first);
            values[i] = std::move(init[i].second);
        }
    }

    constexpr Value operator[](const Key &key) const {
        for (size_t i = 0; i < N; ++i)
            if (keys[i] == key)
                return values[i];
        return {};
    }

};

template <class Key, class Value>
class constexpr_map<Key, Value, (size_t)-1>
{

    // This is a helper specialization which allows creating constexpr_map without
    // specifying the number of elements manually. For example:
    //     constexpr_map<int, int , 3> foo({{1, 2}, {3, 4}, {5, 6}});
    // Is equivalent to:
    //     auto foo = constexpr_map<int, int>::from_array({{1, 2}, {3, 4}, {5, 6}});

public:

    template<size_t N>
    static constexpr auto from_array(const std::pair<Key, Value> (&init) [N])
    {
        return constexpr_map<Key, Value, N>(init);
    }
};

#endif // TVISION_CONSTMAP_H
