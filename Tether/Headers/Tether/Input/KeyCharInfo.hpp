#ifndef _TETHER_INPUT_KEYCHARINFO_HPP
#define _TETHER_INPUT_KEYCHARINFO_HPP

#include <stdint.h>

namespace Tether::Input
{
    class KeyCharInfo
    {
    public:
        KeyCharInfo() {}
        KeyCharInfo(
            char key,
            bool autoRepeat
        );
        
        KeyCharInfo(const KeyCharInfo& ref);
		
        char GetKey();
        bool IsAutoRepeat();
    private:
        char key;
        bool autoRepeat;
    };
}

#endif //_TETHER_INPUT_KEYCHARINFO_HPP