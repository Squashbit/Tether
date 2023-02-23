#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Input/InputType.hpp>
#include <Tether/Input/MouseMoveInfo.hpp>
#include <Tether/Input/MouseClickInfo.hpp>
#include <Tether/Input/RawMouseMoveInfo.hpp>
#include <Tether/Input/KeyInfo.hpp>
#include <Tether/Input/KeyCharInfo.hpp>

#include <vector>

namespace Tether
{
    class Window;
}

namespace Tether::Input
{
    class TETHER_EXPORT InputListener
    {
        friend Window;
    public:
        virtual void OnMouseMove(MouseMoveInfo& info) {}
        virtual void OnMouseClick(MouseClickInfo& info) {}
        virtual void OnRawMouseMove(RawMouseMoveInfo& info) {}
        virtual void OnKey(KeyInfo& info) {}
        virtual void OnKeyChar(KeyCharInfo& info) {}
    };
}
