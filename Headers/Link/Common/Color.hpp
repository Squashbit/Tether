#ifndef _LINK_COLOR_HPP
#define _LINK_COLOR_HPP

#include <iostream>

namespace Link
{
    class Color
    {
    public:
        Color();
        Color(float color);
        Color(float r, float g, float b);

        Color(const Color& ref);
        
        /**
         * @brief Converts a color into a single value
         */
        static uint64_t RGB(float r, float g, float b);

        /**
         * @brief Converts the color into a single value
         */
        uint64_t GetRGB() const;

        void SetR(float r);
        void SetG(float g);
        void SetB(float b);
        float GetR() const;
        float GetG() const;
        float GetB() const;
    private:
        float r, g, b;
    };
}

#endif //_LINK_COLOR_HPP