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
#include <map>

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

#define BOOST_PARSER_USE_HANA_TUPLE
#include <boost/parser/parser.hpp>
namespace bp = boost::parser;

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
    long long i;
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

template <class T>
constexpr bool is_optional_v = false;

template <class U>
constexpr bool is_optional_v<std::optional<U>> = true;
template <class U>
constexpr bool is_optional_v<boost::optional<U>> = true;

constexpr bool is_optional(auto x) {
    return is_optional_v<decltype(x)>;
}

void run_test(auto expected, auto fn) {
    if constexpr (is_optional_v<decltype(expected)>) {
        if (expected) fn(*expected);
    } else {
        fn(expected);
    }
}

inline void run(auto a_fn, auto b_fn, auto test_data, std::string_view input_data) {
    assert(!input_data.empty());

    fmt::println("Test runs:");
    for (const auto [run,test_set] : rv::enumerate(test_data)) {
        const auto [test_str, expected_a, expected_b] = test_set;
        fmt::println(" Test case {}:", run+1);
        if (std::empty(std::string_view{test_str}))
            continue;
        run_test(expected_a, 
            [&](auto expected) { fmt::println("  A: {}", get_result_string(a_fn(test_str), expected)); });
        run_test(expected_b, 
            [&](auto expected) { fmt::println("  B: {}", get_result_string(b_fn(test_str), expected)); });
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