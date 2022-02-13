#ifndef _TETHER_WINDOWHINT_HPP
#define _TETHER_WINDOWHINT_HPP

#include <stdint.h>

namespace Tether
{
    /**
     * @brief The type of window hint.
     */
    enum class HintType
    {
        X,
        Y,
        WIDTH,
        HEIGHT,
        VISIBLE,
        FULLSCREEN,
    };
    
    struct WindowHint
    {
        HintType type;
        int64_t value;
    };
}

#endif //_TETHER_WINDOWHINT_HPP