#include <gtest/gtest.h>
#include <fstream>
#include <regex>

using namespace std;

struct theMove {
    int count;
    int from;
    int to;
};

struct puzzleInput {
    vector<vector<char>> crates;
    vector<theMove> moves;
};

puzzleInput parseInput() {
    ifstream input;
    input.open("../../test/input/day5.txt");
    regex crateLineRegex(R"((\[\w\] *)*)");
    regex blankRegex(R"(\s*)");
    regex crate_regex(R"(\[(\w)\])");
    regex moveRegex(R"(move (\d+) from (\d+) to (\d+))");
    vector<vector<char>> columns;
    vector<theMove> moves;
    while (true) {
        string line;
        getline(input, line);
        if (!input) {
            break;
        }
        if (regex_match(line, blankRegex)) {
            continue;
        }
        smatch moveMatch;
        if (regex_match(line, moveMatch, moveRegex)) {
            auto count = stoi(moveMatch[1].str());
            auto from = stoi(moveMatch[2].str()) - 1;
            auto to = stoi(moveMatch[3].str()) - 1;
            moves.push_back(theMove { count, from, to});
        } else {
            sregex_iterator iter(line.begin(), line.end(), crate_regex);
            for (; iter != sregex_iterator(); ++iter) {
                auto column = iter->position() / 4;
                while (columns.size() <= column) {
                    columns.emplace_back();
                }
                char theChar = iter->str()[1];
                vector<char> &vec = columns.at(column);
                vec.push_back(theChar);
            }
        }
    }
    for (auto &item: columns) {
        std::reverse(item.begin(), item.end());
    }

    puzzleInput puzIn { columns, moves };

    return puzIn;
}

TEST(Day5, Part1) {
    auto puzIn = parseInput();

    for (const auto &item: puzIn.moves) {
        for (auto i = 0; i < item.count; ++i) {
            auto moved = puzIn.crates[item.from].back();
            puzIn.crates[item.from].pop_back();
            puzIn.crates[item.to].push_back(moved);
        }
    }

    // HNSNMTLHQ correct
    for (const auto &item: puzIn.crates) {
        cout << item.back();
    }
    cout << endl;

}

TEST(Day5, Part2) {
    auto puzIn = parseInput();

    for (auto i = 0; i < puzIn.moves.size(); ++i) {
        auto item = puzIn.moves[i];
        auto& source = puzIn.crates[item.from];
        auto& dest = puzIn.crates[item.to];
        auto end = source.end();
        auto start = end - item.count;
        copy(start, end, back_inserter(dest));
        source.erase(start, end);
    }

    //RNLFDJMCT correct
    for (const auto &item: puzIn.crates) {
        cout << item.back();
    }
    cout << endl;

}
