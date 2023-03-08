#pragma once

#include <Tether/Common/Defs.hpp>
#include <stdint.h>

namespace Tether::Input
{
    class TETHER_EXPORT MouseClickInfo
    {
    public:
        enum class ClickType
        {
            LEFT_BUTTON   = 0,
            MIDDLE_BUTTON = 1,
            RIGHT_BUTTON  = 2,
            SIDE_BUTTON1  = 3,
            SIDE_BUTTON2  = 4,
        };

        MouseClickInfo(
            int rootX,
            int rootY,
            int relativeX,
            int relativeY,
            ClickType type,
            bool pressed
        );
        
        // Relative to the screen
        int GetRootX();
        int GetRootY();
        // Relative to the Window
        int GetRelativeX();
        int GetRelativeY();
        ClickType GetClickType();
        bool IsPressed();
    private:
        int rootX = 0;
        int rootY = 0;
        int relativeX = 0;
        int relativeY = 0;
        ClickType type = ClickType::LEFT_BUTTON;
        bool pressed = false;
    };
}
