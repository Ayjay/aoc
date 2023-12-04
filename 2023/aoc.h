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
namespace rv = ranges::views;

#include <boost/hana/core.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/concept.hpp>
#include <boost/hana/ext/std.hpp>
namespace hana = boost::hana;

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

struct reduce_t {
    auto operator()(auto&& r, auto&& op) const {
        return ranges::accumulate(rv::tail(r), *ranges::begin(r), std::forward<decltype(op)>(op));
    }
    auto operator()(auto&& r) const {
        return (*this)(std::forward<decltype(r)>(r), std::plus{});
    }
};
inline reduce_t reduce{};

inline auto to_int = [](std::string_view s) {
    int64_t i;
    auto [ptr,ec] = std::from_chars(s.data(), s.data() + s.size(), i);
    if (ec != std::errc{}) {
        throw std::runtime_error{"bad format"};
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

constexpr inline auto get_lines(std::string_view s, const std::string_view pattern = "\n") {
    auto ret = std::vector<std::string_view>{};
    auto prev = s.begin();
    do {
        auto it = std::search(prev, s.end(), pattern.begin(), pattern.end());
        ret.push_back({prev, it});
        if (it == s.end()) {
            break;
        }
        else {
            prev = it;
            ++prev;
        }
    } while (true);
    return ret;
}

inline std::string get_result_string(auto value, auto expected) {
    if (value == expected)
        return "pass";
    return fmt::format("fail ({} != {})", value, expected);
}

inline void run(auto a_fn, auto b_fn, auto test_data, std::string_view input_data) {
    for (auto [test_data, expected_a, expected_b] : test_data) {
        if (std::empty(std::string_view{test_data}))
            continue;
        fmt::println("Test runs:");
        fmt::println("  A: {}", get_result_string(a_fn(test_data), expected_a));
        fmt::println("  B: {}", get_result_string(b_fn(test_data), expected_b));
    }
    fmt::println("Actual");
    fmt::println("  Part A: {}", a_fn(input_data));
    fmt::println("  Part B: {}", b_fn(input_data));
}

inline void run(auto a_fn, auto b_fn, auto expected_a, auto expected_b, std::string_view test_data, std::string_view input_data) {
    run(a_fn, b_fn, std::array{std::tie(test_data, expected_a, expected_b)}, input_data);
}

template<typename ... Ts>
struct overload : Ts ... { 
    using Ts::operator() ...;
};
template<class... Ts> overload(Ts...) -> overload<Ts...>;
