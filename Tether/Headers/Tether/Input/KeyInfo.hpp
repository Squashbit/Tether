#ifndef _TETHER_INPUT_KEYINFO_HPP
#define _TETHER_INPUT_KEYINFO_HPP

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT KeyInfo
    {
    public:
        KeyInfo() {}
        KeyInfo(
            uint32_t scancode,
            uint32_t key,
            bool pressed
        );
        
        KeyInfo(const KeyInfo& ref);
		
        uint32_t GetScancode();
        uint32_t GetKey();
        bool IsPressed();
    private:
        uint32_t scancode;
        uint32_t key;
        bool pressed;
    };
}

#endif //_TETHER_INPUT_KEYINFO_HPP