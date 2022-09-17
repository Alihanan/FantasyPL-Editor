#pragma once

#include "../lib_include.h"
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
}