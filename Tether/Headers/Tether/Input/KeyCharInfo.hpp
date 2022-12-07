#pragma once

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT KeyCharInfo
    {
    public:
        KeyCharInfo() = default;
        KeyCharInfo(
            char key,
            bool autoRepeat
        );
        
        char GetKey();
        bool IsAutoRepeat();
    private:
        char key;
        bool autoRepeat;
    };
}
