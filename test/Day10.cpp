#include <gtest/gtest.h>
#include <fstream>
#include <regex>

using namespace std;

namespace day10 {

    const regex addx_regex("addx (-?\\d+)");
    const regex noop_regex("noop");

    TEST(Day10, Part1) {
        ifstream input;
        input.open("../../test/input/day10.txt");

        int x = 1;
        int cycle_number = 0;
        deque<function<void()>> pending;

        int result = 0;

        while (true) {
            cycle_number++;

            if (cycle_number == 20 || (cycle_number - 20) % 40 == 0) {
                result += (cycle_number * x);
            }
            if (!pending.empty()) {
                auto todo = pending.front();
                todo();
                pending.pop_front();
            } else {

                string line;
                getline(input, line);

                if (!input) {
                    break;
                }

                smatch match_result;
                if (regex_match(line, match_result, addx_regex)) {
                    auto value = stoi(match_result[1]);
                    pending.emplace_back([=, &x]() { x += value; });
                } else if (regex_match(line, match_result, noop_regex)) {
                }
            }
        }

        cout << result << endl;
    }

    TEST(Day10, Part2) {
        ifstream input;
        input.open("../../test/input/day10.txt");

        int x = 1;
        int cycle_number = 0;
        deque<function<void()>> pending;

        vector<string> result;

        while (true) {
            cycle_number++;

            int row_number = (cycle_number - 1) / 40;
            int column_number = (cycle_number - 1) % 40;

            if (result.size() <= row_number) {
                result.emplace_back(40, '.');
            }

            if (abs(column_number - x) <= 1) {
                result[row_number][column_number] = '#';
            }

            if (!pending.empty()) {
                auto todo = pending.front();
                todo();
                pending.pop_front();
            } else {

                string line;
                getline(input, line);

                if (!input) {
                    break;
                }

                smatch match_result;
                if (regex_match(line, match_result, addx_regex)) {
                    auto value = stoi(match_result[1]);
                    pending.emplace_back([=, &x]() { x += value; });
                } else if (regex_match(line, match_result, noop_regex)) {
                }
            }
        }

        for (const auto &item: result) {
            cout << item << endl;
        }
    }

}