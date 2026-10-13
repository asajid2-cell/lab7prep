#ifndef PACK_H
#define PACK_H

#include <type_traits>

// Queries we run on a list of types (a type pack). The answers are all worked out while we compile,
// so we can turn around and feed them back in as template arguments. We keep them in pack_detail so
// Pack, further down, can reuse them instead of repeating the Args... everywhere.
namespace pack_detail {

// How many types we have. sizeof... already knows, so we don't walk the pack ourselves.
template <typename... Args>
constexpr int packSize = sizeof...(Args);

// The queries below all have the tutorial's shape, a primary template plus specializations, where
// the primary is the least specific and only gets picked when nothing better matches.

// Primary template. A valid index always matches one of the specializations below, so we ONLY land
// here when the index ran off the end; we blow up with our own message instead of letting it fall
// through into a confusing "incomplete type" error.
template <int QueryIndex, typename... Args>
struct _packIndexToType {
    static_assert(QueryIndex < 0, "Pack index is out of range.");
};

// Index 0 is the head.
template <typename First, typename... Rest>
struct _packIndexToType<0, First, Rest...> {
    using type = First;
};

// Any other index, drop the head and ask for one less of what is left. The static_assert catches a
// negative index before we recurse our way down to the compiler's limit.
template <int QueryIndex, typename First, typename... Rest>
struct _packIndexToType<QueryIndex, First, Rest...> {
    static_assert(QueryIndex > 0, "Pack index must be nonnegative.");
    using type = typename _packIndexToType<QueryIndex - 1, Rest...>::type;
};

// The QueryIndex'th type of Args..., as an alias since the raw _packIndexToType<...>::type would make
// every caller write typename in front of it.
template <int QueryIndex, typename... Args>
using packIndexToType = typename _packIndexToType<QueryIndex, Args...>::type;

// Tail recursive helper for packTypeToIndex. Accumulated is the index of the next type we haven't
// looked at yet, and the primary template is the base case, an empty pack means we never found it.
template <int Accumulated, typename QueryType, typename... Args>
constexpr int _packTypeToIndex = -1;

// If the head is the type we want we keep the index we've accumulated, otherwise we look in the tail.
// We eat the types head first and only bump the index on the way down, so the first place it matches
// is the one that wins.
template <int Accumulated, typename QueryType, typename First, typename... Rest>
constexpr int _packTypeToIndex<Accumulated, QueryType, First, Rest...> =
    std::is_same_v<QueryType, First> ? Accumulated
                                     : _packTypeToIndex<Accumulated + 1, QueryType, Rest...>;

// First index where QueryType shows up in Args..., or -1 if it never does.
template <typename QueryType, typename... Args>
constexpr int packTypeToIndex = _packTypeToIndex<0, QueryType, Args...>;

}  // namespace pack_detail

// A light wrapper we hang the queries off, so we don't repeat Args... at every call site.
template <typename... Args>
struct Pack {
    // how many types this pack holds
    static constexpr int size = pack_detail::packSize<Args...>;

    // the QueryIndex'th type
    template <int QueryIndex>
    using type = pack_detail::packIndexToType<QueryIndex, Args...>;

    // first index of QueryType, or -1 if it isn't in the pack
    template <typename QueryType>
    static constexpr int index = pack_detail::packTypeToIndex<QueryType, Args...>;
};

#endif  // PACK_H
