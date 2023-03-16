#include <gtest/gtest.h>
#include <exception>
#include <fstream>
#include <numeric>
#include <queue>
#include <regex>

namespace day25 {
    using namespace std;

    const string sample_input = "1=-0-2\n"
                                " 12111\n"
                                "  2=0=\n"
                                "    21\n"
                                "  2=01\n"
                                "   111\n"
                                " 20012\n"
                                "   112\n"
                                " 1=-1=\n"
                                "  1-12\n"
                                "    12\n"
                                "    1=\n"
                                "   122";

    int64_t snafu_to_dec(string snafu) {
        if (snafu.length() == 0 ) {
            return 0;
        }
        int64_t power = 1;
        int64_t result = 0;
        for (auto iter = snafu.rbegin(); iter != snafu.rend(); ++iter) {
            switch(*iter) {
                case '1':
                    result += power;
                    break;
                case '2':
                    result += (power + power);
                    break;
                case '-':
                    result -= power;
                    break;
                case '=':
                    result -= (power + power);
                    break;
                case '0':
                    break;
                default:
                    throw logic_error("bad char " + to_string(*iter));
            }
            power *= 5;
        }
        return result;
    }

    string dec_to_snafu(int64_t dec) {
        vector<int> base5_digits;
        auto temp = dec;
        while (temp > 0) {
            base5_digits.push_back((int)(temp % 5));
            temp /= 5;
        }
        base5_digits.push_back(0);
        for (auto i = 0; i < base5_digits.size(); ++i) {
            if (base5_digits[i] > 2) {
                base5_digits[i] -= 5;
                base5_digits[i + 1] += 1;
            }
        }
        while (base5_digits.back() == 0) {
            base5_digits.pop_back();
        }
        stringstream result;
        for (auto iter = base5_digits.rbegin(); iter != base5_digits.rend(); ++iter) {
            switch (*iter) {
                case -2:
                    result << '=';
                    break;
                case -1:
                    result << '-';
                    break;
                case 0:
                    result << '0';
                    break;
                case 1:
                    result << '1';
                    break;
                case 2:
                    result << '2';
                    break;
                default:
                    throw logic_error("bad digit: " + to_string(*iter));
            }
        }
        return result.str();
    }

    TEST(Day25, Part1) {
        ifstream input;
        input.open("../../test/input/day25.txt");
//        stringstream input(sample_input);

        string line;
        int64_t sum = 0;
        while (getline(input, line)) {
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            auto dec = snafu_to_dec(line);
            {
                auto snafu = dec_to_snafu(dec);
                if (snafu != line) {
                    string message;
                    message.append("snafu: ").append(line).append("  dec: ").append(to_string(dec)).append("  snafu: ").append(snafu);
                    throw logic_error(message);
                }
            }
            sum += dec;
            {
                auto snafu = dec_to_snafu(sum);
                auto resum = snafu_to_dec(snafu);
                if (sum != resum) {
                    string message;
                    message.append("dec: ").append(to_string(sum)).append("  snafu: ").append(snafu).append("  redec: ").append(to_string(resum));
                    throw logic_error(message);
                }
            }
        }
        cout << dec_to_snafu(sum) << endl;
    }
}
