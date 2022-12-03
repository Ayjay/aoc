#pragma once

#include <string>
#include <fmt/core.h>
#include <sstream>
#include <fstream>
#include <charconv>
#include <exception>

#include <string_view>
using namespace std::string_view_literals;

#include <range/v3/core.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/numeric.hpp>
#include <range/v3/view.hpp>
#include <range/v3/range/conversion.hpp>
namespace rv = ranges::view;

#include <cassert>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <utility>
#include <type_traits>
namespace sr = std::ranges;
namespace sv = std::views;

inline auto reduce = [](auto&& r, auto&& op) {
    return ranges::accumulate(rv::tail(r), *ranges::begin(r), std::forward<decltype(op)>(op));
};

inline auto to_int = [](auto s) {
    int i;
    auto [ptr,ec] = std::from_chars(s.data(), s.data() + s.size(), i);
    if (ec != std::errc{}) {
        throw std::exception("bad format");
    }
    return i;
};

std::string get_input(int day) {
    auto f = std::ifstream{fmt::format("input{}.txt", day)};
    auto buffer = std::stringstream{};
    buffer << f.rdbuf();
    return buffer.str();
}

void run(auto a_fn, auto b_fn, auto expected_a, auto expected_b, std::string_view test_data, std::string_view input_data) {
    fmt::print("Test A: {}\n", a_fn(test_data) == expected_a ? "pass" : "fail");
    fmt::print("Test B: {}\n", b_fn(test_data) == expected_b ? "pass" : "fail");
    fmt::print("Part A: {}\n", a_fn(input_data));
    fmt::print("Part B: {}\n", b_fn(input_data));
}
