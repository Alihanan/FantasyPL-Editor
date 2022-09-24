#pragma once

#include <algorithm>
#include <set>
#include <string>
#include <vector>
#include <regex>

namespace PL
{
    template <typename T>
    std::vector<T> differenceSubsets(std::vector<T> A, std::vector<T> B)
    {
        std::sort(A.begin(), A.end());     
        std::sort(B.begin(), B.end());
        std::vector<T> result{};

        std::set_difference(A.begin(),A.end(), B.begin(), B.end(), std::back_inserter(result));
        return result;
    }

    template <typename T>
    bool vectorConstains(std::vector<T> vec, T elem)
    {
        if(std::find(vec.begin(), vec.end(), elem) != vec.end()) {
            return true;
        } else {
            return false;
        }
    }

    inline static bool ends_with(std::string const & value, std::string const & ending)
    {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    std::vector<std::string> split_string(std::string source, std::string splitters);
}