#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <cmath>
#include <list>
#include <unordered_map>
#include <iostream>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day14 {
using i64 = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>
{R"(p=0,4 v=3,-3
p=6,3 v=-1,-3
p=10,3 v=-1,2
p=2,0 v=2,-1
p=0,0 v=1,3
p=3,0 v=-2,-2
p=7,6 v=-1,-3
p=3,0 v=-1,-2
p=9,3 v=2,3
p=7,3 v=-1,2
p=2,4 v=2,-3
p=9,5 v=-3,-3)", {}, {}}
};

using namespace hana::literals; 
using namespace grid;

auto parse(std::string_view s) {
    return *bp::parse(s, *("p=" > bp::long_long > ',' > bp::long_long > " v=" > bp::long_long > ',' > bp::long_long > -bp::eol));
}

enum quad {
    top_left,
    bottom_left,
    top_right,
    bottom_right
};

struct floor_t {
    i64 width;
    i64 height;

    auto get_position(auto robot, i64 time) const {
        const auto [pos_x,pos_y,vel_x,vel_y] = robot;
        return std::tuple{
            (pos_x + (vel_x >= 0 ? vel_x : width + vel_x)*time) % width,
            (pos_y + (vel_y >= 0 ? vel_y : height + vel_y)*time) % height};
    }

    std::optional<int> get_quadrant(auto robot, i64 time) const {
        const auto [pos_x,pos_y,vel_x,vel_y] = robot;
        const auto [final_pos_x,final_pos_y] = get_position(robot, time);
        if (final_pos_x == width / 2 or final_pos_y == height / 2)
            return {};
        if (final_pos_x < width/2)
            if (final_pos_y < height/2)
                return top_left;
            else
                return bottom_left;
        else if (final_pos_y < height/2)
                return top_right;
            else
                return bottom_right;
    }

    auto simulate(const auto& robots, i64 time) const {
        auto quadrant_count = std::array<i64,4>{};
        for (auto robot : robots) {
            if (auto quadrant = get_quadrant(robot, time); quadrant) {
                ++quadrant_count[*quadrant];
            }
        }
        return ranges::accumulate(quadrant_count, 1, std::multiplies{});
    }

    auto get_tiles(const auto& robots, i64 time) const {
        auto tiles = std::map<std::tuple<i64,i64>,int>{};
        for (auto r : robots) {
            ++tiles[get_position(r, time)];
        }
        return tiles;
    }

    void render(const auto& robots, i64 time) const {
        auto tiles = get_tiles(robots, time);
        for (auto y = 0; y < height; ++y) {
            for (auto x = 0; x < width; ++x) {
                auto count = tiles[{x,y}];
                if (count > 0)
                    std::cout << count;
                else
                    std::cout << '.';
            }
            std::cout << '\n';
        }
        std::cout << std::flush;
    }
};

TEST_CASE("get_position", "[day14]") {
    auto robot = parse("p=2,4 v=2,-3").front();
    auto floor = floor_t{11,7};
    REQUIRE(floor.get_position(robot, 0) == std::tuple{2,4});
    REQUIRE(floor.get_position(robot, 1) == std::tuple{4,1});
    REQUIRE(floor.get_position(robot, 2) == std::tuple{6,5});
    REQUIRE(floor.get_position(robot, 3) == std::tuple{8,2});
    REQUIRE(floor.get_position(robot, 4) == std::tuple{10,6});
    REQUIRE(floor.get_position(robot, 5) == std::tuple{1,3});
}

TEST_CASE("single robot", "[day14]") {
    auto robot = parse("p=2,4 v=2,-3").front();
    auto floor = floor_t{11,7};
    REQUIRE(*floor.get_quadrant(robot, 0) == bottom_left);
    REQUIRE(*floor.get_quadrant(robot, 1) == top_left);
    REQUIRE(*floor.get_quadrant(robot, 2) == bottom_right);
    REQUIRE(*floor.get_quadrant(robot, 3) == top_right);
    REQUIRE(*floor.get_quadrant(robot, 4) == bottom_right);
    REQUIRE(!floor.get_quadrant(robot, 5).has_value());
}

//TEST_CASE("small floor grid", "[day14]") {
//    const auto robots = parse(std::get<0>(test_data.front()));
//    floor_t{11,7}.render(robots, 0);
//    std::cout << "-----------\n";
//    floor_t{11,7}.render(robots, 100);
//}

TEST_CASE("small floor", "[day14]") {
    const auto robots = parse(std::get<0>(test_data.front()));
    REQUIRE(floor_t{11,7}.simulate(robots, 100) == 12);
}

auto run_a(std::string_view s) {
    const auto robots = parse(s);
    return floor_t{101,103}.simulate(robots, 100);
}

auto run_b(std::string_view s) {
    const auto robots = parse(s);
    auto floor = floor_t{101,103};
    for (auto i : rv::iota(1ll)) {
        auto tiles_ = floor.get_tiles(robots, i);
        auto tiles = tiles_ | rv::keys | ranges::to<boost::unordered_set> | ranges::to<std::vector>;
        const auto get_y = [](auto v) { return std::get<1>(v); };
        ranges::sort(tiles, std::less{}, get_y);
        auto rows = tiles | rv::chunk_by([](auto p, auto q) { return std::get<1>(p) == std::get<1>(q); });
        if (ranges::any_of(rows, [](auto subrange) { return ranges::distance(subrange) > 30; })) {
            std::cout << "time=" << i << std::endl;
            floor.render(robots, i);
            char c;
            std::cin >> c;
            if (c == 'q')
                break;
            std::cout << std::endl;
        }
    }
}

}

WEAK void entry() {
    using namespace day14;
    const auto input = get_input(AOC_DAY);
    {
        auto t = SimpleTimer("Part A");
        fmt::println("A: {}", run_a(input));
    }
    {
        run_b(input);
    }
}