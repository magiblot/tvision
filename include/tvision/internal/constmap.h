#ifndef TVISION_CONSTMAP_H
#define TVISION_CONSTMAP_H

#include <tvision/tv.h>
#include <internal/strings.h>

#include <cstddef>
#include <utility>
#include <algorithm>
#include <unordered_map>

template <class Key, class Value, size_t N=(size_t)-1>
class constexpr_map
{

    Key keys[N];
    Value values[N];

public:

    constexpr
    constexpr_map(const std::pair<Key, Value> (&init) [N]) :
        keys {},
        values {}
    {
        for (size_t i = 0; i < N; ++i) {
            keys[i] = std::move(init[i].first);
            values[i] = std::move(init[i].second);
        }
    }

    constexpr
    Value operator[](const Key &key) const {
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
    constexpr
    static constexpr_map<Key, Value, N>
    from_array(const std::pair<Key, Value> (&init) [N])
    {
        return constexpr_map<Key, Value, N>(init);
    }
};

// Same as unordered_map, but with operator[] const.

template<typename Key, typename Value>
class const_unordered_map_base : public std::unordered_map<Key, Value>
{

    using super = std::unordered_map<Key, Value>;

public:

    using super::super;

    Value operator[](const Key &key) const
    {
        auto it = super::find(key);
        if (it == super::end())
            return {};
        return it->second;
    }

};

template<typename Key, typename Value>
class const_unordered_map : public const_unordered_map_base<Key, Value>
{

    using super = const_unordered_map_base<Key, Value>;

public:

    using super::super;

};

// Overload of const_unordered_map for Key=uint64_t, which provides an additional
// constructor function 'with_string_keys'.
//
// 'with_string_keys' allows using short strings as keys. These strings are reinterpreted
// as integers on-the-fly. For example, "012" becomes (uint64_t) 0x323130.
//
// Complete example:
//
// const auto map = const_unordered_map<uint64_t, bool>::with_string_keys({
//     { "012", true },
// });
//
// assert(map["012"]    == true);
// assert(map[0x323130] == true);
// assert(map["abc"]    == false);

template<typename Value>
class const_unordered_map<uint64_t, Value> : public const_unordered_map_base<uint64_t, Value>
{

    using super = const_unordered_map_base<uint64_t, Value>;

    struct StringAsIntPair : const_unordered_map::super::value_type
    {

        using super = typename const_unordered_map::super::value_type;

        using super::super;

        constexpr StringAsIntPair(TStringView s, const Value &v) :
            super(detail::string_as_int<uint64_t>(s), v)
        {
        }

        constexpr StringAsIntPair(TStringView s, Value &&v) :
            super(detail::string_as_int<uint64_t>(s), std::move(v))
        {
        }

    };

public:

    using super::super;

    static const_unordered_map with_string_keys(std::initializer_list<StringAsIntPair> init)
    {
        return const_unordered_map(
            static_cast<const typename super::value_type *>(init.begin()),
            static_cast<const typename super::value_type *>(init.end())
        );
    }

    Value operator[](TStringView key) const
    {
        return super::operator[](detail::string_as_int<uint64_t>(key));
    }

};

#endif // TVISION_CONSTMAP_H
