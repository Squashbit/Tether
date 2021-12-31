#ifndef _TETHER_WINDOW_HPP
#define _TETHER_WINDOW_HPP

#include <Tether/Common/Color.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Events/EventHandler.hpp>
#include <Tether/Events/EventType.hpp>

#include <vector>
#include <functional>

#ifdef __linux__
#include <Tether/XDefs.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif //__linux__

namespace Tether
{
    namespace Controls
    {
        class Control;
    }

    class Window : public IDisposable
    {
        friend Tether::Controls::Control;
    public:
        Window() {}
        
        Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

        /**
         * @brief Initializes the window.
         * 
         * @param pTetherContext A pointer to the Tether context.
         * @param x The initial X position of the window.
         * @param y The initial Y position of the window.
         * @param width The initial width of the window.
         * @param height The initial height of the window.
         * @param showAfterInit True if the window should be displayed after init.
         * @param stripped If the window should allow controls.
         * 
         * @returns True if the window was successfully created. 
         *      Otherwise, false.
         */
        bool Init(
            int	x = 200,
            int	y = 200,
            unsigned int width = 800,
            unsigned int height = 600,
            bool showAfterInit = true,
            bool stripped = false
        );

        /**
         * @brief Called when the window is initialized.
         */
        virtual void InitializeComponent() {}
        
        void AddEventHandler(Events::EventHandler& handler, 
            Events::EventType eventType);
        void AddEventHandler(Events::EventHandler* handler, 
            Events::EventType eventType);
        void RemoveEventHandler(Events::EventHandler& handler);
        void RemoveEventHandler(Events::EventHandler* handler);

        void SetVisible(bool visibility);
        bool IsVisible();

        void SetX(int64_t x);
        void SetY(int64_t y);
        void SetPosition(int64_t x, int64_t y);
        void SetWidth(uint64_t width);
        void SetHeight(uint64_t height);
        void SetSize(uint64_t width, uint64_t height);
        void SetTitle(const char* title);
        // Window X
        int64_t GetX();
        // Window Y
        int64_t GetY();
        uint64_t GetMouseX();
        uint64_t GetMouseY();
        uint64_t GetRelativeMouseX();
        uint64_t GetRelativeMouseY();
        uint64_t GetWidth();
        uint64_t GetHeight();
        
        /**
         * @brief Processes all pending events for the window.
         */
        void PollEvents();

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
         * @returns True, if the control was successfully removed.
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
         * @returns True, if the control was successfully removed.
         *      Otherwise, false.
         */
        bool RemoveControlNoRepaint(Controls::Control* pControl);

        /**
         * @brief Sets the background color of the window.
         */
        void SetBackgroundColor(Color backgroundColor);

        void ClearWindow();
        uint64_t GetHandle();

        bool IsCloseRequested();
        void IgnoreClose();

        void Repaint();
    protected:
        void OnDispose();

    #ifdef __linux__
        unsigned long window = 0;
        XEvent event;
    #endif //__linux__
    private:
        bool InitGraphics();
        void SwapBuffers();

        void SpawnEvent(
            Events::EventType eventType,
            std::function<void(Events::EventHandler*)> callEventFun
        );

        void DispatchNoInit(std::string functionName);

        void DisposeGraphics();
        
        std::vector<Controls::Control*> controls;

        // Window stuff
        int64_t x = 200;
        int64_t y = 200;
        uint64_t width = 800;
        uint64_t height = 600;
        bool visible = false;

        Color backgroundColor;

        // Mouse stuff
        bool prevReceivedMouseMove = false;
        uint64_t mouseX = 0;
        uint64_t mouseY = 0;
        uint64_t relMouseX = 0;
        uint64_t relMouseY = 0;

        // Each event has a list of handlers to handle that specific event.
        std::unordered_map<Events::EventType, 
            std::vector<Events::EventHandler*>> handlers;
        
        bool closeRequested = false;
        bool stripped = false;
    };
}

#endif //_TETHER_WINDOW_HPP