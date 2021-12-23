#ifndef _LINK_WINDOW_HPP
#define _LINK_WINDOW_HPP

#include <Link/Common/Color.hpp>
#include <Link/Common/IDisposable.hpp>
#include <Link/Events/EventHandler.hpp>
#include <Link/Events/EventType.hpp>

#include <vector>
#include <functional>

#include <Link/glad/glad.h>
#include <Link/Graphics/VAO.hpp>
#include <Link/Graphics/Buffer.hpp>
#include <Link/Graphics/SimpleShader.hpp>

#ifdef __linux__
#include <Link/XDefs.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#endif //__linux__

namespace Link
{
    namespace Controls
    {
        class Control;
    }

    class Window : public IDisposable
    {
        friend Link::Controls::Control;
    public:
        Window() {}
        
        Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

        /**
         * @brief Initializes the window.
         * 
         * @param pLinkContext A pointer to the Link context.
         * @param x The initial X position of the window.
         * @param y The initial Y position of the window.
         * @param width The initial width of the window.
         * @param height The initial height of the window.
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
            bool stripped = false
        );

        /**
         * @brief Called when the window is initialized.
         */
        virtual void InitializeComponent() {}

        /**
         * @brief Sets the current glX context to this window.
         */
        void MakeCurrent();
        
        void AddEventHandler(Events::EventHandler& handler, 
            Events::EventType eventType);
        void AddEventHandler(Events::EventHandler* handler, 
            Events::EventType eventType);
        void RemoveEventHandler(Events::EventHandler& handler);
        void RemoveEventHandler(Events::EventHandler* handler);

        void Show();
        void Hide();

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
        void SetDrawingColor(Color color);
        void UseDefaultShader();
        void DrawRect(uint64_t x, uint64_t y, uint64_t width, uint64_t height);
        
        uint64_t GetHandle();

    #ifdef __linux__
        // Linux specific functions
        Display* GetXDisplay();
    #endif //__linux__

        bool IsCloseRequested();
        void IgnoreClose();

        void Repaint();
    protected:
        void OnDispose();

    #ifdef __linux__
        Display* display = nullptr;
        int screen = 0;
        unsigned long window = 0;
        XEvent event;
        Atom wmDelete;
        XVisualInfo* visual;
        GLXContext glxContext;
        Colormap cmap;
    #endif //__linux__
    private:
        void InitGraphics();
        void SwapBuffers();
        
        std::vector<Controls::Control*> controls;

        // Window stuff
        int64_t x = 200;
        int64_t y = 200;
        uint64_t width = 800;
        uint64_t height = 600;
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
        
        Graphics::VAO rectVAO;
        Graphics::Buffer rectVertexBuffer;
        
        bool closeRequested = false;
        bool stripped = false;
    };
}

#endif //_LINK_WINDOW_HPP