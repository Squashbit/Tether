#include <Link/Controls/Panel.hpp>

using namespace Link::Controls;

void Panel::Render(Window* pWindow)
{
    Color foreground = GetForegroundColor();
    glColor3f(foreground.GetR(), foreground.GetG(), foreground.GetB());
    
    glBegin(GL_POLYGON);
        glVertex2i(GetX(), GetY());
        glVertex2i(GetX() + GetWidth(), GetY());
        glVertex2i(GetX() + GetWidth(), GetY() + GetHeight());
        glVertex2i(GetX(), GetY() + GetHeight());
    glEnd();

    glFlush();
}
