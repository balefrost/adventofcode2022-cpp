#pragma once

#include <ostream>

inline namespace {
    using namespace std;

    /**
     * Assumes Y increases top-to-bottom; X increases left-to-right (i.e. screen coords, not math coords)
     */
    struct pos2 {
        int x;
        int y;

        pos2 rotate_cw_about_origin() {
            return {-y, x};
        }

        pos2 rotate_ccw_about_origin() {
            return {y, -x};
        }

        pos2 operator+(const pos2 &b) const {
            return {x + b.x, y + b.y};
        }

        pos2 &operator+=(const pos2 &b) {
            x += b.x;
            y += b.y;
            return *this;
        }

        pos2 operator-() const {
            return {-x, -y};
        }

        pos2 operator-(const pos2 &b) const {
            return {x - b.x, y - b.y};
        }

        pos2 &operator-=(const pos2 &b) {
            x -= b.x;
            y -= b.y;
            return *this;
        }

        bool operator<(const pos2 &r) const {
            return x < r.x || (x == r.x && y < r.y);
        }

        bool operator==(const pos2 &r) const {
            return x == r.x && y == r.y;
        }
    };

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
