#pragma once

#include <functional>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

inline namespace {
    using namespace std;

    template<typename Iter>
    string string_join(Iter begin, Iter end, const string &joiner = ", ") {
        stringstream result;
        bool first = true;
        for (auto current = begin; current != end; ++current) {
            if (!first) {
                result << joiner;
            }
            result << *current;
            first = false;
        }
        return result.str();
    }

    template<typename T>
    class postorder {
    public:
        postorder(
                T root,
                const function<vector<T>(T elem)> &find_adjacent
        ) : root_(std::move(root)), find_adjacent_(find_adjacent) {

        }

        struct postorder_stack_elem {
            vector<T> elems;
            size_t current_index{0};

            [[nodiscard]] bool at_end() const { return current_index == elems.size(); }

            T &current_elem() {
                return elems.at(current_index);
            }

            bool operator==(const postorder_stack_elem &b) {
                return elems == b.elems && current_index == b.current_index;
            }
        };

        struct postorder_iter {
        private:
            stack<postorder_stack_elem> stack_{};
            function<vector<T>(const T &elem)> find_adjacent_;

            void fill_stack() {
                if (!stack_.empty()) {
                    while (true) {
                        auto adj = find_adjacent_(stack_.top().current_elem());
                        if (adj.empty()) {
                            break;
                        }
                        stack_.push({adj, 0});
                    }
                }
            }

        public:
            postorder_iter(stack<postorder_stack_elem> stack,
                           function<vector<T>(const T &elem)> find_adjacent) : stack_(stack), find_adjacent_(
                    std::move(find_adjacent)) {
                fill_stack();
            }

            bool operator==(const postorder_iter &rhs) const {
                return stack_ == rhs.stack_;
            }

            bool operator!=(const postorder_iter &rhs) const {
                return !(*this == rhs);
            }

            T *operator->() {
                if (stack_.empty()) {
                    throw logic_error("invalid dereference");
                }
                postorder_stack_elem &st = stack_.top();
                return &(st.elems.at(st.current_index));
            };

            T &operator*() {
                if (stack_.empty()) {
                    throw logic_error("invalid dereference");
                }
                return stack_.top().elems.at(stack_.top().current_index);
            }

            postorder_iter &operator++() {
                if (!stack_.empty()) {
                    stack_.top().current_index++;
                    if (stack_.top().at_end()) {
                        stack_.pop();
                    } else {
                        fill_stack();
                    };
                }
                return *this;
            }
        };

        [[nodiscard]] postorder_iter begin() const {
            stack<postorder_stack_elem> s;
            s.push({{root_}, 0});
            return postorder_iter{std::move(s), find_adjacent_};
        }

        [[nodiscard]] postorder_iter end() const {
            return postorder_iter{stack<postorder_stack_elem>(), find_adjacent_};
        }

    private:
        T root_;
        function<vector<T>(T elem)> find_adjacent_;
    };

    template<typename Iter>
    vector<pair<set<typename iterator_traits<Iter>::value_type>, set<typename iterator_traits<Iter>::value_type>>>
    find_partitions(const Iter &begin, const Iter &end) {
        using namespace std;

        typedef typename iterator_traits<Iter>::value_type value_type;

        vector<bool> included(distance(begin, end));

        vector<pair<set<value_type>, set<value_type>>> result;

        while (true) {
            set<value_type> p1;
            set<value_type> p2;
            Iter item_curr = begin;
            auto incl_curr = included.begin();
            while (item_curr != end) {
                if (*incl_curr) {
                    p1.insert(*item_curr);
                } else {
                    p2.insert(*item_curr);
                }
                ++item_curr;
                ++incl_curr;
            }
            result.emplace_back(std::move(p1), std::move(p2));

            auto carry = false;
            for (auto &&item: included) {
                if (item == false) {
                    item = true;
                    carry = false;
                    break;
                } else {
                    item = false;
                    carry = true;
                }
            }
            if (carry) {
                break;
            }
        }

        return result;
    }

    template<typename S>
    pair<vector<S>, vector<S>> iterate_until_cycle(
            S initial_state,
            const function<S(const S &)> &next_state,
            const function<bool(const S &, const S &)> &states_less) {
        S current = std::move(initial_state);
        vector<S> in_order;
        map<S, size_t, function<bool(const S &, const S &)>> seen(states_less);

        for (size_t idx = 0;; ++idx) {
            auto has_been_seen = seen.find(current);
            if (has_been_seen == seen.end()) {
                in_order.push_back(current);
                seen[current] = idx;
                current = next_state(current);
            } else {
                auto foo = has_been_seen->second;
                auto vec_iter = in_order.begin();
                advance(vec_iter, foo);
                vector<S> prefix{in_order.begin(), vec_iter};
                vector<S> cycle{vec_iter, in_order.end()};
                return {std::move(prefix), std::move(cycle)};
            }
        }
    }

    template<typename T>
    class compare_items {
    private:
        vector<function<strong_ordering(const T &, const T &)>> comparisons;
    public:
        class less_comparer_func {
        private:
            vector<function<strong_ordering(const T &, const T &)>> comparisons;

        public:
            explicit less_comparer_func(vector<function<strong_ordering(const T &, const T &)>> comparisons)
                    : comparisons(comparisons) {}

            bool operator()(const T &a, const T &b) const {
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
        compare_items &then_by(function<R(T)> f) {
            comparisons.push_back([f](const T &a, const T &b) {
                return f(a) <=> f(b);
            });
            return *this;
        }

        template<typename R>
        compare_items &then_by(R T::* f) {
            comparisons.push_back([f](const T &a, const T &b) {
                return a.*f <=> b.*f;
            });
            return *this;
        }

        strong_ordering compare(const T& a, const T& b) const {
            for (const auto &item: comparisons) {
                auto comparison_result = item(a, b);
                if (comparison_result != strong_ordering::equal) {
                    return comparison_result;
                }
            }

            return strong_ordering::equal;
        }

        less_comparer_func as_less() {
            return less_comparer_func{comparisons};
        }
    };
}
