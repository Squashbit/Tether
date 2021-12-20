#ifndef _LINK_VECTORUTILS_HPP
#define _LINK_VECTORUTILS_HPP

namespace Link::VectorUtils
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

#endif //_LINK_VECTORUTILS_HPP