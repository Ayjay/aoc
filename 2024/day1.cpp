//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
//#define BOOST_TEST_MODULE lib_test
#include <boost/test/unit_test.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1)", 21, {}}
};

auto run_a(std::string_view s) {
    return -1;
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
