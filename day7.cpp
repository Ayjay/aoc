#include "aoc.h"

#include <tuple>
#include <unordered_map>
#include <variant>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/regex.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::lit;
using ascii::space;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
    {R"($ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k)", 95437, -1}
};

struct directory_t;

using file_t = std::tuple<int, std::string>;
using directory_entry_t = std::tuple<std::string, std::variant<file_t, directory_t>>;
struct directory_t : std::vector<directory_entry_t> {
    using std::vector<directory_entry_t>::vector;
};

struct file_size_t {
    using kind = boost::vertex_property_tag;
};
const file_size_t file_size;

using file_system = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    boost::property<boost::vertex_name_t, std::string,
    boost::property<file_size_t, int64_t>>
>;

file_system parse(std::string_view s) {
    auto iss = std::istringstream(std::string{ s });
    auto line = std::string{};
    auto match = boost::smatch{};

    auto ret = file_system {1};
    auto cursor = file_system::vertex_descriptor{};
    get(boost::vertex_name, ret)[cursor] = "/";

    std::getline(iss, line); //skip '$ cd /'

    const boost::regex cd(R"(\$ cd (.*))");
    const boost::regex ls(R"(\$ ls)");
    const boost::regex ls_file(R"((\d+) (.*))");
    const boost::regex ls_dir(R"(dir (.*))");

    auto navigate_to_subdir = [&](std::string_view subdir_name) {
        //auto out_edges_its = out_edges(cursor, ret);
        //auto out_edges_range = std::ranges::subrange{ out_edges_its.first, out_edges_its.second };
        //auto get_child = [&](auto edge) {
        //    return target(edge, ret);
        //};
        //auto child_vertices = out_edges_range | sv::transform(get_child);
        //cursor = *std::ranges::find_if(child_vertices, [&](auto v) {
        //    return get(boost::vertex_name, ret)[v] == subdir_name;
        //});
        for (auto out_edges_its = out_edges(cursor, ret); out_edges_its.first != out_edges_its.second; ++out_edges_its.first) {
            auto u = target(*out_edges_its.first, ret);
            if (get(boost::vertex_name, ret)[u] == subdir_name) {
                cursor = u;
                return;
            }
        }
    };

    std::getline(iss, line);
    while (iss) {
        if (boost::regex_match(line, match, cd)) {
            auto dir_name = match[1];
            if (dir_name == "..") {
                auto in_edge = *in_edges(cursor, ret).first;
                cursor = source(in_edge, ret);
            }
            else {
                navigate_to_subdir(dir_name.str());
            }
            std::getline(iss, line);
        }
        else if (boost::regex_match(line, match, ls)) {
            while (std::getline(iss, line) && line[0] != '$') {
                if (boost::regex_match(line, match, ls_file)) {
                    auto v = add_vertex(ret);
                    add_edge(cursor, v, ret);
                    get(boost::vertex_name, ret)[v] = match[2];
                    get(file_size, ret)[v] = to_int(match[1].str());
                }
                else if (boost::regex_match(line, match, ls_dir)) {
                    auto v = add_vertex(ret);
                    add_edge(cursor, v, ret);
                    get(boost::vertex_name, ret)[v] = match[1];
                }
            }
        }
    }
    return ret;
}

void dump(auto& g) {
    using namespace std::string_literals;
    boost::write_graphviz(
        std::cout, g, 
        [&](std::ostream& out, auto v) {
            auto name = get(boost::vertex_name, g)[v];
            auto& size = get(file_size, g)[v];
            if (size > 0) {
                out << fmt::format(R"([label="{}: {}"])", name, size);
            }
            else {
                out << fmt::format(R"([label="{}"])", name);

            }
        }
    );
}

auto run_a(std::string_view s) {
    auto fs = parse(s);
    dump(fs);

    return 0;
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}