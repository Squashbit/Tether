#pragma once

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT MouseClickInfo
    {
    public:
        MouseClickInfo() = default;
        MouseClickInfo(
            uint64_t rootX,
            uint64_t rootY,
            uint64_t relativeX,
            uint64_t relativeY
        );
        
        uint64_t GetRootX();
        uint64_t GetRootY();
        uint64_t GetRelativeX();
        uint64_t GetRelativeY();
    private:
        uint64_t rootX = 0;
        uint64_t rootY = 0;
        uint64_t relativeX = 0;
        uint64_t relativeY = 0;
    };
}
