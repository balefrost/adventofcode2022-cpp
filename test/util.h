#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <vector>

template<typename Iter>
std::string string_join(Iter begin, Iter end, const std::string &joiner = ", ") {
    std::stringstream result;
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
            const std::function<std::vector<T>(T elem)> &find_adjacent
    ) : root_(std::move(root)), find_adjacent_(find_adjacent) {

    }

    struct postorder_stack_elem {
        std::vector<T> elems;
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
        std::stack<postorder_stack_elem> stack_{};
        std::function<std::vector<T>(const T &elem)> find_adjacent_;

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
        postorder_iter(std::stack<postorder_stack_elem> stack,
                       std::function<std::vector<T>(const T &elem)> find_adjacent) : stack_(stack), find_adjacent_(
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
                throw std::logic_error("invalid dereference");
            }
            postorder_stack_elem &st = stack_.top();
            return &(st.elems.at(st.current_index));
        };

        T &operator*() {
            if (stack_.empty()) {
                throw std::logic_error("invalid dereference");
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
        std::stack<postorder_stack_elem> s;
        s.push({{root_}, 0});
        return postorder_iter{std::move(s), find_adjacent_};
    }

    [[nodiscard]] postorder_iter end() const {
        return postorder_iter{std::stack<postorder_stack_elem>(), find_adjacent_};
    }

private:
    T root_;
    std::function<std::vector<T>(T elem)> find_adjacent_;
};

template<typename Iter>
std::vector<std::pair<std::set<typename std::iterator_traits<Iter>::value_type>,std::set<typename std::iterator_traits<Iter>::value_type>>>
find_partitions(const Iter &begin, const Iter &end) {
    using namespace std;

    typedef typename iterator_traits<Iter>::value_type value_type;

    vector<bool> included(distance(begin, end));

    vector<pair<set<value_type>,set<value_type>>> result;

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
