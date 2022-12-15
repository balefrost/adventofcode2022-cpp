#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include <set>
#include "position.h"

using namespace std;

const regex line_regex(R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))");

struct sb_pair {
    position sensor_position;
    position beacon_position;
};

vector<sb_pair> parse(istream &input) {
    vector<sb_pair> result;
    string line;
    while (getline(input, line)) {
        smatch regex_result;
        if (!regex_match(line, regex_result, line_regex)) {
            throw logic_error("no match");
        }

        auto sx = stoi(regex_result[1]);
        auto sy = stoi(regex_result[2]);
        auto bx = stoi(regex_result[3]);
        auto by = stoi(regex_result[4]);
        position sensor_pos { sx, sy };
        position beacon_position { bx, by };
        result.push_back(sb_pair {sensor_pos, beacon_position });
    }
    return result;
}

const string sample_input = "Sensor at x=2, y=18: closest beacon is at x=-2, y=15\n"
                            "Sensor at x=9, y=16: closest beacon is at x=10, y=16\n"
                            "Sensor at x=13, y=2: closest beacon is at x=15, y=3\n"
                            "Sensor at x=12, y=14: closest beacon is at x=10, y=16\n"
                            "Sensor at x=10, y=20: closest beacon is at x=10, y=16\n"
                            "Sensor at x=14, y=17: closest beacon is at x=10, y=16\n"
                            "Sensor at x=8, y=7: closest beacon is at x=2, y=10\n"
                            "Sensor at x=2, y=0: closest beacon is at x=2, y=10\n"
                            "Sensor at x=0, y=11: closest beacon is at x=2, y=10\n"
                            "Sensor at x=20, y=14: closest beacon is at x=25, y=17\n"
                            "Sensor at x=17, y=20: closest beacon is at x=21, y=22\n"
                            "Sensor at x=16, y=7: closest beacon is at x=15, y=3\n"
                            "Sensor at x=14, y=3: closest beacon is at x=15, y=3\n"
                            "Sensor at x=20, y=1: closest beacon is at x=15, y=3";

TEST(Day15, Part1) {
    ifstream input;
    input.open("../../test/input/day15.txt");
    int magic_row = 2000000;
//    stringstream input(sample_input);
//    int magic_row = 10;

    auto sensor_beacon_positions = parse(input);

    set<position> nonbeacon_positions;
    for (const auto &item: sensor_beacon_positions) {
        auto radius = item.sensor_position.manhattan_distance_to(item.beacon_position);
        auto dist_from_row = abs(item.sensor_position.y - magic_row);
        if (dist_from_row <= radius) {
            auto minx = item.sensor_position.x - (radius - dist_from_row);
            auto maxx = minx + (radius - dist_from_row) * 2;
            for (auto x = minx; x <= maxx; ++x) {
                nonbeacon_positions.insert(position { x, magic_row });
            }
        }
    }

    for (const auto &item: sensor_beacon_positions) {
        if (item.beacon_position.y == magic_row) {
            nonbeacon_positions.erase(item.beacon_position);
        }
    }

    cout << nonbeacon_positions.size() << endl;
}

TEST(Day15, Part2) {
    ifstream input;
    input.open("../../test/input/day15.txt");
}
