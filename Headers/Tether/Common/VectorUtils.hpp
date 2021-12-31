#ifndef _TETHER_VECTORUTILS_HPP
#define _TETHER_VECTORUTILS_HPP

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

#endif //_TETHER_VECTORUTILS_HPP