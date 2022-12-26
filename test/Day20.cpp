#include <gtest/gtest.h>
#include <fstream>

using namespace std;

namespace day20 {

    const string sample_input = "1\n"
                                "2\n"
                                "-3\n"
                                "3\n"
                                "-2\n"
                                "0\n"
                                "4";

    vector<long long> parse_input(istream &input) {
        string line;
        vector<long long> result;
        while (getline(input, line)) {
            result.push_back(stoi(line));
        }
        return result;
    }

    long long posmod(long long numerator, long long denominator) {
        auto result = numerator % denominator;
        if (result < 0) {
            result += denominator;
        }
        return result;
    }

    pair<vector<size_t>, vector<size_t>> prepare_next_prev(size_t n) {
        vector<size_t> next(n);
        vector<size_t> prev(n);
        for (auto i = 0; i < n; ++i) {
            next[i] = i + 1;
            prev[i] = i - 1;
        }
        next[n - 1] = 0;
        prev[0] = n - 1;

        return {next, prev};
    }

    void mix_indices(const vector<long long> &values, vector<size_t> &next, vector<size_t> &prev) {
        for (size_t i = 0; i < values.size(); ++i) {
            auto to_move = posmod(values[i], values.size() - 1);
            if (to_move == 0) {
                continue;
            }
            auto before_idx = i;
            for (size_t j = 0; j < to_move; ++j) {
                before_idx = next[before_idx];
            }
            auto after_idx = next[before_idx];

            //remove item being moved
            next[prev[i]] = next[i];
            prev[next[i]] = prev[i];

            //insert item being moved
            next[before_idx] = i;
            prev[i] = before_idx;

            next[i] = after_idx;
            prev[after_idx] = i;
        }
    }

    vector<long long>
    assemble_vector(const vector<long long> &values, const vector<size_t> &next, size_t initial_index = 0) {
        vector<long long> result(values.size());
        size_t idx = initial_index;
        for (auto i = 0; i < values.size(); ++i) {
            result[i] = values[idx];
            idx = next[idx];
        }

        return result;
    }

    vector<long long> mix(const vector<long long> &values) {
        auto [next, prev] = prepare_next_prev(values.size());

        mix_indices(values, next, prev);

        return assemble_vector(values, next);
    }

    TEST(Day20, Part1) {
        ifstream input;
        input.open("../../test/input/day20.txt");
//        stringstream input(sample_input);

        auto values = parse_input(input);

        auto idx0 = std::find(values.begin(), values.end(), 0) - values.begin();

        auto [next, prev] = prepare_next_prev(values.size());
        mix_indices(values, next, prev);

        auto mixed = assemble_vector(values, next, idx0);
        auto result = mixed.at(1000 % mixed.size()) + mixed.at(2000 % mixed.size()) +
                      mixed.at(3000 % mixed.size());

        cout << result << endl;
    }

    TEST(Day20, Part2) {
        ifstream input;
        input.open("../../test/input/day20.txt");
//        stringstream input(sample_input);

        auto values = parse_input(input);
        for (auto &item: values) {
            item *= 811589153;
        }

        auto idx0 = std::find(values.begin(), values.end(), 0) - values.begin();

        auto [next, prev] = prepare_next_prev(values.size());
        for (auto i = 0; i < 10; ++i) {
            mix_indices(values, next, prev);
        }

        auto mixed = assemble_vector(values, next, idx0);
        auto result = mixed.at(1000 % mixed.size()) + mixed.at(2000 % mixed.size()) +
                      mixed.at(3000 % mixed.size());

        cout << result << endl;
    }

}