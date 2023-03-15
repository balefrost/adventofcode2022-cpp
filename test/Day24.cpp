#include <gtest/gtest.h>
#include <exception>
#include <fstream>
#include <numeric>
#include <queue>
#include <regex>
#include <unordered_map>
#include "pos2.h"
#include "util.h"

namespace day24 {
    struct adj_entry {
        size_t steps;
        pos2_ll pos;

        bool operator<(const adj_entry &other) const {
            return pos < other.pos || (pos == other.pos && steps < other.steps);
        }

        bool operator==(const adj_entry &other) const {
            return pos == other.pos && steps == other.steps;
        }
    };
}

template<>
struct std::hash<::day24::adj_entry> {
    using adj_entry = ::day24::adj_entry;

    std::size_t operator()(const adj_entry &entry) const {
        size_t seed = 0;
        hash_combine(seed, entry.steps);
        hash_combine(seed, entry.pos);
        return seed;
    }
};

namespace day24 {
    using namespace std;

    const string sample_input = "#.######\n"
                                "#>>.<^<#\n"
                                "#.<..<<#\n"
                                "#>v.><>#\n"
                                "#<^v^^>#\n"
                                "######.#";

    struct Input {
        map<char, vector<string>> storms;
        pos2_t<long long> start{0, 0};
        pos2_t<long long> dest{0, 0};
        long long width{0};
        long long height{0};

        [[nodiscard]] bool is_pos_safe(long long steps, pos2_t<long long> pos) const {
            if (pos == start || pos == dest) {
                return true;
            } else if (pos.x < 0 || pos.x >= width ||
                       pos.y < 0 || pos.y >= height) {
                return false;
            }
            auto up_ch = storms.at('^').at(posmod(pos.y + steps, height)).at(pos.x);
            auto down_ch = storms.at('v').at(posmod(pos.y - steps, height)).at(pos.x);
            auto right_ch = storms.at('>').at(pos.y).at(posmod(pos.x - steps, width));
            auto left_ch = storms.at('<').at(pos.y).at(posmod(pos.x + steps, width));

            return up_ch == '.' && down_ch == '.' && right_ch == '.' && left_ch == '.';
        }
    };

    Input parse_input(istream &input) {
        string line;
        vector<string> board;
        Input result;
        while (getline(input, line)) {
            board.push_back(std::move(line));
        }

        for (int r = 1; r < board.size() - 1; ++r) {
            const auto &row = board[r];
            map<char, string> dir_storms{
                    {'^', ""},
                    {'>', ""},
                    {'v', ""},
                    {'<', ""},
            };

            for (int c = 1; c < row.size() - 1; ++c) {
                auto item = row[c];
                for (auto &[d, str]: dir_storms) {
                    if (d == item) {
                        str += d;
                    } else {
                        str += '.';
                    }
                }
            }

            for (const auto &[dir, s]: dir_storms) {
                result.storms[dir].push_back(s);
            }
        }
        result.start.x = (long long) board.front().find('.') - 1;
        result.start.y = -1;
        result.dest.x = (long long) board.back().find('.') - 1;
        result.dest.y = (long long) board.size() - 2;

        result.width = (long long) board.front().size() - 2;
        result.height = (long long) board.size() - 2;

        return result;
    }

    struct safe_cache {
        explicit safe_cache(const Input &input) {
            iterations = lcm(input.width, input.height);
            safe_.resize(iterations);
            for (auto iter = 0; iter < iterations; ++iter) {
                auto &safe_iter = safe_.at(iter);
                safe_iter.resize(input.height + 2);
                for (auto y = -1; y < input.height + 1; ++y) {
                    auto &safe_row = safe_iter[y + 1];
                    safe_row.resize(input.width + 2);
                    for (auto x = -1; x < input.width + 1; ++x) {
                        if (input.is_pos_safe(iter, {x, y})) {
                            safe_row[x + 1] = true;
                        }
                    }
                }
            }
        }

        [[nodiscard]] bool get(const adj_entry &entry) const {
            const vector<vector<bool>> &step = safe_.at(entry.steps % iterations);
            if (entry.pos.y + 1 >= step.size()) {
                return false;
            }
            const vector<bool> &row = step.at(entry.pos.y + 1);
            if (entry.pos.x + 1 >= row.size()) {
                return false;
            }
            return row.at(entry.pos.x + 1);
        }

    private:
        size_t iterations;
        vector<vector<vector<bool>>> safe_;
    };

    size_t a_star(
            adj_entry initial_state,
            const std::function<std::vector<adj_entry>(const adj_entry &)> &next_states,
            const std::function<size_t(const adj_entry &)> &heuristic_remaining,
            const std::function<bool(const adj_entry &)> &done) {
        unordered_map < adj_entry, size_t > best_so_far{
                {initial_state, 0}
        };
        priority_queue<adj_entry, std::vector<adj_entry>, std::function<bool(const adj_entry &,
                                                                             const adj_entry &)>> queue(
                [&heuristic_remaining, &best_so_far]
                        (const adj_entry &a, const adj_entry &b) {
                    // intentionally backwards; we want to explore the node with the lowest cost first
                    return best_so_far.at(a) + heuristic_remaining(a) > best_so_far.at(b) + heuristic_remaining(b);
                });

        queue.push(initial_state);
        while (!queue.empty()) {
            auto item = queue.top();
            queue.pop();

            auto cost_to_get_here = best_so_far.at(item) + 1;

            auto nexts = next_states(item);
            for (const auto &next: nexts) {
                const auto last_best = best_so_far.find(next);
                if (last_best == best_so_far.end() || cost_to_get_here < last_best->second) {
                    best_so_far[next] = cost_to_get_here;
                    queue.push(next);
                }

                if (done(next)) {
                    return best_so_far.at(next);
                }
            }
        }

        throw logic_error("No more states");
    }

    const vector<pos2_t<long long>> dirs{
            {0,  0},
            {0,  -1},
            {0,  1},
            {-1, 0},
            {1,  0},
    };

    size_t run_a_star(const Input &in, const safe_cache &safe, size_t steps, pos2_ll start,
                      pos2_ll dest) {
        auto iterations = lcm(in.width, in.height);
        adj_entry initial_state{
                steps % iterations,
                start
        };

        auto cost = a_star(
                initial_state,
                [&](const adj_entry &state) {
                    vector<adj_entry> result;
                    for (const auto &dir: dirs) {
                        adj_entry next_state{state.steps + 1, state.pos + dir};
                        if (safe.get(next_state)) {
                            result.push_back(next_state);
                        }
                    }
                    return result;
                },
                [&dest](const adj_entry &x) {
                    return x.pos.manhattan_distance_to(dest);
                },
                [&dest](const adj_entry &state) {
                    return state.pos == dest;
                });

        return cost;
    }

    TEST(Day24, Part1) {
        ifstream input;
        input.open("../../test/input/day24.txt");
//        stringstream input(sample_input);

        auto in = parse_input(input);

        safe_cache safe(in);

        auto cost = run_a_star(in, safe, 0, in.start, in.dest);

        cout << cost << endl;
    }

    TEST(Day24, Part2) {
        ifstream input;
        input.open("../../test/input/day24.txt");
//        stringstream input(sample_input);

        auto in = parse_input(input);

        safe_cache safe(in);

        size_t cost_so_far = run_a_star(in, safe, 0, in.start, in.dest);
        cost_so_far += run_a_star(in, safe, cost_so_far, in.dest, in.start);
        cost_so_far += run_a_star(in, safe, cost_so_far, in.start, in.dest);

        cout << cost_so_far << endl;
    }
}
