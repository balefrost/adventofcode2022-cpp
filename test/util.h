#pragma once

#include <compare>
#include <functional>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <vector>

inline namespace {
    using std::common_type;
    using std::function;
    using std::iterator_traits;
    using std::logic_error;
    using std::map;
    using std::pair;
    using std::set;
    using std::stack;
    using std::string;
    using std::stringstream;
    using std::vector;

    enum strong_ordering {
        less,
        equal,
        greater
    };

    template <typename T>
    struct compare_three_way {
        strong_ordering operator()(const T& a, const T& b) {
            return a < b ? strong_ordering::less :
                   a > b ? strong_ordering::greater :
                   strong_ordering::equal;
        }
    };

//    template <>
//    struct compare_three_way<int> {
//        strong_ordering operator()(int a, int b) {
//            return default_compare_three_way(a, b);
//        }
//    };
//
//    template <>
//    struct compare_three_way<unsigned long> {
//        strong_ordering operator()(unsigned long a, unsigned long b) {
//            return default_compare_three_way(a, b);
//        }
//    };
//
//    template <typename T>
//    struct compare_three_way<vector<T>> {
//        strong_ordering operator()(const vector<T> &a, const vector<T> &b) {
//            return default_compare_three_way(a, b);
//        }
//    };

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
                return *this != rhs;
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
                    }
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
                if (!item) {
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
        compare_items &then_by(function<R(const T)> f) {
            comparisons.push_back([f](const T &a, const T &b) {
                return compare_three_way<R>{}(f(a), f(b));
            });
            return *this;
        }

        template<typename R>
        compare_items &then_by(R T::* m) {
            comparisons.push_back([m](const T &a, const T &b) {
                return compare_three_way<R>{}(a.*m, b.*m);
            });
            return *this;
        }

        template<typename R>
        compare_items &then_by_reversed(R T::* f) {
            comparisons.push_back([f](const T &a, const T &b) {
                return compare_three_way<R>{}(b.*f, a.*f);
            });
            return *this;
        }

        compare_items &then_by_intrinsic() {
            comparisons.push_back([](const T &a, const T &b) {
                return compare_three_way<T>{}(a, b);
            });
            return *this;
        }

        compare_items &then_by_comparer(const compare_items<T> &comparer) {
            comparisons.push_back([comparer](const T &a, const T &b) {
                return comparer.compare(a, b);
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

    template <typename Coll>
    class push_iterator {
    private:
        Coll *coll;
    public:
        explicit push_iterator(Coll &coll) : coll(addressof(coll)) {}

        push_iterator& operator++() {
            return *this;
        }
        push_iterator operator++(int) {
            return *this;
        }
        push_iterator& operator*() {
            return *this;
        }
        push_iterator& operator=(const typename Coll::value_type &v) {
            coll->push(v);
            return *this;
        }
        push_iterator& operator=(const typename Coll::value_type &&v) {
            coll->push(std::move(v));
            return *this;
        }
    };

    template<typename T>
    T sign(T value) {
        return value < 0 ? -1 :
               value > 0 ? 1 :
               0;
    }

    //see https://vladris.com/blog/2018/10/13/arithmetic-overflow-and-underflow.html
    template<typename T1, typename T2>
    constexpr bool AdditionOverflows(const T1 &a, const T2 &b) {
        typedef typename common_type<T1, T2>::type ct;
        return (b >= 0) && (a > (std::numeric_limits<ct>::max() - b));
    }

    template<typename T1, typename T2>
    constexpr bool AdditionUnderflows(const T1 &a, const T2 &b) {
        typedef typename common_type<T1, T2>::type ct;
        return (b < 0) && (a < std::numeric_limits<ct>::min() - b);
    }

    template<typename T1, typename T2>
    constexpr bool MultiplicationOverflows(const T1 &a, const T2 &b) {
        typedef typename common_type<T1, T2>::type ct;
        if (b == 0) return false; // Avoid division by 0
        return ((b > 0) && (a > 0) && (a > std::numeric_limits<ct>::max() / b))
               || ((b < 0) && (a < 0) && (a < std::numeric_limits<ct>::max() / b));
    }

    template<typename T1, typename T2>
    constexpr bool MultiplicationUnderflows(const T1 &a, const T2 &b) {
        typedef typename common_type<T1, T2>::type ct;
        if (b == 0) return false; // Avoid division by 0
        return ((b > 0) && (a < 0) && (a < std::numeric_limits<ct>::min() / b))
               || ((b < 0) && (a > 0) && (a > std::numeric_limits<ct>::min() / b));
    }

    template<typename T1, typename T2>
    typename common_type<T1, T2>::type safe_add(T1 a, T2 b) {
        if (AdditionOverflows(a, b)) {
            throw logic_error("addition overflow");
        }
        if (AdditionUnderflows(a, b)) {
            throw logic_error("addition underflow");
        }

        return a + b;
    }

    template<typename T1, typename T2>
    typename common_type<T1, T2>::type safe_multiply(T1 a, T2 b) {
        if (a == 0 || b == 0) {
            return 0;
        }

        if (MultiplicationOverflows(a, b)) {
            throw logic_error("multiplication overflow");
        }
        if (MultiplicationUnderflows(a, b)) {
            throw logic_error("multiplication underflow");
        }

        return a * b;
    }

    //TODO: figure out how to better deduce template types
    template<typename V, typename C>
    void postorder_traversal(
            const V &root,
            const function<const C(const V &)> &get_children,
            const function<void(const V &)> &visit
    ) {
        stack<V> queue;
        queue.emplace(root);
        set<V> seen;

        while (!queue.empty()) {
            string item = queue.top();
            if (seen.insert(item).second) {
                for (const auto &c: get_children(item)) {
                    queue.push(c);
                }
            } else {
                queue.pop();
                visit(item);
            }
        }
    }

    template <typename Key, typename Compare, typename Allocator>
    bool set_contains(const set<Key, Compare, Allocator> &s, const Key &elem) {
        return s.find(elem) != s.end();
    }

    template<typename T>
    T posmod(T numerator, T divisor) {
        T temp = numerator % divisor;
        if (temp < 0) {
            return temp + divisor;
        } else {
            return temp;
        }
    }

    // stolen from https://stackoverflow.com/a/2595226, allegedly from Boost
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}
