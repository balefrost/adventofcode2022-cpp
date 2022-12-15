#pragma once

struct position {
    int x;
    int y;

    [[nodiscard]] bool operator<(const position &b) const {
        return x < b.x || (x == b.x && y < b.y);
    }

    [[nodiscard]] bool operator==(const position &b) const {
        return x == b.x && y == b.y;
    }

    [[nodiscard]] position operator+(const position &b) const {
        return {x + b.x, y + b.y};
    }

    [[nodiscard]] int manhattan_distance_to(const position &b) const {
        return abs(x - b.x) + abs(y - b.y);
    }
};
