#pragma once

#include <algorithm>
#include <set>

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
}