#include <gtest/gtest.h>
#include <fstream>
#include <unordered_set>

using namespace std;

struct cell {
    int height;
    int cost;
};

struct pos {
    int r;
    int c;
};

ostream& operator<<(ostream& out, const pos& thePos) {
    out << "{r:" << thePos.r << ", c:" << thePos.c << "}";
    return out;
}

bool operator<(const pos &a, const pos &b) {
    return a.r < b.r || (a.r == b.r && a.c < b.c);
}

pos operator+(const pos &a, const pos &b) {
    return { a.r + b.r, a.c + b.c };
}

//template<>
//struct std::hash<pos> {
//    std::size_t operator()(const pos& p) const noexcept {
//        return p.r * 31 + p.c;
//    }
//};

vector<pos> get_adjacent(const pos &p, size_t h, size_t w) {
    vector<pos> result;
    if (p.r > 0) {
        result.push_back(p + pos {-1, 0 });
    }
    if (p.r < h - 1) {
        result.push_back(p + pos {1, 0 });
    }
    if (p.c > 0) {
        result.push_back(p + pos {0, -1 });
    }
    if (p.c < w - 1) {
        result.push_back(p + pos {0, 1 });
    }
    return result;
}

const string sample_input = "Sabqponm\n"
                            "abcryxxl\n"
                            "accszExk\n"
                            "acctuvwj\n"
                            "abdefghi";

struct parsed_input {
    vector<vector<cell>> cells;
    pos start;
    pos target;
};

parsed_input parse_input(istream& input) {
    vector<vector<cell>> cells;

    int row = 0;
    pos start {};
    pos target {};
    while (true) {
        string line;
        getline(input, line);
        if (!input) {
            break;
        }

        vector<cell> line_heights;

        for (int i = 0; i < line.length(); ++i) {
            auto& ch = line[i];
            int height;
            int cost = numeric_limits<int>::max();
            if (ch == 'S') {
                start = pos { row, i };
                height = 0;
            } else if (ch == 'E') {
                target = pos { row, i };
                height = 25;
            } else {
                height = ch - 'a';
            }
            line_heights.push_back({ height, cost});
        }
        cells.push_back(line_heights);
        ++row;
    }

    return { cells, start, target };
}

TEST(Day12, Part1) {
    ifstream input;
    input.open("../../test/input/day12.txt");
//    stringstream input(sample_input);

    auto parsed_input = parse_input(input);
    auto& cells = parsed_input.cells;

    cells[parsed_input.start.r][parsed_input.start.c].cost = 0;

    size_t w = cells.front().size();
    size_t h = cells.size();
    set<pos> frontier { parsed_input.start };

    int step_no = 0;

    while (!frontier.empty()) {

        step_no++;

        set<pos> new_frontier;

        for (const auto &frnt_pos: frontier) {
            cell &frnt_cell = cells[frnt_pos.r][frnt_pos.c];
            const vector<pos> adjacent = get_adjacent(frnt_pos, h, w);
            for (const auto &adj_pos: adjacent) {
                cell& adj_cell = cells.at(adj_pos.r).at(adj_pos.c);
                if (adj_cell.height - frnt_cell.height <= 1 && step_no < adj_cell.cost) {
                    adj_cell.cost = step_no;
                    new_frontier.insert(adj_pos);
                }
            }
        }

        swap(frontier, new_frontier);
    }

    cout << cells[parsed_input.target.r][parsed_input.target.c].cost << endl;
}

int do_search(vector<vector<cell>> cells, pos target) {
    size_t w = cells.front().size();
    size_t h = cells.size();
    set<pos> frontier { target };

    int step_no = 0;

    while (!frontier.empty()) {

        step_no++;

        set<pos> new_frontier;

        for (const auto &frnt_pos: frontier) {
            cell &frnt_cell = cells[frnt_pos.r][frnt_pos.c];
            const vector<pos> adjacent = get_adjacent(frnt_pos, h, w);
            for (const auto &adj_pos: adjacent) {
                cell& adj_cell = cells.at(adj_pos.r).at(adj_pos.c);
                if (frnt_cell.height - adj_cell.height <= 1 && step_no < adj_cell.cost) {
                    if (adj_cell.height == 0) {
                        return step_no;
                    }
                    adj_cell.cost = step_no;
                    new_frontier.insert(adj_pos);
                }
            }
        }

        swap(frontier, new_frontier);
    }

    throw runtime_error("did not find hike");
}

TEST(Day12, Part2) {
    ifstream input;
    input.open("../../test/input/day12.txt");
//    stringstream input(sample_input);

    auto parsed_input = parse_input(input);
    parsed_input.cells[parsed_input.target.r][parsed_input.target.c].cost = 0;

    int steps = do_search(parsed_input.cells, parsed_input.target);

    cout << steps << endl;}
