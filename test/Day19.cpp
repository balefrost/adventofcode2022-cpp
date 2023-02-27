#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <concepts>
#include <fstream>
#include <iterator>
#include <numeric>
#include <queue>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include "util.h"

using namespace std;

namespace day19 {
    struct state {
        int minutes;

        int ore;
        int clay;
        int obsidian;
        int geodes;

        int ore_bots;
        int clay_bots;
        int obs_bots;
        int geo_bots;

        [[nodiscard]] int ub_max_geodes() const {
            //assume we make one robot in every remaining minute.
            //minute 1: make geo_bots geodes, make 1 geo bot
            //minute 2: mane (geo_bots + 1) geodes, make 1 geo bot
            //minute 3: mane (geo_bots + 2) geodes, make 1 geo bot
            //...
            //minute n: make (geo_bots + n - 1) geodes, make 1 geo bot
            //total geodes: (geo_bots + geo_bots + minutes - 1) * minutes / 2
            auto additional_geodes = (geo_bots + geo_bots + minutes - 1) * minutes / 2;
            return geodes + additional_geodes;
        }

        bool operator<(const state &b) const {
            return comparer.compare(*this, b) == strong_ordering::less;
        }

        bool operator==(const state &b) const {
            return comparer.compare(*this, b) == strong_ordering::equal;
        }

        static const inline auto comparer = compare_items<state>()
                .then_by_reversed(&state::minutes)
                .then_by<int>(&state::ub_max_geodes)
                .then_by(&state::geo_bots)
                .then_by(&state::obs_bots)
                .then_by(&state::clay_bots)
                .then_by(&state::ore_bots)
                .then_by(&state::geodes)
                .then_by(&state::obsidian)
                .then_by(&state::clay)
                .then_by(&state::ore)
        ;
    };
}

//template<>
//struct std::hash<day19::state> {
//    std::size_t operator()(const day19::state &s) const {
//        return day19::state::comparer.hash(s);
//    }
//};

namespace day19 {

    const string sample_input = "Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.\n"
                                "Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian.";

    const regex line_regex(
            "Blueprint (\\d+): Each ore robot costs (\\d+) ore. Each clay robot costs (\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each geode robot costs (\\d+) ore and (\\d+) obsidian.");


    struct blueprint {
        int blueprint_no;
        int ore_ore_cost;
        int clay_ore_cost;
        int obs_ore_cost;
        int obs_clay_cost;
        int geo_ore_cost;
        int geo_obs_cost;
    };

    template<typename OutIter>
    void find_adjacent(state s, const blueprint &bp, OutIter out) {

        auto ss = s;
        ss.ore += s.ore_bots;
        ss.clay += s.clay_bots;
        ss.obsidian += s.obs_bots;
        ss.geodes += s.geo_bots;
        ss.minutes -= 1;

        if (ss.minutes < 0) {
            return;
        }

        bool can_make_ore_bot = false;
        if (s.ore >= bp.ore_ore_cost) {
            auto sss = ss;
            sss.ore -= bp.ore_ore_cost;
            sss.ore_bots += 1;
            *out++ = sss;
            can_make_ore_bot = true;
        }
        bool can_make_clay_bot = false;
        if (s.ore >= bp.clay_ore_cost) {
            auto sss = ss;
            sss.ore -= bp.clay_ore_cost;
            sss.clay_bots += 1;
            *out++ = sss;
            can_make_clay_bot = true;
        }
        bool can_make_obs_bot = false;
        if (s.ore >= bp.obs_ore_cost && s.clay >= bp.obs_clay_cost) {
            auto sss = ss;
            sss.ore -= bp.obs_ore_cost;
            sss.clay -= bp.obs_clay_cost;
            sss.obs_bots += 1;
            *out++ = sss;
            can_make_obs_bot = true;
        }
        bool can_make_geo_bot = false;
        if (s.ore >= bp.geo_ore_cost && s.obsidian >= bp.geo_obs_cost) {
            auto sss = ss;
            sss.ore -= bp.geo_ore_cost;
            sss.obsidian -= bp.geo_obs_cost;
            sss.geo_bots += 1;
            *out++ = sss;
            can_make_geo_bot = true;
        }

//        if (can_make_clay_bot && ss.clay_bots == 0) {
//            return;
//        }
//        if (can_make_obs_bot && ss.obs_bots == 0) {
//            return;
//        }
//        if (can_make_geo_bot && ss.geo_bots == 0) {
//            return;
//        }

        if (can_make_ore_bot && can_make_clay_bot && can_make_obs_bot && can_make_geo_bot) {
            return;
        }

        *out++ = ss;
    }

    vector<blueprint> parse_blueprints(istream &input) {
        vector<blueprint> result;
        string line;
        while (getline(input, line)) {
            smatch the_match;
            regex_match(line, the_match, line_regex);
            blueprint bp{};
            bp.blueprint_no = stoi(the_match[1]);
            bp.ore_ore_cost = stoi(the_match[2]);
            bp.clay_ore_cost = stoi(the_match[3]);
            bp.obs_ore_cost = stoi(the_match[4]);
            bp.obs_clay_cost = stoi(the_match[5]);
            bp.geo_ore_cost = stoi(the_match[6]);
            bp.geo_obs_cost = stoi(the_match[7]);

            result.push_back(bp);
        }
        return result;
    }

//    const auto state_priority_comparer = compare_items<state>()
//            .then_by<int>([](const state &v) { return v.ub_max_geodes(); })
//            .then_by_comparer(state::comparer);
//
//    bool compare_state_priority(const state &a, const state &b) {
//        return state_priority_comparer.compare(a, b) == strong_ordering::less;
//    }

    int find_best_cracked_geodes(const blueprint &bp) {
        state initial_state{.minutes = 24, .ore_bots = 1};

        priority_queue<state> frontier;
        frontier.push(initial_state);

//        unordered_set<state, vector<state>, decltype(compare_state_priority)> seen;
        size_t last_total_processed = 0;
        size_t total_processed = 1;
        int max_geodes = 0;
        vector<state> adj;
        set<state> distinct_states;
        while (!frontier.empty()) {
            ++total_processed;
            auto s = frontier.top();
            frontier.pop();
            if (distinct_states.insert(s).second) {

                if (s.geodes > max_geodes) {
                    max_geodes = s.geodes;
                    cout << "max now " << max_geodes << endl;
                }

                if (s.ub_max_geodes() >= max_geodes) {
                    adj.clear();
                    find_adjacent(s, bp, back_inserter(adj));
                    for (const auto &a: adj) {
                        if (a.ub_max_geodes() >= max_geodes) {
                            frontier.push(a);
                        }
                    }
                }
            }
//            if (seen.insert(s).second) {
//                find_adjacent(s, blueprints[0], back_inserter(frontier));
//            }
            if (last_total_processed / 100000 != total_processed / 100000) {
                last_total_processed = total_processed;
                cout << ((total_processed / 100000) * 100000) << "    " << frontier.size() << endl;
            }
        }

        cout << "total: " << total_processed << endl;
        cout << "distinct: " << distinct_states.size() << endl;
        return max_geodes;
    }

    TEST(Day19, DISABLED_Part1) {
        ifstream input;
        input.open("../../test/input/day19.txt");
//        stringstream input(sample_input);

        auto blueprints = parse_blueprints(input);

        auto start = chrono::steady_clock::now();

        auto sum = 0;
        for (const auto &item: blueprints) {
//            auto &item = blueprints[0];
            auto ql = item.blueprint_no * find_best_cracked_geodes(item);
            sum += ql;
        }
//
////        int quality_sum = 0;
////        for (const auto &bp: blueprints) {
////            quality_sum += bp.blueprint_no * find_best_cracked_geodes(bp);
////        }
//
        cout << "result: " << sum << endl;
        auto end = chrono::steady_clock::now();
        auto p_as_float = (double)chrono::steady_clock::period::num / (double)chrono::steady_clock::period::den;
        cout << "seconds: " << ((end - start).count() * p_as_float) << endl;
    }

    TEST(Day19, Part2) {
//    ifstream input;
//    input.open("../../test/input/day19.txt");
        stringstream input(sample_input);
    }
}