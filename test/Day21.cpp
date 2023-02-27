#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include "util.h"

using namespace std;

namespace day21 {

    const string sample_input = "root: pppw + sjmn\n"
                                "dbpl: 5\n"
                                "cczh: sllz + lgvd\n"
                                "zczc: 2\n"
                                "ptdq: humn - dvpt\n"
                                "dvpt: 3\n"
                                "lfqf: 4\n"
                                "humn: 5\n"
                                "ljgn: 2\n"
                                "sjmn: drzm * dbpl\n"
                                "sllz: 4\n"
                                "pppw: cczh / lfqf\n"
                                "lgvd: ljgn * ptdq\n"
                                "drzm: hmdt - zczc\n"
                                "hmdt: 32";

    const regex operation_regex("(\\w+): (\\w+) ([+/*-]) (\\w+)");
    const regex number_regex("(\\w+): (\\d+)");

    struct entry {
        vector<string> dependencies;
        function<long long(const map<string, long long> &)> calculate;
        string op_sym;
        string rule;
    };

    map<string, entry> parse_input(istream &input) {
        string line;
        map<string, entry> result;
        while (getline(input, line)) {
            smatch m;
            if (regex_match(line, m, operation_regex)) {
                auto name = m[1];
                auto arg0 = m[2].str();
                auto op = m[3];
                auto arg1 = m[4].str();
                function<long long(long long, long long)> op_fn;
                if (op == "+") {
                    op_fn = [](long long a, long long b) {
                        return a + b;
                    };
                } else if (op == "-") {
                    op_fn = [](long long a, long long b) {
                        return a - b;
                    };
                } else if (op == "*") {
                    op_fn = [](long long a, long long b) {
                        return a * b;
                    };
                } else if (op == "/") {
                    op_fn = [](long long a, long long b) {
                        return a / b;
                    };
                }
                auto f = [arg0, arg1, op_fn](const map<string, long long> &m) {
                    return op_fn(m.at(arg0), m.at(arg1));
                };
                result[name] = entry{
                        {arg0, arg1},
                        f,
                        op.str(),
                        line
                };
            } else if (regex_match(line, m, number_regex)) {
                auto name = m[1];
                auto value = stoi(m[2]);
                result[name] = entry{
                        {},
                        [value](const map<string, long long> &m) {
                            return value;
                        },
                        m[2].str(),
                        line
                };
            } else {
                throw logic_error("no match");
            }
        }

        return result;
    }

    long long execute(const map<string, entry> &entries) {
        map<string, long long> results;

        postorder_traversal<string, vector<string>>(
                "root",
                [&](const string &node) {
                    const entry &e = entries.at(node);
                    return e.dependencies;
                },
                [&](const string &node) {
                    const entry &e = entries.at(node);
                    results[node] = e.calculate(results);
                }
        );

        return results.at("root");
    }

    TEST(Day21, Part1) {
        ifstream input;
        input.open("../../test/input/day21.txt");
//        stringstream input(sample_input);

        auto entries = parse_input(input);

        auto result = execute(entries);

        cout << result << endl;
    }

    void print_exp_tree(const map<string, entry> &entries, string root, int depth = 0) {
        const auto &entry = entries.at(root);
        cout << string(depth * 2, ' ') << entry.rule << endl;
        for (const auto &item: entry.dependencies) {
            print_exp_tree(entries, item, depth + 1);
        }
    }

    TEST(Day21, Part2) {
        ifstream input;
        input.open("../../test/input/day21.txt");
//        stringstream input(sample_input);

        auto entries = parse_input(input);


        auto a = entries["root"].dependencies[0];
        auto b = entries["root"].dependencies[1];

        map<string, long long> results;
        set<string> ground;

        postorder_traversal<string, vector<string>>(
                "root",
                [&](const string &node) {
                    const entry &e = entries.at(node);
                    return e.dependencies;
                },
                [&](const string &node) {
                    if (node != "humn") {
                        const entry &e = entries.at(node);
                        auto all_deps_ground = std::all_of(e.dependencies.begin(), e.dependencies.end(),
                                                           [&](auto d) { return set_contains(ground, d); });
                        if (all_deps_ground) {
                            ground.insert(node);
                            results[node] = e.calculate(results);
                        }
                    }
                }
        );

        //keep ground term in b
        if (set_contains(ground, a) && !set_contains(ground, b)) {
            swap(a, b);
        }
        auto bval = results.at(b);
        while (a != "humn") {
            const auto &a_entry = entries.at(a);
            if (a_entry.dependencies.size() < 2) {
                throw logic_error("invalid left side");
            }
            auto ll = a_entry.dependencies.at(0);
            auto rr = a_entry.dependencies.at(1);
            if (a_entry.op_sym == "+") {
                if (set_contains(ground, rr)) {
                    bval -= results.at(rr);
                } else {
                    bval -= results.at(ll);
                }
            } else if (a_entry.op_sym == "-") {
                if (set_contains(ground, rr)) {
                    bval += results.at(rr);
                } else {
                    bval = results.at(ll) - bval;
                }
            } else if (a_entry.op_sym == "*") {
                if (set_contains(ground, rr)) {
                    bval /= results.at(rr);
                } else {
                    bval /= results.at(ll);
                }
            } else if (a_entry.op_sym == "/") {
                if (set_contains(ground, rr)) {
                    bval *= results.at(rr);
                } else {
                    bval = results.at(ll) / bval;
                }
            } else {
                throw logic_error("unknown op");
            }

            if (set_contains(ground, rr)) {
                a = ll;
            } else {
                a = rr;
            }
        }

        cout << bval << endl;
    }

}