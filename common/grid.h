#pragma once

#include "aoc.h"

#include <tuple>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>

#include <range/v3/view.hpp>
namespace rv = ranges::views;

using bounds_t = std::tuple<long long, long long>;
using point_t = std::tuple<long long, long long>;
using direction_t = std::tuple<long long, long long>;

const inline auto north = direction_t{-1, 0};
const inline auto south = direction_t{ 1, 0};
const inline auto east  = direction_t{ 0, 1};
const inline auto west  = direction_t{ 0,-1};

inline point_t add(point_t x, point_t y) {
    return {std::get<0>(x) + std::get<0>(y), std::get<1>(x) + std::get<1>(y)};
}

template <template<class...> class Map = std::unordered_map>
inline auto parse_grid(std::string_view s) {
    auto grid = Map<point_t, char, boost::hash<point_t>>{};
    const auto lines = get_lines(s);
    for (const auto [row,line] : rv::enumerate(lines)) {
        for (const auto [col,c] : rv::enumerate(line)) {
            grid.insert({point_t{row,col},c});
        }
    }
    return std::tuple{ bounds_t { lines.size(), lines.front().size() }, grid };
}
