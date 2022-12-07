#pragma once

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
    };
    
    struct WindowHint
    {
        HintType type;
        int64_t value;
    };
}
