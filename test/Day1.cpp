#include <gtest/gtest.h>
#include <fstream>

namespace day1 {

    TEST(Day1, Part1) {
        std::ifstream input;
        input.open("../../test/input/day1.txt");
        std::string line;
        auto sum = 0;
        auto largestSum = 0;
        while (std::getline(input, line)) {
            if (line.empty()) {
                largestSum = std::max(largestSum, sum);
                sum = 0;
            } else {
                sum += stoi(line);
            }
        }
        std::cout << largestSum << "\n";
    }

    TEST(Day1, Part2) {
        std::ifstream input;
        input.open("../../test/input/day1.txt");
        std::string line;
        auto sum = 0;
        std::vector<int> counts;
        while (std::getline(input, line)) {
            if (line.empty()) {
                counts.push_back(sum);
                sum = 0;
            } else {
                sum += stoi(line);
            }
        }
        std::sort(counts.begin(), counts.end(), [](auto a, auto b) {
            return a > b;
        });
        auto totalSum = 0;
        std::for_each(counts.begin(), counts.begin() + 3, [&](auto a) { totalSum += a; });
        std::cout << totalSum << "\n";
    }

}