#ifndef _TETHER_INPUT_MOUSEMOVEINFO_HPP
#define _TETHER_INPUT_MOUSEMOVEINFO_HPP

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT MouseMoveInfo
    {
    public:
        MouseMoveInfo() {}
        MouseMoveInfo(
            uint64_t rootX,
            uint64_t rootY,
            uint64_t relativeX,
            uint64_t relativeY,
            uint64_t oldRelativeX,
            uint64_t oldRelativeY,
            uint64_t oldRootX,
            uint64_t oldRootY
        );
        
        MouseMoveInfo(const MouseMoveInfo& ref);
		
        uint64_t GetRootX();
        uint64_t GetRootY();
        uint64_t GetRelativeX();
        uint64_t GetRelativeY();
        uint64_t GetOldRelativeX();
        uint64_t GetOldRelativeY();
        uint64_t GetOldRootX();
        uint64_t GetOldRootY();
    private:
        uint64_t rootX = 0;
        uint64_t rootY = 0;
        uint64_t relativeX = 0;
        uint64_t relativeY = 0;
        uint64_t oldRelativeX = 0;
        uint64_t oldRelativeY = 0;
        uint64_t oldRootX = 0;
        uint64_t oldRootY = 0;
    };
}

#endif //_TETHER_INPUT_MOUSEMOVEINFO_HPP