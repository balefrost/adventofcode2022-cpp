#include <gtest/gtest.h>
#include <deque>
#include <fstream>

using namespace std;

namespace day13 {

    int compare_ints(int a, int b) {
        return a < b ? -1 :
               a > b ? 1 :
               0;
    }

    enum token_type {
        lbracket_token,
        rbracket_token,
        number_token
    };

    struct token {
        token_type type;
        int value;

        bool operator==(const token &b) const {
            return type == b.type && value == b.value;
        }
    };

    vector<token> tokenize(istream &input) {
        vector<token> result;

        while (true) {
            while (input.peek() == ' ' || input.peek() == ',') {
                input.get();
            }
            if (!input) {
                break;
            }
            if (input.peek() == '[') {
                result.push_back(token{lbracket_token, 0});
                input.get();
            } else if (input.peek() == ']') {
                result.push_back(token{rbracket_token, 0});
                input.get();
            } else if (input.peek() >= '0' && input.peek() <= '9') {
                int value;
                input >> value;
                result.push_back(token{number_token, value});
            }
        }

        return result;
    }

    typedef vector<token>::const_iterator token_iter;
    typedef pair<token_iter, token_iter> token_range;

    token_range extract_list(vector<token>::const_iterator &iter) {
        vector<token>::const_iterator start;
        vector<token>::const_iterator end;
        if (iter->type == lbracket_token) {
            ++iter;
            start = iter;

            int depth = 1;
            while (depth > 0) {
                if (iter->type == lbracket_token) {
                    depth += 1;
                } else if (iter->type == rbracket_token) {
                    depth -= 1;
                    end = iter;
                }
                ++iter;
            }
        } else if (iter->type == number_token) {
            start = iter;
            ++iter;
            end = iter;
        } else {
            throw runtime_error("bad token");
        }

        return {start, end};
    }

    int compare_tokens(const token_range &a, const token_range &b) {
        auto aiter = a.first;
        auto biter = b.first;

        while (aiter != a.second && biter != b.second) {
            if (aiter->type == number_token && biter->type == number_token) {
                auto comparison = compare_ints(aiter->value, biter->value);
                if (comparison != 0) {
                    return comparison;
                } else {
                    ++aiter;
                    ++biter;
                }
            } else {
                auto alist = extract_list(aiter);
                auto blist = extract_list(biter);
                auto comparison = compare_tokens(alist, blist);
                if (comparison != 0) {
                    return comparison;
                }
            }
        }

        if (aiter == a.second && biter == b.second) {
            return 0;
        } else if (aiter == a.second) {
            return -1;
        } else {
            return 1;
        }
    }

    const string sample_input = "[1,1,3,1,1]\n"
                                "[1,1,5,1,1]\n"
                                "\n"
                                "[[1],[2,3,4]]\n"
                                "[[1],4]\n"
                                "\n"
                                "[9]\n"
                                "[[8,7,6]]\n"
                                "\n"
                                "[[4,4],4,4]\n"
                                "[[4,4],4,4,4]\n"
                                "\n"
                                "[7,7,7,7]\n"
                                "[7,7,7]\n"
                                "\n"
                                "[]\n"
                                "[3]\n"
                                "\n"
                                "[[[]]]\n"
                                "[[]]\n"
                                "\n"
                                "[1,[2,[3,[4,[5,6,7]]]],8,9]\n"
                                "[1,[2,[3,[4,[5,6,0]]]],8,9]";

    TEST(Day13, Part1) {
        ifstream input;
        input.open("../../test/input/day13.txt");
//    stringstream input(sample_input);

        int sum_indices_right_order = 0;

        int current_pair_index = 1;
        while (true) {
            string line1, line2;
            getline(input, line1);
            getline(input, line2);
            if (!input) {
                break;
            }
            string junk;
            getline(input, junk);

            auto ss1 = stringstream(line1);
            auto ss2 = stringstream(line2);
            auto tokens1 = tokenize(ss1);
            auto tokens2 = tokenize(ss2);

            auto comparison = compare_tokens({tokens1.begin(), tokens1.end()}, {tokens2.begin(), tokens2.end()});
            if (comparison == -1) {
                sum_indices_right_order += current_pair_index;
            }
            ++current_pair_index;
        }

        cout << sum_indices_right_order << endl;
    }

    vector<token> divider_packet(int value) {
        return vector<token>{
                token{lbracket_token, 0},
                token{lbracket_token, 0},
                token{number_token, value},
                token{rbracket_token, 0},
                token{rbracket_token, 0}
        };
    }

    TEST(Day13, Part2) {
        ifstream input;
        input.open("../../test/input/day13.txt");
//    stringstream input(sample_input);

        vector<vector<token>> all_tokens{
                divider_packet(2),
                divider_packet(6)
        };

        while (true) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }

            if (line.empty()) {
                continue;
            }

            auto ss = stringstream(line);
            all_tokens.push_back(tokenize(ss));
        }

        sort(all_tokens.begin(), all_tokens.end(), [](auto a, auto b) {
            return compare_tokens({a.begin(), a.end()}, {b.begin(), b.end()}) == -1;
        });

        auto div_pack_2_iter = find(all_tokens.begin(), all_tokens.end(), divider_packet(2));
        auto div_pack_6_iter = find(all_tokens.begin(), all_tokens.end(), divider_packet(6));

        auto diff1 = div_pack_2_iter - all_tokens.begin() + 1;
        auto diff2 = div_pack_6_iter - all_tokens.begin() + 1;

        cout << (diff1 * diff2) << endl;
    }

}