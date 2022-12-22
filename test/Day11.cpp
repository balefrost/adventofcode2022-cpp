#include <gtest/gtest.h>
#include <fstream>
#include <numeric>
#include <regex>

using namespace std;

namespace day11 {

    void getlines(istream &input, int count, vector<string> &lines) {
        for (int i = 0; i < count; ++i) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }
            lines.push_back(std::move(line));
        }
    }


    template<typename Iter>
    class IteratorIterable {
    public:
        IteratorIterable(Iter begin, Iter end) : begin_(begin), end_(end) {}

        Iter begin() { return begin_; }

        Iter end() { return end_; }

    private:
        Iter begin_;
        Iter end_;
    };

    template<class... Types>
    IteratorIterable<sregex_iterator> regex_iterable(Types... args) {
        sregex_iterator begin(args...);
        sregex_iterator end;

        return {begin, end};
    }

    const regex monkey_regex("Monkey (\\d+):");
    const regex digit_regex("\\d+");
    const regex operation_regex(R"(  Operation: new = old (.) (\w+|\d+))");
    const regex test_regex("  Test: divisible by (\\d+)");
    const regex throw_regex("    If (?:true|false): throw to monkey (\\d+)");

    vector<string> regex_split(string input, const regex &splitter) {
        vector<string> result;

        sregex_iterator current(input.begin(), input.end(), splitter);
        sregex_iterator end;

        size_t start_of_next_string = 0;
        for (; current != end; ++current) {
            if (current->position() > start_of_next_string) {
                result.push_back(input.substr(start_of_next_string, current->position() - start_of_next_string));
            }
            start_of_next_string = current->position() + current->size();
        }

        if (start_of_next_string < input.size()) {
            result.push_back(input.substr(start_of_next_string));
        }

        return result;
    }

    TEST(Day11, regex_split) {
        ASSERT_EQ(regex_split("1", regex(":")), (vector<string>{"1"}));
        ASSERT_EQ(regex_split("1:2:3", regex(":")), (vector<string>{"1", "2", "3"}));
        ASSERT_EQ(regex_split(":2:", regex(":")), (vector<string>{"2"}));
    }

    struct monkey {
        int monkey_no;
        vector<long long> items;
        string op_op;
        string op_rhs;
        int divisible_by;
        int true_throw;
        int false_throw;
    };

    vector<monkey> parse_monkeys(istream &input) {
        vector<monkey> monkeys;

        while (true) {
            vector<string> lines;
            getlines(input, 6, lines);
            if (!input) {
                break;
            }
            string line;
            getline(input, line);

            smatch regex_result;
            if (!regex_match(lines[0], regex_result, monkey_regex)) {
                throw runtime_error("no match");
            }
            int monkey_num = stoi(regex_result[1]);
            vector<long long> starting_items;
            for (const auto &item: regex_iterable(lines[1].begin(), lines[1].end(), digit_regex)) {
                starting_items.push_back(stoi(item.str()));
            }
            if (!regex_match(lines[2], regex_result, operation_regex)) {
                throw runtime_error("no match");
            }
            string op = regex_result[1];
            string rhs = regex_result[2];
            if (!regex_match(lines[3], regex_result, test_regex)) {
                throw runtime_error("no match");
            }
            int divisible_by = stoi(regex_result[1]);
            if (!regex_match(lines[4], regex_result, throw_regex)) {
                throw runtime_error("no match");
            }
            int true_target = stoi(regex_result[1]);
            if (!regex_match(lines[5], regex_result, throw_regex)) {
                throw runtime_error("no match");
            }
            int false_target = stoi(regex_result[1]);

            monkeys.push_back(monkey{monkey_num, starting_items, op, rhs, divisible_by, true_target, false_target});
        }

        return monkeys;
    }

    void run_rounds(
            vector<monkey> monkeys,
            int rounds,
            const function<long long(long long, const monkey &)> &update_worry,
            vector<long long> &inspections
    ) {
        for (auto round = 0; round < rounds; ++round) {
            for (auto i = 0; i < monkeys.size(); ++i) {
                auto &monkey = monkeys[i];
                vector<long long> itemsClone = monkey.items;
                monkey.items.clear();
                for (const auto &worry: itemsClone) {
                    inspections[i]++;
                    auto new_worry = update_worry(worry, monkey);
                    int target_monkey;
                    if (new_worry % monkey.divisible_by == 0) {
                        target_monkey = monkey.true_throw;
                    } else {
                        target_monkey = monkey.false_throw;
                    }
                    monkeys.at(target_monkey).items.push_back(new_worry);
                }
            }
        }
    }

    const string sample_input = "Monkey 0:\n"
                                "  Starting items: 79, 98\n"
                                "  Operation: new = old * 19\n"
                                "  Test: divisible by 23\n"
                                "    If true: throw to monkey 2\n"
                                "    If false: throw to monkey 3\n"
                                "\n"
                                "Monkey 1:\n"
                                "  Starting items: 54, 65, 75, 74\n"
                                "  Operation: new = old + 6\n"
                                "  Test: divisible by 19\n"
                                "    If true: throw to monkey 2\n"
                                "    If false: throw to monkey 0\n"
                                "\n"
                                "Monkey 2:\n"
                                "  Starting items: 79, 60, 97\n"
                                "  Operation: new = old * old\n"
                                "  Test: divisible by 13\n"
                                "    If true: throw to monkey 1\n"
                                "    If false: throw to monkey 3\n"
                                "\n"
                                "Monkey 3:\n"
                                "  Starting items: 74\n"
                                "  Operation: new = old + 3\n"
                                "  Test: divisible by 17\n"
                                "    If true: throw to monkey 0\n"
                                "    If false: throw to monkey 1";

    TEST(Day11, Part1) {
        ifstream input;
        input.open("../../test/input/day11.txt");
//    stringstream input(sample_input);

        vector<monkey> monkeys = parse_monkeys(input);
        vector<long long> inspections(monkeys.size(), 0);

        run_rounds(
                monkeys,
                20,
                [](long long worry, const monkey &monkey) {
                    long long rhsValue = monkey.op_rhs == "old" ? worry : stoi(monkey.op_rhs);
                    if (monkey.op_op == "*") {
                        worry *= rhsValue;
                    } else if (monkey.op_op == "+") {
                        worry += rhsValue;
                    } else {
                        throw logic_error("invalid op");
                    }
                    worry /= 3;
                    return worry;
                },
                inspections
        );

        std::sort(inspections.begin(), inspections.end());

        cout << (*(inspections.end() - 1) * *(inspections.end() - 2)) << endl;
    }

    TEST(Day11, mult_mod) {
        for (int d = 1; d < 100; ++d) {
            for (int a = 0; a < 1000; ++a) {
                for (int b = 0; b < 1000; ++b) {
                    ASSERT_EQ((a * b) % d, ((a % d) * (b % d)) % d);
                }
            }
        }
    }

    TEST(Day11, Part2) {
        ifstream input;
        input.open("../../test/input/day11.txt");
//    stringstream input(sample_input);

        vector<monkey> initial_monkeys = parse_monkeys(input);

        auto monkeys = initial_monkeys;

        auto theLcm = transform_reduce(
                monkeys.begin(),
                monkeys.end(),
                1L,
                [](long a, long b) { return lcm(a, b); },
                [](auto a) { return (long) a.divisible_by; }
        );

        vector<long long> inspections(monkeys.size(), 0);
        run_rounds(
                monkeys,
                10000,
                [theLcm](long long worry, auto &monkey) {
                    worry %= theLcm;
                    long long rhsValue = monkey.op_rhs == "old" ? worry : stoi(monkey.op_rhs);
                    rhsValue %= theLcm;
                    if (monkey.op_op == "*") {
                        worry *= rhsValue;
                    } else if (monkey.op_op == "+") {
                        worry += rhsValue;
                    } else {
                        throw logic_error("invalid op");
                    }
                    if (worry < 0) {
                        throw logic_error("invalid worry");
                    }
                    return worry;
                },
                inspections
        );

        std::sort(inspections.begin(), inspections.end());

        cout << (*(inspections.end() - 1) * *(inspections.end() - 2)) << endl;
    }

}