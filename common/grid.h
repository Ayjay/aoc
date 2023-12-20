#pragma once

#include "aoc.h"

#include <tuple>
#include <unordered_map>
#include <utility>

#include <boost/container_hash/hash.hpp>

#include <range/v3/view.hpp>
namespace rv = ranges::views;

using bounds_t = std::tuple<long long, long long>;
using point_t = std::tuple<long long, long long>;
using direction_t = std::tuple<long long, long long>;

constexpr auto north = direction_t{-1, 0};
constexpr auto south = direction_t{ 1, 0};
constexpr auto east  = direction_t{ 0, 1};
constexpr auto west  = direction_t{ 0,-1};
constexpr auto directions = std::array { north, south, east, west };

inline auto opposite(direction_t direction) {
    if (direction == north) return south;
    if (direction == south) return north;
    if (direction == west) return east;
    if (direction == east) return west;
    std::unreachable();
}

inline auto direction_to_index(direction_t direction) {
    if (direction == north) return 0;
    if (direction == south) return 1;
    if (direction == west) return 2;
    if (direction == east) return 3;
    std::unreachable();
}

inline point_t add(point_t x, point_t y) {
    return {std::get<0>(x) + std::get<0>(y), std::get<1>(x) + std::get<1>(y)};
}

inline point_t sub(point_t x, point_t y) {
    return {std::get<0>(x) - std::get<0>(y), std::get<1>(x) - std::get<1>(y)};
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

inline auto get_bounds(const auto& grid) {
    return bounds_t { grid.size(), grid.empty() ? 0 : grid.front().size() };
}

inline auto make_in_bounds(const auto& grid) {
    return [bounds=get_bounds(grid)](point_t p) {
        const auto [max_rows, max_cols] = bounds;
        const auto [row, col] = p;
        return row >= 0 && row < max_rows &&
            col >= 0 && col < max_cols;
    };
}

inline auto parse_grid_to_array(std::string_view s) {
    const auto lines = get_lines(s);
    return std::tuple{ get_bounds(lines), lines };
}
