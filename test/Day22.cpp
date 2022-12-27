#include <gtest/gtest.h>
#include <fstream>
#include <regex>

using namespace std;

namespace day22 {

    const string sample_input = "        ...#\n"
                                "        .#..\n"
                                "        #...\n"
                                "        ....\n"
                                "...#.......#\n"
                                "........#...\n"
                                "..#....#....\n"
                                "..........#.\n"
                                "        ...#....\n"
                                "        .....#..\n"
                                "        .#......\n"
                                "        ......#.\n"
                                "\n"
                                "10R5L5R10L4R5L5";

    const regex directions_regex("(?:(\\d+)|([RL]))");

    /**
     * Assumes Y increases top-to-bottom; X increases left-to-right (i.e. screen coords, not math coords)
     */
    struct pos2 {
        int x;
        int y;

        pos2 rotate_cw_about_origin() {
            return {-y, x};
        }

        pos2 rotate_ccw_about_origin() {
            return {y, -x};
        }

        pos2 operator+(const pos2 &b) const {
            return {x + b.x, y + b.y};
        }

        pos2 &operator+=(const pos2 &b) {
            x += b.x;
            y += b.y;
            return *this;
        }

        pos2 operator-() const {
            return {-x, -y};
        }

        pos2 operator-(const pos2 &b) const {
            return {x - b.x, y - b.y};
        }

        pos2 &operator-=(const pos2 &b) {
            x -= b.x;
            y -= b.y;
            return *this;
        }
    };

    ostream &operator<<(ostream &out, const pos2 &p) {
        out << "{" << p.x << "," << p.y << "}";
        return out;
    }

    char get_map(const vector<string> &map, pos2 pos) {
        if (pos.y < 0 || pos.y >= map.size()) {
            return ' ';
        }
        const auto &line = map[pos.y];
        if (pos.x < 0 || pos.x >= line.size()) {
            return ' ';
        }
        return line[pos.x];
    }

    pair<pos2, char> find_adjacent(const vector<string> &map, pos2 pos, pos2 dir) {
        pos2 new_pos = pos + dir;
        char ch = get_map(map, new_pos);
        if (ch != ' ') {
            return {new_pos, ch};
        } else {
            auto scanpos = pos - dir;
            while (get_map(map, scanpos) != ' ') {
                scanpos -= dir;
            }
            scanpos += dir;
            return {scanpos, get_map(map, scanpos)};
        }
    }

    string dir_str(pos2 dir) {
        if (dir.x > 0) {
            return "E";
        } else if (dir.x < 0) {
            return "W";
        } else if (dir.y < 0) {
            return "N";
        } else {
            return "S";
        }
    }

    TEST(Day22, Part1) {
        ifstream input;
        input.open("../../test/input/day22.txt");
//        stringstream input(sample_input);

        vector<string> map;
        string line;
        while (getline(input, line)) {
            if (line.empty()) {
                break;
            }
            map.push_back(line);
        }
        string directions_line;
        getline(input, directions_line);

        auto iter = std::find_if(map[0].cbegin(), map[0].cend(), [](auto ch) { return ch != ' '; });
        pos2 my_pos{(int) (iter - map[0].begin()), 0};
        pos2 my_dir{1, 0};

        smatch mr;
        auto current = directions_line.cbegin();
        while (regex_search(current, directions_line.cend(), mr, directions_regex)) {
            if (mr[1].matched) {
//                cout << "moving straight " << mr[1] << endl;
                for (auto i = 0; i < stoi(mr[1]); ++i) {
                    auto [newpos, ch] = find_adjacent(map, my_pos, my_dir);
                    if (ch == '#') {
//                        cout << "  hit wall at " << newpos << "  " << dir_str(my_dir) << "  " << my_dir << endl;
                        break;
                    }
//                    cout << "  moved to " << newpos << "  " << dir_str(my_dir) << "  " << my_dir << endl;
                    my_pos = newpos;
                }
            } else {
//                cout << "turning " << mr[2] << endl;
                if (mr[2] == "R") {
                    my_dir = my_dir.rotate_cw_about_origin();
                } else {
                    my_dir = my_dir.rotate_ccw_about_origin();
                }
            }
            current = mr.suffix().first;
        }

        auto facing_str = dir_str(my_dir);
        auto facing_amt = facing_str == "E" ? 0 :
                          facing_str == "S" ? 1 :
                          facing_str == "W" ? 2 :
                          3;
        cout << (my_pos.y + 1) * 1000 + (my_pos.x + 1) * 4 + facing_amt;
    }

    TEST(Day22, Part2) {
        ifstream input;
        input.open("../../test/input/day22.txt");
        //stringstream input(sample_input);
    }

}