#ifndef _TETHER_WINDOWHINT_HPP
#define _TETHER_WINDOWHINT_HPP

#include <stdint.h>

namespace Tether
{
    /**
     * @brief The type of window hint. Each window hint has a value.
     *  The comment next to each HintType entry describes the value ptr type.
     * @see WindowHint
     */
    enum class HintType
    {
        X, // int64 (screen coordinates)
        Y, // int64 (screen coordinates)
        WIDTH, // uint64
        HEIGHT, // uint64
        VISIBLE, // bool
    };
    
    struct WindowHint
    {
        HintType type;
        int64_t value;
    };
}

#endif //_TETHER_WINDOWHINT_HPP