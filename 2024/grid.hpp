#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <string_view>
#include <tuple>
#include "aoc2024.h"

namespace grid {
using vector2 = std::tuple<long long, long long>;

constexpr inline auto up = vector2{-1, 0};
constexpr inline auto down = vector2{1, 0};
constexpr inline auto left = vector2{0, -1};
constexpr inline auto right = vector2{0, 1};
constexpr inline std::string_view dir_to_string(vector2 dir) {
    if (dir == up)
        return "up";
    if (dir == down)
        return "down";
    if (dir == left)
        return "left";
    return "right";
}
constexpr inline char dir_to_arrow(vector2 dir) {
    if (dir == up)
        return '^';
    else if (dir == down)
        return 'v';
    else if (dir == left)
        return '<';
    else {
        assert(dir == right);
        return '>';
    }
}

constexpr inline auto directions = std::array{up, down, left, right};
constexpr inline vector2 turn_right(vector2 facing) {
    if (facing == up)
        return right;
    else if (facing == right)
        return down;
    else if (facing == down)
        return left;
    else
        return up;
}
constexpr inline vector2 turn_left(vector2 facing) {
    if (facing == up)
        return left;
    else if (facing == right)
        return up;
    else if (facing == down)
        return right;
    else
        return down;
}
inline vector2 operator+(vector2 a, vector2 b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return {a_r + b_r, a_c + b_c};
}
inline vector2& operator+=(vector2& a, vector2 b) {
    a = a + b;
    return a;
}
inline vector2 operator-(vector2 a, vector2 b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return {a_r - b_r, a_c - b_c};
}
inline vector2& operator-=(vector2& a, vector2 b) {
    a = a - b;
    return a;
}
inline vector2 operator*(vector2 a, auto constant) {
    const auto [a_r, a_c] = a;
    return {a_r * constant, a_c * constant};
}

inline auto distance(vector2 a, vector2 b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return std::sqrt(std::pow(b_r - a_r, 2) + std::pow(b_c - a_c, 2));
}

inline long long manhattan_distance(vector2 a, vector2 b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return std::abs(b_r - a_r) + std::abs(b_c - a_c);
}

inline auto indexer(const auto& map) {
    return [&](const vector2 p) {
        const auto [row, col] = p;
        return map[row][col];
    };
}

class grid_t {
   public:
    std::vector<std::string> map;
    long long rows;
    long long cols;

    explicit grid_t(std::string_view s)
        : map(get_lines<std::string>(s)),
          rows(map.size()),
          cols(map.front().size()) {}

    char& get(vector2 p) {
        const auto [row, col] = p;
        return map[row][col];
    }

    char get(vector2 p) const {
        const auto [row, col] = p;
        return map[row][col];
    }

    bool on_map(vector2 p) const {
        const auto [row, col] = p;
        return row >= 0 and row < rows and col >= 0 and col < cols;
    };

    std::optional<char> checked_get(vector2 p) const {
        if (on_map(p))
            return get(p);
        else
            return {};
    }

    auto cells() const {
        return rv::cartesian_product(rv::iota(0, rows), rv::iota(0, cols));
    }

    auto cell_getter() {
        return [&](vector2 pos) -> char& { return get(pos); };
    }

    auto cell_getter() const {
        return [&](vector2 pos) -> char { return get(pos); };
    }

    vector2 find_single(char c) const {
        const auto cs = cells();
        return *ranges::find(cs, c, cell_getter());
    }
};

}  // namespace grid