#ifndef _TETHER_APPLICATION_HPP
#define _TETHER_APPLICATION_HPP

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif //__linux__

namespace Tether
{
    class Application;
    class ApplicationInternal
    {
        friend Application;
    public:
        ~ApplicationInternal();

        bool Init();

        bool IsInitialized();
    
    #ifdef __linux__
        Display* GetDisplay();
        Atom* GetWMDeleteAtom();
        int GetScreen();
    #endif

        void Dispose();
    protected:
        ApplicationInternal() {}
    private:
        Display* display = nullptr;
        Atom wmDelete;
        int screen = 0;

        bool initialized = false;
    };

    class Application
    {
    public:
        Application() = delete;
        ~Application();

        static bool Init();

        static bool IsInitialized();

    #ifdef __linux__
        static Display* GetDisplay();
        static Atom* GetWMDeleteAtom();
        static int GetScreen();
    #endif

        static void Dispose();
    private:
        static inline ApplicationInternal app;
    };
}

#endif //_TETHER_APPLICATION_HPP