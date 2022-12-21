#include <gtest/gtest.h>
#include <fstream>

#include "util.h"

using namespace std;

const string sample_input = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";

const vector<string> sym_minus = {"####"};
const vector<string> sym_plus = {" # ",
                                 "###",
                                 " # "};
const vector<string> sym_back_l = {"  #",
                                   "  #",
                                   "###"};
const vector<string> sym_vert_i = {"#",
                                   "#",
                                   "#",
                                   "#"};
const vector<string> sym_square = {"##",
                                   "##"};

const vector<vector<string>> pieces{sym_minus, sym_plus, sym_back_l, sym_vert_i, sym_square};

template<typename T>
T sign(T value) {
    return value < 0 ? -1 :
           value > 0 ? 1 :
           0;
}

bool board_get(const vector<byte> &board, size_t bx, size_t by) {
    return ((board[by] >> bx) & (byte) 0x01) != (byte) 0x00;
}

void board_set(vector<byte> &board, size_t bx, size_t by, bool value) {
    if (value) {
        board[by] |= (byte) (1 << bx);
    } else {
        board[by] &= (byte) (~(1 << bx));
    }
}

bool iterate_piece(const vector<string> &piece,
                   size_t px, size_t py,
                   const function<bool(size_t bx, size_t by, size_t px, size_t py)> &f) {
    for (auto r = 0; r < piece.size(); ++r) {
        auto br = py + r;
        size_t pr = piece.size() - 1 - r;
        auto &piece_row = piece[piece.size() - 1 - r];
        for (auto c = 0; c < piece[r].size(); ++c) {
            auto bc = px + c;
            auto piece_symbol = piece_row[c];
            if (piece_symbol == '#') {
                if (!f(bc, br, c, pr)) {
                    return false;
                }
            }
        }
    }
    return true;
}

void print_top_of_board(const vector<byte> &board, int n) {
    for (auto i = 0; i < n; ++i) {
        if (i >= board.size()) {
            break;
        }
        auto row = board.size() - 1 - i;
        for (auto x = 0; x < 7; ++x) {
            cout << (board_get(board, x, row) ? '#' : '.');
        }
        cout << endl;
    }
}

bool can_move(const vector<byte> &board,
              const vector<string> &piece,
              size_t px, size_t py,
              int dx, int dy) {
    return iterate_piece(piece, px, py, [&board, dx, dy](int bx, int by, int px, int py) {
        auto tbx = bx + sign(dx);
        auto tby = by + sign(dy);
        if (tby < 0 || tbx < 0 || tbx >= 7) {
            return false;
        }
        if (board_get(board, tbx, tby)) {
            return false;
        }
        return true;
    });
}

void drop_piece(vector<byte> &board,
                const vector<string> &piece,
                const string &wind,
                size_t &top,
                size_t &wind_index) {
    auto py = top + 3;
    auto px = 2;
    auto curr_sym_height = piece.size();
    auto needed_rows = top + 3 + curr_sym_height;
    while (board.size() < needed_rows) {
        board.push_back((byte) 0);
    }

    while (true) {
        auto shift_dir = wind[wind_index];
        wind_index = (wind_index + 1) % wind.size();
        auto dx = shift_dir == '<' ? -1 : 1;
        if (can_move(board, piece, px, py, dx, 0)) {
            px += dx;
        }

        if (can_move(board, piece, px, py, 0, -1)) {
            py -= 1;
        } else {
            iterate_piece(piece, px, py, [&](size_t bx, size_t by, size_t px, size_t py) {
                top = max(top, by + 1);
                board_set(board, bx, by, true);
                return true;
            });
            break;
        }
    }
}

TEST(Day17, Part1) {
    ifstream input;
    input.open("../../test/input/day17.txt");
//    stringstream input(sample_input);

    string wind;
    getline(input, wind);

    vector<byte> board;
    size_t top = 0;

    size_t windex = 0;
    for (auto i = 0; i < 2022; ++i) {
        auto &curr_piece = pieces[i % pieces.size()];
        drop_piece(board, curr_piece, wind, top, windex);
    }

    cout << top << endl;
}

void compress_board(vector<byte> &board, size_t &top) {
    board.erase(board.begin() + top, board.end());

    if (top == 0) {
        return;
    }

    typedef pair<size_t, size_t> pos;
    set<pos> frontier;
    set<pos> seen;
    for (auto i = 0; i < 7; ++i) {
        if (!board_get(board, i, top - 1)) {
            frontier.insert({i, top - 1});
        }
    }
    auto lowest = top - 1;
    while (!frontier.empty()) {
        auto f = *frontier.begin();
        frontier.erase(frontier.begin());
        if (seen.find(f) == seen.end()) {
            seen.insert(f);
            lowest = min(lowest, f.second);
            if (f.first > 0) {
                if (!board_get(board, f.first - 1, f.second)) {
                    frontier.insert({f.first - 1, f.second});
                }
            }
            if (f.first < 6) {
                if (!board_get(board, f.first + 1, f.second)) {
                    frontier.insert({f.first + 1, f.second});
                }
            }
            if (f.second > 0) {
                if (!board_get(board, f.first, f.second - 1)) {
                    frontier.insert({f.first, f.second - 1});
                }
            }
        }
    }

    if (lowest > 0) {
        board.erase(board.begin(), board.begin() + lowest);
    }

    top -= lowest;
}

template<typename T>
struct item_comparer {
    vector<function<strong_ordering(const T &, const T &)>> comparisons;

    struct less_comparer_func {
        vector<function<strong_ordering(const T &, const T &)>> comparisons;

        bool operator()(const T &a, const T &b) {
            for (const auto &item: comparisons) {
                auto comparison_result = item(a, b);
                if (comparison_result == strong_ordering::less) {
                    return true;
                } else if (comparison_result == strong_ordering::greater) {
                    return false;
                }
            }

            return false;
        }
    };

    template<typename R>
    item_comparer &then_by(function<R(T)> f) {
        comparisons.push_back([f](const T &a, const T &b) {
            return f(a) <=> f(b);
        });
        return *this;
    }

    template<typename R>
    item_comparer &then_by(R T::* f) {
        comparisons.push_back([f](const T &a, const T &b) {
            return a.*f <=> b.*f;
        });
        return *this;
    }

    less_comparer_func as_less() {
        return less_comparer_func{comparisons};
    }
};

TEST(Day17, Part2) {
    ifstream input;
    input.open("../../test/input/day17.txt");
//    stringstream input(sample_input);

    string wind;
    getline(input, wind);

    vector<byte> board;

    struct state {
        int pindex;
        size_t windex;
        vector<byte> board;
        size_t top;
        size_t height_increased_from_last_state;
    };

    auto state_comparer = item_comparer<state>()
            .then_by(&state::pindex)
            .then_by(&state::windex)
            .then_by(&state::board)
            .then_by(&state::top)
            .then_by(&state::height_increased_from_last_state)
            .as_less();

    auto p = iterate_until_cycle<state>(
            state{0, 0, board, 0, 0},
            [&board, &wind](state s) {
                auto &curr_piece = pieces[s.pindex];
                s.pindex = (s.pindex + 1) % pieces.size();
                auto old_top = s.top;
                drop_piece(board, curr_piece, wind, s.top, s.windex);
                s.height_increased_from_last_state = s.top - old_top;
                compress_board(board, s.top);
                return s;
            },
            state_comparer);

    cout << p.first.size() << "  " << p.second.size() << endl;

    unsigned long long remaining_pieces = 1000000000000;
    unsigned long long total_height = 0;
    unsigned long long height_for_prefix = 0;
    for (const auto &item: p.first) {
        height_for_prefix += item.height_increased_from_last_state;
    }

    unsigned long long height_for_cycle = 0;
    for (const auto &item: p.second) {
        height_for_cycle += item.height_increased_from_last_state;
    }

    total_height += height_for_prefix;
    remaining_pieces -= max((size_t) 0, p.first.size() - 1);  //the initial state is included in p.first

    auto total_cycles = remaining_pieces / p.second.size();
    total_height += total_cycles * height_for_cycle;
    remaining_pieces -= total_cycles * p.second.size();

    auto iter = p.second.begin();
    for (auto i = 0; i < remaining_pieces; ++i) {
        total_height += p.second[i].height_increased_from_last_state;
        ++iter;
    }

    cout << total_height << endl;
}
