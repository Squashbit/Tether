#pragma once

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT KeyInfo
    {
    public:
        KeyInfo() = default;
        KeyInfo(
            uint32_t scancode,
            uint32_t key,
            bool pressed
        );
        
        uint32_t GetScancode();
        uint32_t GetKey();
        bool IsPressed();
    private:
        uint32_t scancode;
        uint32_t key;
        bool pressed;
    };
}
