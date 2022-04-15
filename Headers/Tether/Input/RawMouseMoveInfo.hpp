#ifndef _TETHER_INPUT_RAWMOUSEMOVEINFO_HPP
#define _TETHER_INPUT_RAWMOUSEMOVEINFO_HPP

#include <stdint.h>

namespace Tether::Input
{
    class RawMouseMoveInfo
    {
    public:
        RawMouseMoveInfo() {}
        RawMouseMoveInfo(
            uint64_t rawX,
            uint64_t rawY
        );
        
        RawMouseMoveInfo(const RawMouseMoveInfo& ref);
		
        int64_t GetRawX();
        int64_t GetRawY();
    private:
        int64_t rawX = 0;
        int64_t rawY = 0;
    };
}

#endif //_TETHER_INPUT_RAWMOUSEMOVEINFO_HPP