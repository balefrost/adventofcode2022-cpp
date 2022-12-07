#include <gtest/gtest.h>
#include <fstream>
#include <regex>

using namespace std;

const regex cdRegex("^\\$ cd (.+)$");
const regex fileRegex("^(\\d+) (.*)$");

template<class Iter>
string joinPath(Iter begin, Iter end) {
    string pathString = "/";
    bool first = true;
    for (Iter i = begin; i != end; ++i) {
        if (!first) {
            pathString += "/";
        }
        first = false;
        pathString += *i;
    }
    return pathString;
}

unordered_map<string, int> buildAggregateSize(istream &input) {
    vector<string> currentPath;
    unordered_map<string, int> aggregateSize;

    while (true) {
        string line;
        getline(input, line);
        if (!input) {
            break;
        }
        smatch results;
        if (regex_match(line, results, cdRegex)) {
            auto path = results[1];
            if (path == "..") {
                currentPath.pop_back();
            } else if (path == "/") {
                currentPath.clear();
            } else {
                currentPath.push_back(results[1]);
            }
        } else if (regex_match(line, results, fileRegex)) {
            int fileSize = stoi(results[1]);
            auto end = currentPath.begin();
            while (true) {
                string pathString = joinPath(currentPath.begin(), end);
                aggregateSize[pathString] += fileSize;
                if (end == currentPath.end()) {
                    break;
                }
                ++end;
            }
        }
    }

    return aggregateSize;
}

TEST(Day7, Part1) {
    ifstream input;
    input.open("../../test/input/day7.txt");

    auto aggregateSize = buildAggregateSize(input);

    int sum = 0;
    for (const auto &item: aggregateSize) {
        if (item.second <= 100000) {
            sum += item.second;
        }
    }

    cout << sum << endl;
}

TEST(Day7, Part2) {
    ifstream input;
    input.open("../../test/input/day7.txt");

    auto aggregateSize = buildAggregateSize(input);

    vector<pair<string, int>> pairs;
    copy(aggregateSize.begin(), aggregateSize.end(), back_inserter(pairs));

    sort(pairs.begin(), pairs.end(), [](auto a, auto b) { return a.second < b.second; });

    auto used = aggregateSize["/"];
    auto unused = 70000000 - used;
    auto needed = 30000000;
    auto neededToFree = needed - unused;

    auto found = find_if(pairs.begin(), pairs.end(), [&](auto a) { return a.second >= neededToFree; });
    if (found == pairs.end()) {
        throw logic_error("not found");
    }

    cout << found->second << endl;
}
