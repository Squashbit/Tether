/*
 * The NativeWindow class uses native operating-system controls to draw
 * graphics. It cannot do nearly as complicated graphics as the Window class,
 * but it is useful for tool apps.
 * 
 * NOTICE:
 * This file is still in preview. If you want to use it, you have to declare
 * the TETHER_PREVIEW_FEATURES preprocessor definition.
 */

#ifndef _TETHER_NATIVEWINDOW_HPP
#define _TETHER_NATIVEWINDOW_HPP
#ifdef TETHER_PREVIEW_FEATURES

#include <Tether/IWindow.hpp>

namespace Tether
{
    namespace Controls
    {
        class Control;
    }

    class NativeWindow : public IWindow
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

#endif // TETHER_PREVIEW_FEATURES
#endif //_TETHER_NATIVEWINDOW_HPP