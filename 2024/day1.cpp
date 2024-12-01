//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
//#define BOOST_TEST_MODULE lib_test
#include <boost/test/unit_test.hpp>

#include <boost/hana/tuple.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(3   4
4   3
2   5
1   3
3   9
3   3)", 11, {}}
};

auto parse(std::string_view s) {
    auto vals = *bp::parse(s, *(bp::long_long >> bp::long_long), bp::ws);
    using namespace hana::literals;
    return std::tuple{
        vals | rv::transform([](auto a) { return a[0_c]; }) | ranges::to<std::vector>,
        vals | rv::transform([](auto a) { return a[1_c]; }) | ranges::to<std::vector>
    };
}

auto run_a(std::string_view s) {
    auto [left, right] = parse(s);
    ranges::sort(left);
    ranges::sort(right);
    return reduce(rv::zip_with([](auto a, auto b) { return std::abs(a - b); }, left, right));
}

auto run_b(std::string_view s) {
    return -1;
}

BOOST_AUTO_TEST_CASE(first_test)
{
  int i = 1;
  BOOST_TEST(i);
  BOOST_TEST(i == 2);
}

void entry() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
