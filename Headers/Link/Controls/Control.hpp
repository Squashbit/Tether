#ifndef _LINK_CONTROL_HPP
#define _LINK_CONTROL_HPP

#include <Link/Window.hpp>

#include <iostream>

namespace Link::Controls
{
    class Control
    {
        friend Window;
    public:
        Control() {}
        
        Control(const Control&) = delete;
		Control(Control&&) = delete;
		Control& operator=(const Control&) = delete;
		Control& operator=(Control&&) = delete;

        void SetX(float x);
        void SetY(float y);
        void SetWidth(float width);
        void SetHeight(float height);
        void SetForegroundColor(Color color);
        void SetBackgroundColor(Color color);
        float GetX() const;
        float GetY() const;
        float GetWidth() const;
        float GetHeight() const;
        Color GetForegroundColor() const;
        Color GetBackgroundColor() const;
    protected:
        virtual void Render(Window* pWindow) {}
    private:
        float x = 0, y = 0;
        float width = 0, height = 0;
        Color foreground;
        Color background;
    };
}

#endif //_LINK_CONTROL_HPP