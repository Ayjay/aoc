#include "aoc2024.h"

#include <bitset>
#include <cmath>
#include <list>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day9 {
using result_type = long long;
const auto test_data = std::vector{std::tuple<std::string_view,
                                              std::optional<result_type>,
                                              std::optional<result_type>>{
    R"(2333133121414131402)", 1928, 2858}};

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
    return (n * (n + 1)) / 2;
}

auto checksum_block(block_t block, result_type index) {
    const auto [id, count] = block;
    if (id)
        return static_cast<result_type>(
            *id * (count * index + triangular_number(count - 1)));
    else
        return 0ll;
}

TEST_CASE("checksum_block", "[day9]") {
    REQUIRE(checksum_block({1, 2}, 0) == 1);
    REQUIRE(checksum_block({id_t{}, 2}, 0) == 0);
}

auto checksum(auto range) {
    const auto op = [](auto acc, auto block) {
        const auto [total, pos] = acc;
        return std::tuple{total + checksum_block(block, pos),
                          pos + block.count};
    };
    return std::get<0>(ranges::accumulate(range, std::tuple{0ll, 0ll}, op));
}

TEST_CASE("checksum1", "[day9]") {
    auto blocks = std::vector<block_t>{{1, 2}, {id_t{}, 2}, {2, 2}};
    REQUIRE(checksum(blocks) == 19);
}

TEST_CASE("checksum2", "[day9]") {
    auto blocks = std::vector<block_t>{{0, 2}, {9, 2}, {8, 1}};
    REQUIRE(checksum(blocks) == 77);
}

std::vector<block_t> rearrange_a(std::vector<block_t> blocks) {
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
            free_block =
                blocks.insert(free_block, {file_block->id, file_count});
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

TEST_CASE("rearrange_a", "[day9]") {
    REQUIRE(ranges::equal(rearrange_a({{id_t{}, 1}, {0, 1}}),
                          std::array{block_t{0, 1}, block_t{id_t{}, 1}}));
}

auto run_a(std::string_view s) {
    return checksum(rearrange_a(parse(s)));
}

auto make_next(auto it) {
    return ++it;
}
auto make_prev(auto it) {
    return --it;
}

std::vector<block_t> rearrange_b(std::vector<block_t> blocks_) {
    auto blocks = blocks_ | ranges::to<std::list>;
    const auto is_free = [](block_t b) -> bool { return !b.id.has_value(); };
    const auto is_file = [](block_t b) -> bool { return b.id.has_value(); };
    auto file_block = std::find_if(blocks.rbegin(), blocks.rend(), is_file);
    auto last_file_id = *file_block->id;
    const auto next_file = [&] {
        return std::find_if(
            file_block, blocks.rend(),
            [&](const block_t block) { return block.id == last_file_id - 1; });
    };

    for (; file_block != blocks.rend(); file_block = next_file()) {
        last_file_id = *file_block->id;
        auto free_block = std::find_if(
            blocks.begin(), file_block.base(), [&](const block_t block) {
                return !block.id.has_value() and
                       block.count >= file_block->count;
            });
        if (free_block != file_block.base()) {
            free_block->id = file_block->id;
            const auto remaining_space = free_block->count - file_block->count;
            free_block->count = file_block->count;
            if (remaining_space) {
                auto next = make_next(free_block);
                if (is_free(*next)) {
                    next->count += remaining_space;
                } else {
                    blocks.insert(next, {id_t{}, remaining_space});
                }
            }
            auto fwd_it = make_prev(file_block.base());
            auto space_before =
                fwd_it != blocks.begin() and is_free(*make_prev(fwd_it));
            auto space_after = make_next(fwd_it) != blocks.end() and
                               is_free(*make_next(fwd_it));
            if (space_before and space_after) {
                const auto total_space = make_prev(fwd_it)->count +
                                         fwd_it->count +
                                         make_next(fwd_it)->count;
                fwd_it = blocks.erase(make_prev(fwd_it), make_next(fwd_it));
                fwd_it->count = total_space;
            } else if (space_before) {
                make_prev(fwd_it)->count += file_block->count;
                fwd_it = blocks.erase(fwd_it);
            } else if (space_after) {
                make_next(fwd_it)->count += file_block->count;
                fwd_it = blocks.erase(fwd_it);
            } else {
                file_block->id = {};
            }
            file_block = std::make_reverse_iterator(fwd_it);
        }
    }

    return blocks | ranges::to<std::vector>;
}

static auto run_b(std::string_view s) {
    return checksum(rearrange_b(parse(s)));
}

TEST_CASE("day9a", "[day9]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day9b", "[day9]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day9

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