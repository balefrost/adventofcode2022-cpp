#include <gtest/gtest.h>
#include <fstream>
#include <numeric>
#include <regex>
#include <queue>
#include "util.h"

using namespace std;

namespace day16 {

    //const regex line_regex("Value (\\w+) has flow rate=(\\d+); tunnels lead to values (.*)");
    const regex line_regex("Valve (\\w+) has flow rate=(\\d+); tunnels? leads? to valves? (.*)");
    const regex comma_regex(", ");

    const string sample_input = "Valve AA has flow rate=0; tunnels lead to valves DD, II, BB\n"
                                "Valve BB has flow rate=13; tunnels lead to valves CC, AA\n"
                                "Valve CC has flow rate=2; tunnels lead to valves DD, BB\n"
                                "Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE\n"
                                "Valve EE has flow rate=3; tunnels lead to valves FF, DD\n"
                                "Valve FF has flow rate=0; tunnels lead to valves EE, GG\n"
                                "Valve GG has flow rate=0; tunnels lead to valves FF, HH\n"
                                "Valve HH has flow rate=22; tunnel leads to valve GG\n"
                                "Valve II has flow rate=0; tunnels lead to valves AA, JJ\n"
                                "Valve JJ has flow rate=21; tunnel leads to valve II";

    struct valve_info {
        string valve_name;
        int rate;
        vector<string> adjacent_valves;
    };

    struct state {
        string position;
        int minutes_left;
        set<string> remaining_valves;
        unsigned long long pressure_reduced;
        vector<string> path;
    };

    vector<state> find_adjacent(
            const map<string, valve_info> &valves,
            const map<pair<string, string>, int> &distances,
            const state &s
    ) {
        vector<state> result;
        if (s.minutes_left == 0) {
            return result;
        }

        for (const auto &rv: s.remaining_valves) {
            auto upd_minutes_left = s.minutes_left - distances.at({s.position, rv}) - 1;
            auto upd_remaining_valves = s.remaining_valves;
            upd_remaining_valves.erase(rv);
            auto upd_path = s.path;
            upd_path.push_back(rv);
            if (upd_minutes_left >= 0) {
                result.push_back(state{
                        rv,
                        upd_minutes_left,
                        upd_remaining_valves,
                        s.pressure_reduced + upd_minutes_left * valves.at(rv).rate,
                        upd_path
                });
            }
        }

        return result;
    }

    map<pair<string, string>, int> compute_total_distance(const map<string, valve_info> &valves) {
        set<string> positions;
        for (const auto &p: valves) {
            positions.insert(p.first);
        }
        map<pair<string, string>, int> result;
        set<pair<string, string>> frontier;

        for (const auto &p1: positions) {
            frontier.insert({p1, p1});
            result[{p1, p1}] = 0;
        }

        int steps = 1;
        while (!frontier.empty()) {
            set<pair<string, string>> new_frontier;
            for (const auto &f: frontier) {
                for (const auto &p2: valves.at(f.second).adjacent_valves) {
                    pair<string, string> key{f.first, p2};
                    if (result.try_emplace(key, steps).second) {
                        new_frontier.insert(key);
                    }
                }
            }
            ++steps;
            frontier = std::move(new_frontier);
        }

        return result;
    }

    map<string, valve_info> parse_valves(istream &input) {
        map<string, valve_info> valves;
        string line;
        while (getline(input, line)) {
            smatch m;
            if (!regex_match(line, m, line_regex)) {
                throw logic_error("no match");
            }
            string valve_name = m[1];
            int rate = stoi(m[2]);
            auto adj_str = m[3].str();
            sregex_token_iterator iter(adj_str.begin(), adj_str.end(), comma_regex, -1);
            sregex_token_iterator end;
            vector<string> adjacent_valves(iter, end);
            while (iter != end) {
                adjacent_valves.push_back(iter->str());
                ++iter;
            }
            valves[valve_name] = valve_info{valve_name, rate, adjacent_valves};
        }

        return valves;
    }

    TEST(Day16, Part1) {
        ifstream input;
        input.open("../../test/input/day16.txt");
//    stringstream input(sample_input);

        auto valves = parse_valves(input);

        set<string> interesting_valves;
        for (const auto &v: valves) {
            if (v.second.rate != 0) {
                interesting_valves.insert(v.first);
            }
        }

        state initial_state{"AA", 30, interesting_valves, 0, vector<string>{"AA"}};

        auto total_distance = compute_total_distance(valves);

        stack<state> active_states;
        active_states.push(initial_state);

        unsigned long long most_pressure_reduced = 0;
        vector<string> best_path;
        while (!active_states.empty()) {
            auto s = active_states.top();
            active_states.pop();
            if (s.pressure_reduced > most_pressure_reduced) {
                most_pressure_reduced = s.pressure_reduced;
                best_path = s.path;
            }
            auto adjacent = find_adjacent(valves, total_distance, s);
            for (const auto &adj: adjacent) {
                active_states.push(adj);
            }
        }

        cout << most_pressure_reduced << endl;

    }

    TEST(Day16, Part2) {
        ifstream input;
        input.open("../../test/input/day16.txt");
//    stringstream input(sample_input);
    }

}