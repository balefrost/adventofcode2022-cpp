#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>

class span {
private:
    int _first;
    int _last;
public:
    span(int first, int last) : _first(first), _last(last) {}

    [[nodiscard]] int first() const { return _first; }

    [[nodiscard]] int last() const { return _last; }

    [[nodiscard]] int length() const { return _last - _first + 1; }

    void setFirst(int first) {
        _first = first;
    }

    void setLast(int last) {
        _last = last;
    }

    bool operator==(const span &b) const {
        return _first == b._first && _last == b._last;
    }
};

class span_list {
private:
    std::vector <span> occupied;
public:

    [[nodiscard]] int size() const {
        int result = 0;
        for (const auto &item: occupied) {
            result += item.length();
        }
        return result;
    }

    typedef std::vector<span>::iterator iterator;
    typedef std::vector<span>::const_iterator const_iterator;

    [[nodiscard]] const_iterator begin() const {
        return occupied.begin();
    }

    [[nodiscard]] const_iterator end() const {
        return occupied.end();
    }

    void insert_range(int a, int b) {

        if (b < a) {
            std::swap(a, b);
        }

        auto touching_begin = std::lower_bound(
                occupied.begin(), occupied.end(),
                span{a, b},
                [](auto a, auto b) { return a.last() < b.first() - 1; });
        auto touching_end = std::upper_bound(
                touching_begin, occupied.end(),
                span{a, b},
                [](auto a, auto b) { return a.last() < b.first() - 1; });

//        if (touching_begin == touching_end) {
//            occupied.insert(touching_begin, {a, b});
//        } else {
//            vector<span> result;
//            copy(occupied.begin(), touching_begin, back_inserter(result));
//            occupied.emplace_back(min(touching_begin->first(), a), max((touching_end - 1)->last(), b));
//            copy(touching_end, occupied.end(), back_inserter(result));
//            occupied = std::move(result);
//        }


        if (touching_begin == touching_end) {
            occupied.insert(touching_begin, {a, b});
        } else {
            touching_begin->setFirst(std::min(touching_begin->first(), a));
            touching_begin->setLast(std::max((touching_end - 1)->last(), b));
            ++touching_begin;
            occupied.erase(touching_begin, touching_end);
        }
    }

    void remove_range(int a, int b) {

        if (b < a) {
            std::swap(a, b);
        }

        auto ol_begin = std::lower_bound(
                occupied.begin(), occupied.end(),
                span{a, b},
                [](auto a, auto b) { return a.last() < b.first(); });
        auto ol_end = upper_bound(
                ol_begin, occupied.end(),
                span{a, b},
                [](auto a, auto b) { return a.last() < b.first(); });

//        vector<span> result;
//
//        copy(occupied.begin(), ol_begin, back_inserter(result));
//        if (ol_end - ol_begin == 1) {
//            auto& item = ol_begin;
//            if (item->first() < a && item->last() > b) {
//                result.emplace_back(item->first(), a - 1);
//                result.emplace_back(b + 1, item->last());
//            } else if (item->first() == a && item->last() > b) {
//                result.emplace_back(b + 1, item->last());
//            } else if (item->first() < a && item->last() == b) {
//                result.emplace_back(item->first(), a - 1);
//            }
//        } else if (ol_end != ol_begin) {
//            for(auto iter = ol_begin; iter != ol_end; ++iter) {
//                auto x = max(iter->first(), b + 1);
//                auto y = min(iter->last(), a - 1);
//                if (x <= y) {
//                    result.emplace_back(x, y);
//                }
//            }
//        }
//        copy(ol_end, occupied.end(), back_inserter(result));
//
//        occupied = std::move(result);

        if (ol_begin == ol_end) {
            return;
        } else if (ol_end - ol_begin == 1) {
            auto item = ol_begin;

            //we might need to split this item
            if (a <= item->first() && item->last() <= b) {
                // item: [----]
                //       a----b
                //
                // item:    [---]
                //       a---------b
                occupied.erase(item, ol_end);
            } else if (item->first() < a && b < item->last()) {
                // item: [---------]
                //          a---b
                auto span_value = *item;
                item = occupied.insert(item, {span_value.first(), a - 1});
                ++item;
                item->setFirst(b + 1);
            } else if (item->first() < a && item->last() <= b) {
                // item: [-------]
                //          a----b

                // item: [-------]
                //          a-------b
                item->setLast(a - 1);
            } else if (a <= item->first() && b < item->last()) {
                // item:   [-------]
                //         a--b

                // item:   [-------]
                //       a----b
                item->setFirst(b + 1);
            } else {
                throw std::runtime_error("no match");
            }
        } else {
            auto item1 = ol_begin;
            auto item2 = ol_end - 1;

            if (item1->first() < a) {
                // item1: [-------
                //            a---
                item1->setLast(a - 1);
                ++item1;
            }

            if (item2->last() > b) {
                // item2: -------]
                //        ----b
                item2->setFirst(b + 1);
                --item2;
            }

            occupied.erase(item1, item2 + 1);
        }
    }
};

