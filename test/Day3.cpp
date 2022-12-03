#include <gtest/gtest.h>
#include <fstream>
#include <unordered_set>

int item_priority(char item) {
    switch (item) {
        case 'a'...'z':
            return 1 + (item - 'a');
        case 'A'...'Z':
            return 27 + (item - 'A');
        default:
            throw std::logic_error("impossible");
    }
}


TEST(Day3, Part1) {
    std::ifstream input;
    input.open("../../test/input/day3.txt");
    auto sum = 0;
    while (true) {
        std::string line;
        std::getline(input, line);
        if (!input) {
            break;
        }
        std::set<char> compartment1;
        std::set<char> compartment2;
        for (auto i = 0; i < line.length() / 2; ++i) {
            compartment1.insert(line[i]);
        }
        for (auto i = line.length() / 2; i < line.length(); ++i) {
            compartment2.insert(line[i]);
        }
        std::vector<char> common;
        std::set_intersection(compartment1.begin(), compartment1.end(),
                              compartment2.begin(), compartment2.end(),
                              std::back_inserter(common));
        auto priority = item_priority(common.at(0));
        sum += priority;
    }

    std::cout << sum << "\n";
}

TEST(Day3, Part2) {
    std::ifstream input;
    input.open("../../test/input/day3.txt");
    auto sum = 0;
    while (true) {
        const int groupSize = 3;

        std::set<char> intersected;
        for (auto i = 0; i < groupSize; ++i) {
            std::string line;
            std::getline(input, line);
            if (i == 0) {
                intersected.clear();
                std::copy(line.begin(), line.end(), std::inserter(intersected, intersected.end()));
            } else {
                std::set<char> result;
                std::set<char> temp;
                std::copy(line.begin(), line.end(), std::inserter(temp, temp.end()));

                std::set_intersection(
                        intersected.begin(), intersected.end(),
                        temp.begin(), temp.end(),
                        std::inserter(result, result.end())
                );
                std::swap(intersected, result);
            }
        }

        if (!input) {
            break;
        }

        auto priority = item_priority(*intersected.begin());
        sum += priority;
    }

    std::cout << sum << "\n";
}
