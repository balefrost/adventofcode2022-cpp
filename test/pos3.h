#pragma once

#include "util.h"

inline namespace {
    using namespace std;

    struct pos3 {
        int x;
        int y;
        int z;
    };

    struct pos3_bounds {
        pos3 min;
        pos3 max;

        [[nodiscard]] bool contains(const pos3 &p) const {
            return p.x >= min.x &&
                   p.y >= min.y &&
                   p.z >= min.z &&
                   p.x <= max.x &&
                   p.y <= max.y &&
                   p.z <= max.z;
        }
    };

    template<typename Iter>
    pos3_bounds pos3_bounds(Iter begin, Iter end) {
        auto imin = numeric_limits<int>::min();
        auto imax = numeric_limits<int>::max();
        pos3 pmin{imax, imax, imax};
        pos3 pmax{imin, imin, imin};
        for (auto iter = begin; iter != end; ++iter) {
            pmin.x = min(pmin.x, iter->x);
            pmin.y = min(pmin.y, iter->y);
            pmin.z = min(pmin.z, iter->z);
            pmax.x = max(pmax.x, iter->x);
            pmax.y = max(pmax.y, iter->y);
            pmax.z = max(pmax.z, iter->z);
        }
        return {pmin, pmax};
    }

    vector<pos3> pos3_adjacent(const pos3 &p) {
        vector<pos3> result(6);
        result.push_back({p.x - 1, p.y, p.z});
        result.push_back({p.x + 1, p.y, p.z});
        result.push_back({p.x, p.y - 1, p.z});
        result.push_back({p.x, p.y + 1, p.z});
        result.push_back({p.x, p.y, p.z - 1});
        result.push_back({p.x, p.y, p.z + 1});
        return result;
    }

    auto pos3_comparer = compare_items<pos3>()
            .then_by(&pos3::x)
            .then_by(&pos3::y)
            .then_by(&pos3::z);

    auto pos3_less_comparer = pos3_comparer.as_less();

    template <>
    struct compare_three_way<pos3> {
        strong_ordering operator()(const pos3 &a, const pos3 &b) {
            return pos3_comparer.compare(a, b);
        }
    };

    typedef set<pos3, decltype(pos3_less_comparer)> pos3_ordered_set;

    pos3_ordered_set make_pos3_ordered_set() {
        return pos3_ordered_set(pos3_less_comparer);
    }
}
