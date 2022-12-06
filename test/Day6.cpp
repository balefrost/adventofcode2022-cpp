#include <gtest/gtest.h>
#include <fstream>

using namespace std;

int find_marker_pos(const string &line, int message_size) {
    set<char> seen;
    int i = 0;
    for (; i < line.length() - message_size; ++i) {
        seen.clear();
        for (auto j = 0; j < message_size; ++j) {
            seen.insert(line[i + j]);
        }
        if (seen.size() == message_size) {
            break;
        }
    }

    int position = i + message_size;

    return position;
}

TEST(Day6, Part1) {

    ifstream input;
    input.open("../../test/input/day6.txt");
    string line;
    getline(input, line);

    auto position = find_marker_pos(line, 4);
    
    cout << position << "\n";
}

TEST(Day6, Part2) {
    ifstream input;
    input.open("../../test/input/day6.txt");
    string line;
    getline(input, line);

    auto position = find_marker_pos(line, 14);

    cout << position << "\n";
}
