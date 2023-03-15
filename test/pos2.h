#pragma once

#include <ostream>
#include "util.h"

inline namespace {
    using namespace std;

    template <typename T>
    struct pos2_t {
        T x;
        T y;

        pos2_t rotate_cw_about_origin() {
            return {-y, x};
        }

        pos2_t rotate_ccw_about_origin() {
            return {y, -x};
        }

        pos2_t operator+(const pos2_t &b) const {
            return {x + b.x, y + b.y};
        }

        pos2_t &operator+=(const pos2_t &b) {
            x += b.x;
            y += b.y;
            return *this;
        }

        pos2_t operator-() const {
            return {-x, -y};
        }

        pos2_t operator-(const pos2_t &b) const {
            return {x - b.x, y - b.y};
        }

        pos2_t &operator-=(const pos2_t &b) {
            x -= b.x;
            y -= b.y;
            return *this;
        }

        bool operator<(const pos2_t &r) const {
            return x < r.x || (x == r.x && y < r.y);
        }

        bool operator==(const pos2_t &r) const {
            return x == r.x && y == r.y;
        }

        bool operator!=(const pos2_t &r) const {
            return !(*this == r);
        }

        T manhattan_distance_to(pos2_t other) const {
            return abs(other.x - x) + abs(other.y - y);
        }
    };

    /**
     * Assumes Y increases top-to-bottom; X increases left-to-right (i.e. screen coords, not math coords)
     */
    using pos2 = pos2_t<int>;
    using pos2_ll = pos2_t<long long>;

    pair<pos2, pos2> bounding_box(const set<pos2> &ps) {
        int xmin = numeric_limits<int>::max();
        int ymin = numeric_limits<int>::max();
        int xmax = numeric_limits<int>::min();
        int ymax = numeric_limits<int>::min();
        for (const auto &p: ps) {
            xmin = min(xmin, p.x);
            ymin = min(ymin, p.y);
            xmax = max(xmax, p.x);
            ymax = max(ymax, p.y);
        }
        return {{xmin, ymin}, {xmax, ymax}};
    }

    ostream &operator<<(ostream &out, const pos2 &p) {
        out << "{" << p.x << "," << p.y << "}";
        return out;
    }
}

template <typename T>
struct std::hash<pos2_t<T>> {
    size_t operator()(const pos2_t<T> &pos) const {
        size_t seed = 0;
        hash_combine(seed, pos.x);
        hash_combine(seed, pos.y);
        return seed;
    }
};