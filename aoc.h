#pragma once

#include <string>
#include <fmt/core.h>
#include <sstream>
#include <fstream>
#include <charconv>
#include <exception>

#include <string_view>
using namespace std::string_view_literals;

#include <cassert>
#include <algorithm>
#include <numeric>
#include <utility>
#include <type_traits>

#include <ranges>
namespace sr = std::ranges;
namespace sv = std::views;

#include <range/v3/core.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/numeric.hpp>
#include <range/v3/view.hpp>
#include <range/v3/range/conversion.hpp>
namespace rv = ranges::view;

#include <boost/hana/core.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/concept.hpp>
#include <boost/hana/ext/std.hpp>
namespace hana = boost::hana;

struct reduce_t {
    auto operator()(auto&& r, auto&& op) const {
        return ranges::accumulate(rv::tail(r), *ranges::begin(r), std::forward<decltype(op)>(op));
    }
    auto operator()(auto&& r) const {
        return (*this)(std::forward<decltype(r)>(r), std::plus{});
    }
};
inline reduce_t reduce{};

inline auto to_int = [](auto s) {
    int i;
    auto [ptr,ec] = std::from_chars(s.data(), s.data() + s.size(), i);
    if (ec != std::errc{}) {
        throw std::exception("bad format");
    }
    return i;
};

inline std::string get_input(int day) {
    auto f = std::ifstream{fmt::format("input{}.txt", day)};
    auto buffer = std::stringstream{};
    buffer << f.rdbuf();
    auto input = buffer.str();
    if (input.back() == '\n')
        input.pop_back();
    return input;
}

inline auto get_lines(std::string_view& s) {
    return s | sv::split("\n"sv) | sv::transform([](auto a) { return std::string_view{ begin(a), end(a) }; });
}

inline void run(auto a_fn, auto b_fn, auto expected_a, auto expected_b, std::string_view test_data, std::string_view input_data) {
    fmt::print("Test A: {}\n", a_fn(test_data) == expected_a ? "pass" : "fail");
    fmt::print("Test B: {}\n", b_fn(test_data) == expected_b ? "pass" : "fail");
    fmt::print("Part A: {}\n", a_fn(input_data));
    fmt::print("Part B: {}\n", b_fn(input_data));
}
