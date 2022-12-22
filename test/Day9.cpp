#include <gtest/gtest.h>
#include <fstream>
#include <regex>

using namespace std;

namespace day9 {

    const regex line_regex(R"((\w) (\d+))");

    struct point {
        int x;
        int y;
    };

    point operator-(const point &a, const point &b) {
        return point{a.x - b.x, a.y - b.y};
    }

    bool operator==(const point &a, const point &b) {
        return a.x == b.x && a.y == b.y;
    }

    bool operator<(const point &a, const point &b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }

    point update_head(point head_pos, string theDir) {
        point new_head_pos = head_pos;
        if (theDir == "U") {
            new_head_pos.y++;
        } else if (theDir == "R") {
            new_head_pos.x++;
        } else if (theDir == "D") {
            new_head_pos.y--;
        } else if (theDir == "L") {
            new_head_pos.x--;
        }
        return new_head_pos;
    }

    point update_tail(point head_pos, point tail_pos) {
        point new_tail_pos = tail_pos;

        point difference = head_pos - new_tail_pos;

        if (abs(difference.x) > 1 || abs(difference.y) > 1) {
            if (difference.x > 0) {
                new_tail_pos.x++;
            }
            if (difference.x < 0) {
                new_tail_pos.x--;
            }
            if (difference.y > 0) {
                new_tail_pos.y++;
            }
            if (difference.y < 0) {
                new_tail_pos.y--;
            }
        }

        return new_tail_pos;
    }

    TEST(Day9, update_tail) {
        EXPECT_EQ(update_tail(point{1, 0}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{1, 1}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{0, 1}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{-1, 1}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{-1, 0}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{-1, -1}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{0, -1}, point{0, 0}), (point{0, 0}));
        EXPECT_EQ(update_tail(point{1, -1}, point{0, 0}), (point{0, 0}));

        EXPECT_EQ(update_tail(point{2, 0}, point{0, 0}), (point{1, 0}));
        EXPECT_EQ(update_tail(point{-2, 0}, point{0, 0}), (point{-1, 0}));
        EXPECT_EQ(update_tail(point{0, 2}, point{0, 0}), (point{0, 1}));
        EXPECT_EQ(update_tail(point{0, -2}, point{0, 0}), (point{0, -1}));

        EXPECT_EQ(update_tail(point{1, 2}, point{0, 0}), (point{1, 1}));
        EXPECT_EQ(update_tail(point{2, 1}, point{0, 0}), (point{1, 1}));
        EXPECT_EQ(update_tail(point{2, 2}, point{0, 0}), (point{1, 1}));

        EXPECT_EQ(update_tail(point{-1, -2}, point{0, 0}), (point{-1, -1}));
        EXPECT_EQ(update_tail(point{-2, -1}, point{0, 0}), (point{-1, -1}));
        EXPECT_EQ(update_tail(point{-2, -2}, point{0, 0}), (point{-1, -1}));
    }

    TEST(Day9, Part1) {
        ifstream input;
        input.open("../../test/input/day9.txt");
//    stringstream input("R 4\n"
//                       "U 4\n"
//                       "L 3\n"
//                       "D 1\n"
//                       "R 4\n"
//                       "D 1\n"
//                       "L 5\n"
//                       "R 2");

        point head_pos{};
        point tail_pos{};
        set<point> tailPositions;
        tailPositions.insert(point{});

        while (true) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }

            smatch result;
            if (!regex_match(line, result, line_regex)) {
                throw logic_error("didn't match");
            }
            auto dir = result[1].str();
            auto dist = stoi(result[2].str());

            for (auto i = 0; i < dist; ++i) {
                head_pos = update_head(head_pos, dir);
                tail_pos = update_tail(head_pos, tail_pos);
                tailPositions.insert(tail_pos);
            }
        }

        cout << tailPositions.size() << endl;
    }

    TEST(Day9, Part2) {
        ifstream input;
        input.open("../../test/input/day9.txt");
//    stringstream input("R 4\n"
//                       "U 4\n"
//                       "L 3\n"
//                       "D 1\n"
//                       "R 4\n"
//                       "D 1\n"
//                       "L 5\n"
//                       "R 2");
//    stringstream input("R 5\n"
//                       "U 8\n"
//                       "L 8\n"
//                       "D 3\n"
//                       "R 17\n"
//                       "D 10\n"
//                       "L 25\n"
//                       "U 20");

        vector<point> knotPositions(10);
        set<point> tailPositions;
        tailPositions.insert(point{});

        while (true) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }

            smatch result;
            if (!regex_match(line, result, line_regex)) {
                throw logic_error("didn't match");
            }
            auto dir = result[1].str();
            auto dist = stoi(result[2].str());

            for (auto i = 0; i < dist; ++i) {
                knotPositions[0] = update_head(knotPositions[0], dir);
                for (auto j = 1; j < knotPositions.size(); ++j) {
                    knotPositions[j] = update_tail(knotPositions[j - 1], knotPositions[j]);
                }
                tailPositions.insert(knotPositions.back());
            }
        }

        cout << tailPositions.size() << endl;
    }

}