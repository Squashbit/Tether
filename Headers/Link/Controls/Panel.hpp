#ifndef _LINK_PANEL_HPP
#define _LINK_PANEL_HPP

#include <Link/Controls/Control.hpp>

namespace Link::Controls
{
    class Panel : public Control
    {
    public:
        Panel() {}
        
        Panel(const Panel&) = delete;
		Panel(Panel&&) = delete;
		Panel& operator=(const Panel&) = delete;
		Panel& operator=(Panel&&) = delete;
    private:
        void Render(Window* window);
    };
}

#endif //_LINK_PANEL_HPP