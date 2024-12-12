#pragma once

#include <tuple>

namespace grid {
using vector2 = std::tuple<int,int>;

const inline auto up    = vector2{-1, 0};
const inline auto down  = vector2{ 1, 0};
const inline auto left  = vector2{ 0,-1};
const inline auto right = vector2{ 0, 1};
inline vector2 turn_right(vector2 facing) {
    if (facing == up) return right;
    else if (facing == right) return down;
    else if (facing == down) return left;
    else return up;
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
}