//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <boost/container_hash/hash.hpp>
#include <boost/bimap.hpp>
#include <boost/unordered_set.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <fmt/std.h>
#include <fmt/ranges.h>

#ifdef USE_GUI
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9)", 5, 7}
};

using point_t = std::tuple<int, int, int>;
using brick_t = std::tuple<point_t, point_t>;
auto& get_z(point_t& p) { return std::get<2>(p); }
auto get_lowest_z(brick_t brick) {
    const auto [p, q] = brick;
    const auto [p_x, p_y, p_z] = p;
    const auto [q_x, q_y, q_z] = q;
    return p_z;
}

const x3::rule<class point_, point_t> point_ = "point";
const auto point__def = int_ >> ',' >> int_ >> ',' >> int_;
BOOST_SPIRIT_DEFINE(point_);

auto parse(std::string_view s) {
    auto ret = std::vector<brick_t>{};
    phrase_parse(s.begin(), s.end(),
        *(point_ >> '~' >> point_),
        space, ret);
    return ret;
}

auto tile_view(brick_t brick) {
    const auto [p, q] = brick;
    const auto [p_x, p_y, p_z] = p;
    const auto [q_x, q_y, q_z] = q;
    return rv::cartesian_product(
        rv::iota(p_x, q_x+1),
        rv::iota(p_y, q_y+1),
        rv::iota(p_z, q_z+1))
        | rv::transform([](auto a) { return point_t{ a }; });
}

auto fall(brick_t brick) {
    auto& [p, q] = brick;
    auto& [p_x, p_y, p_z] = p;
    auto& [q_x, q_y, q_z] = q;
    --p_z;
    --q_z;
    return brick;
}
auto simulate(auto& bricks) {
    auto tiles = std::unordered_map<point_t, brick_t, boost::hash<point_t>>{};
    for (const auto brick : bricks) {
        for (auto tile : tile_view(brick)) {
            tiles[tile] = brick;
        }
    }
    using boost::bimaps::unordered_multiset_of;
    auto supporting = boost::bimap<unordered_multiset_of<brick_t>, unordered_multiset_of<brick_t>>{};

    auto falling = bricks;
    ranges::sort(falling, std::less{}, [](const brick_t& b) {
        const auto& [p, q] = b;
        const auto [p_x, p_y, p_z] = p;
        const auto [q_x, q_y, q_z] = q;
        return std::min(p_z, q_z);
        });
    auto final_bricks = std::vector<brick_t>{};

    while (!falling.empty()) {
        for (auto it = falling.begin(); it != falling.end(); ) {
            auto& brick = *it;
            if (get_lowest_z(brick) == 0) {
                final_bricks.push_back(brick);
                it = falling.erase(it);
            }
            else {
                auto fallen_brick = fall(brick);
                auto supporters = tile_view(fallen_brick)
                    | rv::transform([&](point_t p) { return tiles.find(p); })
                    | rv::filter([&](auto it) { return it != tiles.end(); })
                    | rv::transform([](auto it) { return it->second; })
                    | rv::remove(brick)
                    | rv::unique;
                if (std::ranges::empty(supporters)) {
                    for (auto tile : tile_view(brick))
                        tiles.erase(tiles.find(tile));
                    brick = fallen_brick;
                    for (auto tile : tile_view(brick))
                        tiles[tile] = brick;
                    ++it;
                }
                else {
                    for (auto supporter : supporters)
                        supporting.insert({ supporter, brick });
                    final_bricks.push_back(brick);
                    it = falling.erase(it);
                }
            }
        }
    }

    // for (auto [l, r] : supporting.left) {
    //     fmt::println("{} -> {}", l, r);
    // }
    return std::tuple{ supporting,final_bricks };
}

auto run_a(std::string_view s) {
    const auto bricks = parse(s);
    const auto [supporting, final_bricks] = simulate(bricks);

    const auto only_one_support = [&](brick_t brick) {
        return supporting.right.count(brick) == 1;
    };
    const auto can_be_disintegrated = [&](brick_t brick) {
        auto [begin,end] = supporting.left.equal_range(brick);
        return ranges::none_of(ranges::subrange{ begin,end }, only_one_support, [](auto kv) { return kv.second; });
    };

    return ranges::count_if(final_bricks, can_be_disintegrated);
}
#ifdef USE_GUI

void render_grid(auto& supporting, auto& final_bricks, auto proj) {
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
    constexpr auto tile_size = 10;
    for (auto brick : final_bricks) {
        const auto [p, q] = brick;
        const auto [p_x, p_y, p_z] = p;
        const auto [q_x, q_y, q_z] = q;
        draw_list->AddRect(
            { (float)proj(p)*tile_size + canvas_p0.x, (float)(p_z+1)*tile_size + canvas_p0.y},
            { (float)proj(q)*tile_size + canvas_p0.x, (float)(q_z+1)*tile_size + canvas_p0.y},
            IM_COL32(100, 100, 100, 255));
    }
}

void render_bricks(auto& supporting, auto& final_bricks) {
    ImGui::BeginChild("X grid", { 800,800 });
    render_grid(supporting, final_bricks, [](point_t p) { return std::get<0>(p); });
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("Y grid", { 800,800 });
    render_grid(supporting, final_bricks, [](point_t p) { return std::get<1>(p); });
    ImGui::EndChild();
}

int gui_loop(auto s) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    const auto bricks = parse(s);
    const auto [supporting, final_bricks] = simulate(bricks);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();

        // (Your code process and dispatch Win32 messages)
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(); // Show demo window! :)

        // my code
        ImGui::Begin("Bricks");
        render_bricks(supporting, final_bricks);
        ImGui::End();

        // Rendering
        // (Your code clears your framebuffer, renders your other stuff etc.)
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // (Your code calls glfwSwapBuffers() etc.)

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
#endif

auto run_b(std::string_view s) {
    const auto bricks = parse(s);
    const auto [supporting, final_bricks] = simulate(bricks);

    const auto only_one_support = [&](brick_t b) {
        return supporting.right.count(b) == 1;
    };

    const auto would_fall = [&](brick_t brick) {
        auto falling_range = boost::unordered_set{ brick };
        auto ret = 0;
        while (!falling_range.empty()) {

            const auto all_falling = [&](brick_t b) {
                const auto [begin, end] = supporting.right.equal_range(b);
                return std::all_of(begin, end, [&](auto kv) { return falling_range.contains(kv.second); });
            };

            const auto next_fall = falling_range
                | rv::for_each([&](brick_t b) {
                    const auto [begin, end] = supporting.left.equal_range(b);
                    return ranges::subrange{ begin,end };
                })
                | rv::transform([](auto kv) { return kv.second; })
                | rv::filter(all_falling)
                | ranges::to<boost::unordered_set>;

            ret += next_fall.size();
            falling_range = std::move(next_fall);
        }
        return ret;
    };

    return reduce(final_bricks | rv::transform(would_fall));
}

int main() {
    const auto input = get_input(AOC_DAY);
    //run(run_a, run_b, test_data, get_input(AOC_DAY));
#ifdef USE_GUI
    gui_loop(input);
#endif
}
