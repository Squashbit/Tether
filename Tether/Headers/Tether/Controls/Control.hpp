#pragma once

#include <Tether/NativeWindow.hpp>
#include <Tether/Common/Defs.hpp>

namespace Tether::Controls
{
    class TETHER_EXPORT Control
    {
        friend NativeWindow;
    public:
        Control() = default;
        
        void SetX(float x);
        void SetY(float y);
        void SetWidth(float width);
        void SetHeight(float height);
        void SetForegroundColor(Math::Vector4f color);
        void SetBackgroundColor(Math::Vector4f color);
        float GetX() const;
        float GetY() const;
        float GetWidth() const;
        float GetHeight() const;
        Math::Vector4f GetForegroundColor() const;
        Math::Vector4f GetBackgroundColor() const;
    protected:
        virtual void Render(NativeWindow* pWindow) {}
    private:
        float x = 0, y = 0;
        float width = 0, height = 0;
        Math::Vector4f foreground;
        Math::Vector4f background;
    };
}
