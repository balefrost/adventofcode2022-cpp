#pragma once

#include <sstream>
#include <string>

template<typename Iter>
std::string string_join(Iter begin, Iter end, const std::string &joiner) {
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
