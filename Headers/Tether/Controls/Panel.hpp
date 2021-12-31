#ifndef _TETHER_PANEL_HPP
#define _TETHER_PANEL_HPP

#include <Tether/Controls/Control.hpp>

namespace Tether::Controls
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

#endif //_TETHER_PANEL_HPP