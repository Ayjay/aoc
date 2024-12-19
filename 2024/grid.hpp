#pragma once

#include <tuple>
#include <array>
#include <string_view>
#include "aoc2024.h"

namespace grid {
using vector2 = std::tuple<int,int>;

const inline auto up    = vector2{-1, 0};
const inline auto down  = vector2{ 1, 0};
const inline auto left  = vector2{ 0,-1};
const inline auto right = vector2{ 0, 1};
const inline auto directions = std::array{up,down,left,right};
inline vector2 turn_right(vector2 facing) {
    if (facing == up) return right;
    else if (facing == right) return down;
    else if (facing == down) return left;
    else return up;
}
inline vector2 turn_left(vector2 facing) {
    if (facing == up) return left;
    else if (facing == right) return up;
    else if (facing == down) return right;
    else return down;
}
inline vector2 operator+(vector2 a, vector2 b) {
    const auto [a_r,a_c] = a;
    const auto [b_r,b_c] = b;
    return {a_r+b_r,a_c+b_c};
}
inline vector2 operator-(vector2 a, vector2 b) {
    const auto [a_r,a_c] = a;
    const auto [b_r,b_c] = b;
    return {a_r-b_r,a_c-b_c};
}
inline vector2 operator*(vector2 a, auto constant) {
    const auto [a_r,a_c] = a;
    return {a_r*constant,a_c*constant};
}

inline auto indexer(const auto& map) {
    return [&](const vector2 p) {
        const auto [row,col] = p;
        return map[row][col];
    };
}

class grid_t {
public:
    std::vector<std::string> map;
    const int rows;
    const int cols;

    explicit grid_t(std::string_view s) :
        map (get_lines<std::string>(s)),
        rows(map.size()),
        cols(map.front().size())
    {}

    char& get(vector2 p) {
        const auto [row,col] = p;
        return map[row][col];
    }

    char get(vector2 p) const {
        const auto [row,col] = p;
        return map[row][col];
    }

    std::optional<char> checked_get(vector2 p) const {
        const auto [row,col] = p;
        if (row >= 0 and row < rows and col >= 0 and col < cols)
            return get(p);
        else
            return {};
    }

    auto cells() const {
        return rv::cartesian_product(rv::iota(0, rows), rv::iota(0, cols));
    }
};

}