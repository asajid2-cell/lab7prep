#include "pack.h"

#include <type_traits>

// Everything the pack answers is compile time, so we check it here with static_assert and the build
// just fails if a query comes out wrong.
using Example = Pack<int, double, char>;

static_assert(Example::size == 3);
static_assert(std::is_same_v<Example::type<0>, int>);
static_assert(std::is_same_v<Example::type<1>, double>);
static_assert(std::is_same_v<Example::type<2>, char>);
static_assert(Example::index<double> == 1);
static_assert(Example::index<float> == -1);

// Same queries straight through pack_detail, plus the empty pack and the first occurrence rule.
static_assert(pack_detail::packSize<> == 0);
static_assert(pack_detail::packSize<int, int> == 2);
static_assert(std::is_same_v<pack_detail::packIndexToType<1, char, long, int>, long>);
static_assert(pack_detail::packTypeToIndex<char, int, char, long> == 1);
static_assert(pack_detail::packTypeToIndex<double, int, char> == -1);

// Empty pack, size 0 and nothing found.
static_assert(Pack<>::size == 0);
static_assert(Pack<>::index<int> == -1);

int main() {
    return 0;
}
