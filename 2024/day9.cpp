#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <bitset>
#include <cmath>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day9 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(2333133121414131402)", 1928, {}}
};

using namespace hana::literals; 
using namespace grid; 

using id_t = std::optional<result_type>;
struct block_t {
    id_t id;
    result_type count;
    auto operator<=>(const block_t&) const = default;
};

std::vector<block_t> parse(std::string_view s) {
    std::vector<block_t> ret;
    auto free = false;
    auto id = 0ll;
    for (const auto c : s) {
        const auto count = c - '0';
        if (count != 0)
            ret.push_back({free ? std::nullopt : id_t{id}, count});
        if (!free)
            ++id;
        free = !free;
    }
    return ret;
}

TEST_CASE("parse", "[day9]") {
    const auto blocks = parse("12");
    REQUIRE(blocks.size() == 2);
    REQUIRE(blocks[0].id == 0);
    REQUIRE(blocks[0].count == 1);
    REQUIRE(!blocks[1].id.has_value());
    REQUIRE(blocks[1].count == 2);
}

auto triangular_number(auto n) {
    return (n * (n+1)) / 2;
}

auto checksum_block(block_t block, result_type index) {
    const auto [id,count] = block;
    if (id)
        return static_cast<result_type>(*id * (count*index + triangular_number(count-1)));
    else
        return 0ll;
}

TEST_CASE("checksum_block", "[day9]") {
    REQUIRE(checksum_block({1,2}, 0) == 1);
    REQUIRE(checksum_block({id_t{},2}, 0) == 0);
}

auto checksum(auto range) {
    const auto op = [](auto acc, auto block) {
        const auto [total,pos] = acc;
        return std::tuple{total + checksum_block(block, pos), pos+block.count};
    };
    return std::get<0>(ranges::accumulate(range, std::tuple{0ll,0ll}, op));
}

TEST_CASE("checksum1", "[day9]") {
    auto blocks = std::vector<block_t>{
        {1,2},
        {id_t{},2},
        {2,2}
    };
    REQUIRE(checksum(blocks) == 19);
}

TEST_CASE("checksum2", "[day9]") {
    auto blocks = std::vector<block_t>{
        {0,2},
        {9,2},
        {8,1}
    };
    REQUIRE(checksum(blocks) == 77);
}

std::vector<block_t> rearrange(std::vector<block_t> blocks) {
    const auto is_free = [](block_t b) { return !b.id.has_value(); };
    const auto is_file = [](block_t b) { return b.id.has_value(); };
    auto free_block = blocks.begin();
    auto file_block = blocks.rbegin();
    auto file_count = result_type{};
    const auto advance_free_ptr = [&] {
        while (free_block != blocks.end() and !is_free(*free_block))
            ++free_block;
    };
    const auto advance_file_ptr = [&] {
        while (file_block != blocks.rend() and !is_file(*file_block))
            ++file_block;
        file_count = file_block->count;
    };

    advance_free_ptr();
    advance_file_ptr();

    while (free_block < file_block.base()) {
        if (free_block->count <= file_count) {
            free_block->id = file_block->id;
            file_count -= free_block->count;
            advance_free_ptr();
        } else {
            free_block->count -= file_count;
            const auto file_index = file_block - blocks.rbegin();
            free_block = blocks.insert(free_block, {file_block->id, file_count});
            ++free_block;
            file_block = blocks.rbegin() + file_index;
            file_count = 0;
        }
        if (file_count == 0) {
            file_block->id = {};
            advance_file_ptr();
        }
    }
    file_block->count = file_count;

    return blocks;
}

TEST_CASE("rearrange", "[day9]") {
    REQUIRE(ranges::equal(rearrange({{id_t{},1},{0,1}}), std::array{block_t{0,1},block_t{id_t{},1}}));
}

auto run_a(std::string_view s) {
    return checksum(rearrange(parse(s)));
}

static auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day9a", "[day9]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day9b", "[day9]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day9;
    const auto input = get_input(AOC_DAY);
    {
        auto t = SimpleTimer("Part A");
        fmt::println("A: {}", run_a(input));
    }
    {
        auto t = SimpleTimer("Part B");
        fmt::println("B: {}", run_b(input));
    }
}