#include <gtest/gtest.h>
#include <fstream>
#include <unordered_set>

typedef std::pair<int, int> range;

bool fullyContains(range outer, range inner) {
    return inner.first >= outer.first && inner.second <= outer.second;
}

bool overlaps(range range1, range range2) {
    return range1.second >= range2.first && range1.first <= range2.second;
}

TEST(Day4, Part1) {
    std::ifstream input;
    input.open("../../test/input/day4.txt");

    int countOverlapping = 0;

    while(true) {
        int afrom, ato;
        int bfrom, bto;
        char junk;
        input >> afrom >> junk >> ato >> junk >> bfrom >> junk >> bto;
        if (!input) {
            break;
        }
        auto range1 = std::pair(afrom, ato);
        auto range2 = std::pair(bfrom, bto);

        if (fullyContains(range1, range2) || fullyContains(range2, range1)) {
            countOverlapping++;
        }
    }

    std::cout << countOverlapping << std::endl;
}

TEST(Day4, Part2) {
    std::ifstream input;
    input.open("../../test/input/day4.txt");

    int counterIntersecting = 0;

    while(true) {
        int afrom, ato;
        int bfrom, bto;
        char junk;
        input >> afrom >> junk >> ato >> junk >> bfrom >> junk >> bto;
        if (!input) {
            break;
        }
        auto range1 = std::pair(afrom, ato);
        auto range2 = std::pair(bfrom, bto);

        if (overlaps(range1, range2)) {
            counterIntersecting++;
        }
    }

    std::cout << counterIntersecting << std::endl;
}
