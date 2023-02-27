#include <gtest/gtest.h>
#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <unordered_set>
#include "pos2.h"
#include "util.h"

using namespace std;

namespace day22 {

//    const string sample_input = ".....\n"
//                                "..##.\n"
//                                "..#..\n"
//                                ".....\n"
//                                "..##.\n"
//                                ".....\n";

    const string sample_input = "..............\n"
                                "..............\n"
                                ".......#......\n"
                                ".....###.#....\n"
                                "...#...#.#....\n"
                                "....#...##....\n"
                                "...#.###......\n"
                                "...##.#.##....\n"
                                "....#..#......\n"
                                "..............\n"
                                "..............\n"
                                "..............\n";

    const vector<vector<pos2>> offsets_to_check{
            //NORTH
            {
                    {-1, -1},
                    {0,  -1},
                    {1,  -1}},
            //SOUTH
            {
                    {-1, 1},
                    {0,  1},
                    {1,  1}},
            //WEST
            {
                    {-1, -1},
                    {-1, 0},
                    {-1, 1}},
            //EAST
            {
                    {1,  -1},
                    {1,  0},
                    {1,  1}}
    };

    const vector<pos2> all_adjacent{
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  -1},
            {0,  1},
            {1,  -1},
            {1,  0},
            {1,  1}
    };

    set<pos2> step_elves(const set<pos2> &elves, int offset_offset) {
        //dest, srcs
        map<pos2, vector<pos2>> proposed_moves;
        set < pos2 > new_elves;
        for (const auto &elf: elves) {
            int i;
            for (i = 0; i < 4; ++i) {
                int offset = (i + offset_offset) % 4;
                const auto &to_check = offsets_to_check[offset];
                auto dir_open = all_of(
                        to_check.begin(),
                        to_check.end(),
                        [&](const auto &elem) {
                            return !set_contains(elves, elf + elem);
                        });
                auto has_friends = any_of(
                        all_adjacent.begin(), all_adjacent.end(),
                        [&](const auto &elem) {
                            return set_contains(elves, elf + elem);
                        });
                if (has_friends && dir_open) {
                    auto dest = elf + to_check[1];
                    proposed_moves[dest].push_back(elf);
                    break;
                }
            }
            if (i == 4) {
                new_elves.insert(elf);
            }
        }

        for (const auto &[dst, srcs]: proposed_moves) {
            if (srcs.size() == 1) {
                new_elves.insert(dst);
            } else {
                new_elves.insert(srcs.begin(), srcs.end());
            }
        }

        return new_elves;
    }

    string format_elves(const set<pos2> &elves) {
        stringstream result;
        auto [min, max] = bounding_box(elves);
        for (auto y = min.y; y <= max.y; ++y) {
            for (auto x = min.x; x <= max.x; ++x) {
                if (set_contains(elves, {x, y})) {
                    result << '#';
                } else {
                    result << '.';
                }
            }
            result << '\n';
        }
        return result.str();
    }

    set<pos2> read_elves(istream &input) {
        string line;
        set < pos2 > elves;
        int line_no = 0;
        while (getline(input, line)) {
            if (line.empty()) {
                break;
            }
            for (auto char_no = 0; char_no < line.length(); ++char_no) {
                if (line[char_no] == '#') {
                    elves.insert({char_no, line_no});
                }
            }
            ++line_no;
        }
        return elves;
    }

    TEST(Day23, Part1) {
        ifstream input;
        input.open("../../test/input/day23.txt");
//        stringstream input(sample_input);

        auto elves = read_elves(input);

//        cout << format_elves(elves);
//        cout << std::endl;
//        cout << std::endl;

        for (int i = 0; i < 10; ++i) {
            auto new_elves = step_elves(elves, i);
//            cout << format_elves(new_elves);
//            cout << std::endl;
//            cout << std::endl;
            elves = std::move(new_elves);
        }

        auto [emin, emax] = bounding_box(elves);
        auto empty_ground = 0;
        for (auto y = emin.y; y <= emax.y; ++y) {
            for (auto x = emin.x; x <= emax.x; ++x) {
                if (!set_contains(elves, {x, y})) {
                    empty_ground++;
                }
            }
        }

        cout << empty_ground << endl;
    }

    TEST(Day23, Part2) {
        ifstream input;
        input.open("../../test/input/day23.txt");
//        stringstream input(sample_input);

        auto elves = read_elves(input);

        int i;
        for (i = 0; ; ++i) {
            auto new_elves = step_elves(elves, i);
            if (new_elves == elves) {
                break;
            }
            elves = std::move(new_elves);
        }

        cout << (i + 1) << endl;
    }

}