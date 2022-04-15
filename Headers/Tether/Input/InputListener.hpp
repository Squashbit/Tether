#ifndef _TETHER_INPUT_INPUTLISTENER_HPP
#define _TETHER_INPUT_INPUTLISTENER_HPP

#include <Tether/Input/InputType.hpp>
#include <Tether/Input/MouseMoveInfo.hpp>
#include <Tether/Input/RawMouseMoveInfo.hpp>
#include <Tether/Input/KeyInfo.hpp>
#include <Tether/Input/KeyCharInfo.hpp>

namespace Tether
{
    class IWindow;
}

namespace Tether::Input
{
    class InputListener
    {
    public:
        virtual void OnMouseMove(MouseMoveInfo& info) {}
        virtual void OnRawMouseMove(RawMouseMoveInfo& info) {}
        virtual void OnKey(KeyInfo& info) {}
        virtual void OnKeyChar(KeyCharInfo& info) {}
    };
}

#endif //_TETHER_INPUTLISTENER_HPP