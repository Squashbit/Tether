#pragma once

#include <Tether/Common/Defs.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

namespace Tether::VectorUtils
{
    template<typename VecType, typename T>
    void EraseAll(std::vector<VecType>& vec, T element)
    {
        vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
    }

    template<typename VecType, typename T>
    bool Contains(std::vector<VecType>& vec, T element)
    {
        return std::count(vec.begin(), vec.end(), element);
    }
}
