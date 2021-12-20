#ifndef _LINK_BUTTON_HPP
#define _LINK_BUTTON_HPP

#include <Link/Controls/Control.hpp>

namespace Link::Controls
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

#endif //_LINK_BUTTON_HPP