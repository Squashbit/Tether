#ifndef _TETHER_BUTTON_HPP
#define _TETHER_BUTTON_HPP

#include <Tether/Controls/Control.hpp>

namespace Tether::Controls
{
    class Button : public Control
    {
    public:
        Button() {}
        
        Button(const Button&) = delete;
		Button(Button&&) = delete;
		Button& operator=(const Button&) = delete;
		Button& operator=(Button&&) = delete;
    private:
        void Render(Window* window);
    };
}

#endif //_TETHER_BUTTON_HPP