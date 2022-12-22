#include <gtest/gtest.h>
#include <fstream>

using namespace std;

namespace day8 {

    struct visibility {
        int height;
        bool isVisible;
    };

    void updateVisibility(
            vector<vector<visibility>> &rows,
            int r, int c,
            int dr, int dc) {
        int largestHeight = -1;
        while (r >= 0 && c >= 0 && r < rows.size() && c < rows[0].size()) {
            if (rows[r][c].height > largestHeight) {
                largestHeight = rows[r][c].height;
                rows[r][c].isVisible = true;
            }
            r += dr;
            c += dc;
        }
    }

    int countVisibleTrees(
            const vector<vector<visibility>> &rows,
            int r, int c,
            int dr, int dc) {
        int targetHeight = rows[r][c].height;
        r += dr;
        c += dc;
        int visibleTrees = 0;
        while (r >= 0 && c >= 0 && r < rows.size() && c < rows[0].size()) {
            visibleTrees += 1;
            if (rows[r][c].height >= targetHeight) {
                break;
            }
            r += dr;
            c += dc;
        }

        return visibleTrees;
    }

    TEST(Day8, Part1) {
        ifstream input;
        input.open("../../test/input/day8.txt");

        vector<vector<visibility>> rows;

        while (true) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }

            vector<visibility> row;
            for (const auto &item: line) {
                row.push_back({item - '0', false});
            }
            rows.push_back(std::move(row));
        }

        for (int r = 0; r < rows.size(); ++r) {
            updateVisibility(rows, r, 0, 0, 1);
            updateVisibility(rows, r, rows[0].size() - 1, 0, -1);
        }
        for (int c = 0; c < rows[0].size(); ++c) {
            updateVisibility(rows, 0, c, 1, 0);
            updateVisibility(rows, rows.size() - 1, c, -1, 0);
        }

        int numVisible = 0;

        for (const auto &item: rows) {
            for (const auto &item: item) {
                if (item.isVisible) {
                    numVisible += 1;
                }
            }
        }

        cout << numVisible << endl;
    }

    TEST(Day8, Part2) {
        ifstream input;
        input.open("../../test/input/day8.txt");

        vector<vector<visibility>> rows;

        while (true) {
            string line;
            getline(input, line);
            if (!input) {
                break;
            }

            vector<visibility> row;
            for (const auto &item: line) {
                row.push_back({item - '0', false});
            }
            rows.push_back(std::move(row));
        }

        int bestScore = 0;
        for (int r = 0; r < rows.size(); ++r) {
            for (int c = 0; c < rows[0].size(); ++c) {
                auto trees0 = countVisibleTrees(rows, r, c, 0, 1);
                auto trees1 = countVisibleTrees(rows, r, c, 0, -1);
                auto trees2 = countVisibleTrees(rows, r, c, 1, 0);
                auto trees3 = countVisibleTrees(rows, r, c, -1, 0);
                auto score = trees0 * trees1 * trees2 * trees3;
                if (score > bestScore) {
                    bestScore = score;
                }
            }
        }

        cout << bestScore << endl;
    }

}