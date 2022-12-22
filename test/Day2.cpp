#include <gtest/gtest.h>
#include <fstream>

namespace day2 {

    enum move {
        rock = 0,
        paper = 1,
        scissors = 2
    };

    enum outcome {
        loss = 0,
        tie = 1,
        win = 2
    };

    move to_move(char c) {
        switch (c) {
            case 'A':
            case 'X':
                return rock;
            case 'B':
            case 'Y':
                return paper;
            case 'C':
            case 'Z':
                return scissors;
            default:
                throw std::logic_error("invalid");
        }
    }

    outcome to_outcome(char c) {
        switch (c) {
            case 'X':
                return loss;
            case 'Y':
                return tie;
            case 'Z':
                return win;
            default:
                throw std::logic_error("invalid");
        }
    }

    int move_score(move m) {
        return m + 1;
    }

    outcome match_outcome(move opp_move, move my_move) {
        return outcome((my_move - opp_move + 4) % 3);
    }

    move find_move(move opp_move, outcome desired_outcome) {
        return move(((desired_outcome + 2) + opp_move) % 3);
    }

    int win_score(move opp_move, move my_move) {
        auto result = match_outcome(opp_move, my_move);
        return result * 3;
    }

    int total_score(move opp_move, move my_move) {
        return move_score(my_move) + win_score(opp_move, my_move);
    }

    TEST(Day2, win_score) {
        ASSERT_EQ(win_score(rock, rock), 3);
        ASSERT_EQ(win_score(rock, paper), 6);
        ASSERT_EQ(win_score(rock, scissors), 0);
        ASSERT_EQ(win_score(paper, rock), 0);
        ASSERT_EQ(win_score(paper, paper), 3);
        ASSERT_EQ(win_score(paper, scissors), 6);
        ASSERT_EQ(win_score(scissors, rock), 6);
        ASSERT_EQ(win_score(scissors, paper), 0);
        ASSERT_EQ(win_score(scissors, scissors), 3);
    }

    TEST(Day2, total_score) {
        ASSERT_EQ(total_score(rock, rock), 4);
        ASSERT_EQ(total_score(rock, paper), 8);
        ASSERT_EQ(total_score(rock, scissors), 3);
        ASSERT_EQ(total_score(paper, rock), 1);
        ASSERT_EQ(total_score(paper, paper), 5);
        ASSERT_EQ(total_score(paper, scissors), 9);
        ASSERT_EQ(total_score(scissors, rock), 7);
        ASSERT_EQ(total_score(scissors, paper), 2);
        ASSERT_EQ(total_score(scissors, scissors), 6);
    }

    TEST(Day2, find_move) {
        ASSERT_EQ(find_move(rock, loss), scissors);
        ASSERT_EQ(find_move(rock, tie), rock);
        ASSERT_EQ(find_move(rock, win), paper);
        ASSERT_EQ(find_move(paper, loss), rock);
        ASSERT_EQ(find_move(paper, tie), paper);
        ASSERT_EQ(find_move(paper, win), scissors);
        ASSERT_EQ(find_move(scissors, loss), paper);
        ASSERT_EQ(find_move(scissors, tie), scissors);
        ASSERT_EQ(find_move(scissors, win), rock);
    }

    TEST(Day2, Part1) {
        std::ifstream input;
        input.open("../../test/input/day2.txt");
        long sum = 0;
        while (input) {
            char opponent_move;
            char my_move;
            input >> opponent_move >> my_move;
            if (!input) {
                break;
            }
            auto incremental_score = total_score(to_move(opponent_move), to_move(my_move));
            std::cout << opponent_move << "  " << my_move << "  " << incremental_score << "\n";
            sum += incremental_score;
        }

        std::cout << sum << "\n";
    }

    TEST(Day2, Part2) {
        std::ifstream input;
        input.open("../../test/input/day2.txt");
        std::string line;
        auto sum = 0;
        std::vector<int> counts;
        while (input) {
            char opponent_move;
            char outcome;
            input >> opponent_move >> outcome;
            if (!input) {
                break;
            }
            auto my_move = find_move(to_move(opponent_move), to_outcome(outcome));
            sum += total_score(to_move(opponent_move), my_move);
        }

        std::cout << sum << "\n";
    }

}