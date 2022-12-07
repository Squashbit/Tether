#pragma once

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT RawMouseMoveInfo
    {
    public:
        RawMouseMoveInfo() = default;
        RawMouseMoveInfo(
            uint64_t rawX,
            uint64_t rawY
        );
        
        int64_t GetRawX();
        int64_t GetRawY();
    private:
        int64_t rawX = 0;
        int64_t rawY = 0;
    };
}
