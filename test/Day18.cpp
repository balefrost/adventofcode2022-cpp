#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include "pos3.h"
#include "util.h"

using namespace std;

namespace day18 {

    const string sample_input = "2,2,2\n"
                                "1,2,2\n"
                                "3,2,2\n"
                                "2,1,2\n"
                                "2,3,2\n"
                                "2,2,1\n"
                                "2,2,3\n"
                                "2,2,4\n"
                                "2,2,6\n"
                                "1,2,5\n"
                                "3,2,5\n"
                                "2,1,5\n"
                                "2,3,5";

    const regex number_regex("\\d+");

    pos3_ordered_set parse_positions(istream &input) {
        string line;
        auto positions = make_pos3_ordered_set();
        while (getline(input, line)) {
            sregex_iterator iter(line.begin(), line.end(), number_regex);
            pos3 p{};
            p.x = stoi(iter->str());
            ++iter;
            p.y = stoi(iter->str());
            ++iter;
            p.z = stoi(iter->str());
            ++iter;
            positions.insert(p);
        }
        return positions;
    }

    TEST(Day18, Part1) {
        ifstream input;
        input.open("../../test/input/day18.txt");
//    stringstream input(sample_input);

        auto positions = parse_positions(input);

        auto sides = 0;
        for (const auto &pos: positions) {
            sides += 6;
            for (const auto &adj: pos3_adjacent(pos)) {
                if (positions.find(adj) != positions.end()) {
                    sides -= 1;
                }
            }
        }

        cout << sides << endl;
    }

    TEST(Day18, Part2) {
        ifstream input;
        input.open("../../test/input/day18.txt");
//    stringstream input(sample_input);

        auto positions = parse_positions(input);

        auto bounds = pos3_bounds(positions.begin(), positions.end());
        bounds.min.x -= 1;
        bounds.min.y -= 1;
        bounds.min.z -= 1;
        bounds.max.x += 1;
        bounds.max.y += 1;
        bounds.max.z += 1;
        auto start_pos = bounds.min;

        auto seen = make_pos3_ordered_set();
        auto frontier = make_pos3_ordered_set();

        frontier.insert(start_pos);

        while (!frontier.empty()) {
            auto f = *frontier.begin();
            frontier.erase(frontier.begin());

            auto [_, inserted] = seen.insert(f);
            if (inserted) {
                for (const auto &adj: pos3_adjacent(f)) {
                    if (bounds.contains(adj) && !positions.contains(adj)) {
                        frontier.insert(adj);
                    }
                }
            }
        }

        auto surfaces = 0;
        for (const auto &air: seen) {
            auto adj = pos3_adjacent(air);
            for (const auto &a: pos3_adjacent(air)) {
                if (positions.contains(a)) {
                    ++surfaces;
                }
            }
        }

        cout << surfaces << endl;
    }

}