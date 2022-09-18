/*
 * The NativeWindow class uses native operating-system controls to draw
 * graphics. It cannot do nearly as complicated graphics as the GraphicalWindow class,
 * but it is useful for small applications.
 */

#ifndef _TETHER_NATIVEWINDOW_HPP
#define _TETHER_NATIVEWINDOW_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Color.hpp>
#include <Tether/SimpleWindow.hpp>

namespace Tether
{
    namespace Controls
    {
        class Control;
    }

    class NativeWindow : public SimpleWindow
    {
        friend Tether::Controls::Control;
    public:
        NativeWindow() {}
        
        NativeWindow(const NativeWindow&) = delete;
		NativeWindow(NativeWindow&&) = delete;
		NativeWindow& operator=(const NativeWindow&) = delete;
		NativeWindow& operator=(NativeWindow&&) = delete;

        /**
         * @brief Adds a control to the window.
         * 
         * @param pControl A pointer to the control to add.
         */
        void AddControl(Controls::Control* pControl);
        /**
         * @brief Removes a control from the window.
         * 
         * @param pControl A pointer to the control to remove.
         * 
         * @returns True, if the control was successfully removed;
         *      Otherwise, false.
         */
        bool RemoveControl(Controls::Control* pControl);

        /**
         * @brief Adds a control to the window without repainting.
         * 
         * @param pControl A pointer to the control to add.
         */
        void AddControlNoRepaint(Controls::Control* pControl);
        /**
         * @brief Removes a control from the window without repainting.
         * 
         * @param pControl A pointer to the control to remove.
         * 
         * @returns True, if the control was successfully removed;
         *      Otherwise, false.
         */
        bool RemoveControlNoRepaint(Controls::Control* pControl);

        void ClearWindow();
        
        /**
         * @brief Sets the background color of the window.
         */
        void SetBackgroundColor(Color backgroundColor);
        
        void Repaint();
    private:
        std::vector<Controls::Control*> controls;
        Color backgroundColor;
    };
}

#endif //_TETHER_NATIVEWINDOW_HPP