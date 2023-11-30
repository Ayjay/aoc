#include "aoc.h"

#include <tuple>
#include <memory>
#include <vector>

#include <boost/regex.hpp>
#include <boost/variant.hpp>

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
7214296 k)", 95437, 24933642}
};

struct dir; struct file; 
using child_t = boost::variant<std::unique_ptr<dir>, file>;

struct dir {
    std::string name;
    dir* parent;
    std::vector<child_t> children;
};
struct file { 
    std::string name;
    int64_t size; 
};

std::unique_ptr<dir> parse(std::string_view s) {
    auto iss = std::istringstream(std::string{ s });
    auto line = std::string{};
    auto match = boost::smatch{};

    auto ret = std::unique_ptr<dir>{ new dir{ "/", nullptr, {} } };
    auto cursor = ret.get();
    std::getline(iss, line); //skip '$ cd /'

    const boost::regex cd(R"(\$ cd (.*))");
    const boost::regex ls(R"(\$ ls)");
    const boost::regex ls_file(R"((\d+) (.*))");
    const boost::regex ls_dir(R"(dir (.*))");

    auto get_subdir_name = [](auto& a) {
        auto subdir = boost::get<std::unique_ptr<dir>>(&a);
        return subdir ? (*subdir)->name : "";
    };

    auto navigate_to_subdir = [&](std::string_view subdir_name) {
        auto& subdir = *boost::get<std::unique_ptr<dir>>(&*std::ranges::find(cursor->children, subdir_name, get_subdir_name));
        cursor = subdir.get();
    };

    std::getline(iss, line);
    while (iss) {
        if (boost::regex_match(line, match, cd)) {
            auto dir_name = match[1];
            if (dir_name == "..") {
                cursor = cursor->parent;
            }
            else {
                navigate_to_subdir(dir_name.str());
            }
            std::getline(iss, line);
        }
        else if (boost::regex_match(line, match, ls)) {
            while (std::getline(iss, line) && line[0] != '$') {
                if (boost::regex_match(line, match, ls_file)) {
                    cursor->children.push_back(file{ match[2], to_int(match[1].str()) });
                }
                else if (boost::regex_match(line, match, ls_dir)) {
                    cursor->children.push_back(std::unique_ptr<dir>{ new dir{ match[1], cursor, {} } });
                }
            }
        }
    }
    return ret;
}

struct get_weight {
    std::unordered_map<dir*, int64_t> dir_weights;
    int64_t operator()(const file& f) const {
        return f.size;
    }
    int64_t operator()(const std::unique_ptr<dir>& d) {
        auto children_weights = reduce(d->children | sv::transform([&](auto& a) { return boost::apply_visitor(*this, a); }));
        dir_weights[d.get()] = children_weights;
        return children_weights;
    }
};

struct printer {
    std::string prefix = "";
    void operator()(const file& f) const {
        fmt::print("{}file {} ({})\n", prefix, f.name, f.size);
    }

    void operator()(const std::unique_ptr<dir>& d) {
        fmt::print("{}dir {}\n", prefix, d->name);
        prefix.push_back(' ');
        for (auto& c : d->children)
            boost::apply_visitor(*this, c);
        prefix.pop_back();
    }
};

auto run_a(std::string_view s) {
    auto fs = parse(s);
    //printer{}(fs);
    auto weight_getter = get_weight{};
    weight_getter(fs);
    return reduce(weight_getter.dir_weights | rv::values | sv::filter([](auto size) { return size <= 100000; }));
}

auto run_b(std::string_view s) {
    auto fs = parse(s);
    //printer{}(fs);
    auto weight_getter = get_weight{};
    weight_getter(fs);
    constexpr int64_t total_size = 70000000;
    constexpr int64_t target_size = 30000000;
    const auto current_size = weight_getter.dir_weights.at(fs.get());
    const auto current_space = total_size - current_size;
    const auto required_increase = target_size - current_space;
    auto viable_dirs = weight_getter.dir_weights
      | rv::values
      | sv::filter([&](auto size) { return size >= required_increase; });
    return *std::ranges::min_element(viable_dirs);
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}