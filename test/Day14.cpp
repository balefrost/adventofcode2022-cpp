#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include "position.h"

using namespace std;

const string sample_input = "498,4 -> 498,6 -> 496,6\n"
                            "503,4 -> 502,4 -> 502,9 -> 494,9";

const regex coord_regex("(\\d+),(\\d+)");

position convert_position(const smatch &results) {
    int x = stoi(results[1]);
    int y = stoi(results[2]);
    return {x, y};
}

void draw_line(map<position, char> &field, const position &start, const position &end) {
    if (start.x == end.x) {
        int miny = min(start.y, end.y);
        int maxy = max(start.y, end.y);
        for (auto y = miny; y <= maxy; ++y) {
            field[{start.x, y}] = '#';
        }
    } else if (start.y == end.y) {
        int minx = min(start.x, end.x);
        int maxx = max(start.x, end.x);
        for (auto x = minx; x <= maxx; ++x) {
            field[{x, start.y}] = '#';
        }
    } else {
        throw logic_error("non-ortho line");
    }
}

template<typename K, typename V>
V get_or_default(const map<K, V> &map, const K &key, const V &def) {
    auto iter = map.find(key);
    return iter == map.end() ? def : iter->second;
}

map<position, char> parse_field(istream &input) {
    map<position, char> field;

    while (true) {
        string line;
        getline(input, line);
        if (!input) {
            break;
        }

        sregex_iterator iter(line.begin(), line.end(), coord_regex);
        sregex_iterator end;

        if (iter == end) {
            throw logic_error("no pos");
        }

        auto current_position = convert_position(*iter);
        ++iter;

        for (; iter != end; ++iter) {
            auto next_position = convert_position(*iter);
            draw_line(field, current_position, next_position);
            current_position = next_position;
        }
    }

    return field;
}

pair<position, position> compute_bounds(const map<position, char> &field) {
    int minx = std::numeric_limits<int>::max();
    int miny = std::numeric_limits<int>::max();
    int maxx = std::numeric_limits<int>::min();
    int maxy = std::numeric_limits<int>::min();
    for (const auto &item: field) {
        minx = min(minx, item.first.x);
        miny = min(miny, item.first.y);
        maxx = max(maxx, item.first.x);
        maxy = max(maxy, item.first.y);
    }

    return {{minx, miny},
            {maxx, maxy}};
}

void print_field(ostream &out, const map<position, char> &field) {
    auto bounds = compute_bounds(field);
    int minx = bounds.first.x;
    int miny = bounds.first.y;
    int maxx = bounds.second.x;
    int maxy = bounds.second.y;

    for (auto y = miny; y <= maxy; ++y) {
        for (auto x = minx; x <= maxx; ++x) {
            auto iter = field.find({x, y});
            out << get_or_default(field, {x, y}, '.');
        }
        out << endl;
    }
}

// Returns false if sand goes OOB
bool drop_sand(map<position, char> &field, position sand_pos, int oob_y) {
    while (true) {
        auto pos_down = sand_pos + position{0, 1};
        auto pos_dl = sand_pos + position{-1, 1};
        auto pos_dr = sand_pos + position{1, 1};
        if (get_or_default(field, pos_down, '.') == '.') {
            sand_pos = pos_down;
        } else if (get_or_default(field, pos_dl, '.') == '.') {
            sand_pos = pos_dl;
        } else if (get_or_default(field, pos_dr, '.') == '.') {
            sand_pos = pos_dr;
        } else {
            field[sand_pos] = 'o';
            return true;
        }
        if (sand_pos.y >= oob_y) {
            return false;
        }
    }
}

// Returns the position that the sand comes to a stop
position drop_sand2(map<position, char> &field, position sand_pos, int floor_y) {
    while (true) {
        auto pos_down = sand_pos + position{0, 1};
        auto pos_dl = sand_pos + position{-1, 1};
        auto pos_dr = sand_pos + position{1, 1};
        if (get_or_default(field, pos_down, '.') == '.') {
            sand_pos = pos_down;
        } else if (get_or_default(field, pos_dl, '.') == '.') {
            sand_pos = pos_dl;
        } else if (get_or_default(field, pos_dr, '.') == '.') {
            sand_pos = pos_dr;
        } else {
            field[sand_pos] = 'o';
            return sand_pos;
        }
        if (sand_pos.y == floor_y - 1) {
            field[sand_pos] = 'o';
            return sand_pos;
        }
    }
}

TEST(Day14, Part1) {
    ifstream input;
    input.open("../../test/input/day14.txt");
//    stringstream input(sample_input);

    map<position, char> field = parse_field(input);

    auto bounds = compute_bounds(field);
    int count = 0;
    while (true) {
        if (!drop_sand(field, position{500, 0}, bounds.second.y + 1)) {
            break;
        }
        count++;
//        print_field(cout, field);
//        cout << endl;
    }
    cout << count << endl;
}

TEST(Day14, Part2) {
    ifstream input;
    input.open("../../test/input/day14.txt");
//    stringstream input(sample_input);

    map<position, char> field = parse_field(input);

    const position spout = position{500, 0};
    auto bounds = compute_bounds(field);
    int count = 0;
    while (true) {
        auto rest_pos = drop_sand2(field, spout, bounds.second.y + 2);
        count++;
//        cout << count << endl;
//        print_field(cout, field);
//        cout << endl;
        if (rest_pos == spout) {
            break;
        }
    }
    cout << count << endl;
}
