#include <gtest/gtest.h>
#include <fstream>

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

bool iterate_piece(const vector<string> &piece,
                   int px, int py,
                   function<bool(int bx, int by, int px, int py)> f) {
    for (auto r = 0; r < piece.size(); ++r) {
        auto br = py + r;
        auto pr = piece.size() - 1 - r;
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

void print_top_of_board(const vector<string> &board, int n) {
    for (auto i = 0; i < n; ++i) {
        if (i >= board.size()) {
            break;
        }
        auto row = board.size() - 1 - i;
        cout << board[row] << endl;
    }
}

bool can_move(const vector<string> &board,
              const vector<string> &piece,
              int px, int py,
              int dx, int dy) {
    return iterate_piece(piece, px, py, [&board, dx, dy](int bx, int by, int px, int py){
        auto tbx = bx + sign(dx);
        auto tby = by + sign(dy);
        auto& board_row = board[tby];
        if (tby < 0 || tbx < 0 || tbx >= board.front().size()) {
            return false;
        }
        if (board_row[tbx] == '#') {
            return false;
        }
        return true;
    });
}

TEST(Day17, Part1) {
    ifstream input;
    input.open("../../test/input/day17.txt");
//    stringstream input(sample_input);

    string wind;
    getline(input, wind);

    vector<string> board;
    auto top = -1;

    size_t j = 0;
    for (auto i = 0; i < 2022; ++i) {
        auto &curr_piece = pieces[i % pieces.size()];
        auto curr_sym_height = curr_piece.size();
        auto needed_rows = top + 4 + curr_sym_height;
        while (board.size() < needed_rows) {
            board.emplace_back(7, '.');
        }

        auto piece_row = top + 4;
        auto piece_col = 2;

        while (true) {
            auto shift_dir = wind[j];
            j = (j + 1) % wind.size();
            auto dx = shift_dir == '<' ? -1 : 1;
            if (can_move(board, curr_piece, piece_col, piece_row, dx, 0)) {
                piece_col += dx;
            }

            if (can_move(board, curr_piece, piece_col, piece_row, 0, -1)) {
                piece_row -= 1;
            } else {
                iterate_piece(curr_piece, piece_col, piece_row, [&](int bx, int by, int px, int py){
                    top = max(top, by);
                    board[by][bx] = curr_piece[py][px];
                    return true;
                });
//                print_top_of_board(board, 10);
//                cout << endl;
                break;
            }
        }
    }

//    cout << "count: " << std::count_if(board.begin(), board.end(), [](auto s) { return s == "#######"; }) << endl;

    cout << (top + 1) << endl;
}

TEST(Day17, Part2) {
    ifstream input;
    input.open("../../test/input/day17.txt");
    //stringstream input(sample_input);
}
