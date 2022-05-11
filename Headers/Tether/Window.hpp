/*
 * The Window class uses OpenGL to draw graphics. As a result, it can
 * draw far more complicated graphics then the NativeWindow class.
 * 
 * NOTICE:
 * This file is still in preview. If you want to use it, you have to compile tether
 * with the TETHER_PREVIEW_FEATURES option enabled.
 */

#ifndef _TETHER_WINDOW_HPP
#define _TETHER_WINDOW_HPP

#include <Tether/IWindow.hpp>

namespace Tether
{
    namespace Controls
    {
        class Control;
    }

    class Window : public IWindow
    {
        friend Tether::Controls::Control;
    public:
        Window() {}
        
        Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

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
        bool InitGraphics();
        void SwapBuffers();
        
        void DisposeGraphics();
        
        std::vector<Controls::Control*> controls;
        
        Color backgroundColor;
    };
}

#endif //_TETHER_WINDOW_HPP