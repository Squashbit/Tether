#ifdef __linux__

#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

#include <dlfcn.h>
#include <string.h>

using namespace Tether;

void* Application::LoadLibrary(const char* path)
{
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}

void* Application::LoadFunction(void* handle, const char* funcName)
{
    return dlsym(handle, funcName);
}

void Application::FreeLibrary(void* library)
{
    if (library)
        dlclose(library);
}

bool Application::OnInit()
{
    storage->display = XOpenDisplay(NULL);
    if (!storage->display)
        return false;
    
    storage->screen = DefaultScreen(storage->display);
    storage->root = DefaultRootWindow(storage->display);

    XColor color = {};
    storage->hiddenCursorPixmap = XCreatePixmap(
        storage->display, storage->root, 1, 1, 1);
    storage->hiddenCursor = XCreatePixmapCursor(
        storage->display, 
        storage->hiddenCursorPixmap, storage->hiddenCursorPixmap, 
        &color, &color, 0, 0
    );

    LoadLibraries();
    LoadFunctions();

    CreateKeyLUT();
    
    return true;
}

void Application::OnAppDispose()
{
    XFreeCursor(storage->display, storage->hiddenCursor);
    XFreePixmap(storage->display, storage->hiddenCursorPixmap);

    XCloseDisplay(storage->display);

    FreeLibraries();
}

void Application::LoadLibraries()
{
    using namespace Storage;

    int garbage;

#if defined(__CYGWIN__)
    storage->xrr.handle = LoadLibrary("libXrandr-2.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    storage->xrr.handle = LoadLibrary("libXrandr.so");
#else
    storage->xrr.handle = LoadLibrary("libXrandr.so.2");
#endif

#if defined(__CYGWIN__)
    storage->xi.handle = LoadLibrary("libXi-6.so");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    storage->xi.handle = LoadLibrary("libXi.so");
#else
    storage->xi.handle = LoadLibrary("libXi.so.6");
#endif

    if (!XQueryExtension(storage->display, "XInputExtension", 
        &storage->xi.opcode, &garbage, &garbage))
        storage->xi.available = false;
}

void Application::LoadFunctions()
{
    using namespace Storage;

    if (storage->xi.handle && storage->xi.available)
    {
        storage->xi.selectEvents = (PFN_XISelectEvents)
            LoadFunction(storage->xi.handle, "XISelectEvents");
    }
}

static Keycodes TranslateScancode(const KeySym* keysyms, int width)
{
    if (width > 1)
    {
        switch (keysyms[1])
        {
            case XK_KP_0:           return Keycodes::KEY_KP_0;
            case XK_KP_1:           return Keycodes::KEY_KP_1;
            case XK_KP_2:           return Keycodes::KEY_KP_2;
            case XK_KP_3:           return Keycodes::KEY_KP_3;
            case XK_KP_4:           return Keycodes::KEY_KP_4;
            case XK_KP_5:           return Keycodes::KEY_KP_5;
            case XK_KP_6:           return Keycodes::KEY_KP_6;
            case XK_KP_7:           return Keycodes::KEY_KP_7;
            case XK_KP_8:           return Keycodes::KEY_KP_8;
            case XK_KP_9:           return Keycodes::KEY_KP_9;
            case XK_KP_Separator:
            case XK_KP_Decimal:     return Keycodes::KEY_KP_DECIMAL;
            case XK_KP_Equal:       return Keycodes::KEY_KP_EQUAL;
            case XK_KP_Enter:       return Keycodes::KEY_KP_ENTER;
            default:                break;
        }
    }

    switch (keysyms[0])
    {
        case XK_Escape:         return Keycodes::KEY_ESCAPE;
        case XK_Tab:            return Keycodes::KEY_TAB;
        case XK_Shift_L:        return Keycodes::KEY_LEFT_SHIFT;
        case XK_Shift_R:        return Keycodes::KEY_RIGHT_SHIFT;
        case XK_Control_L:      return Keycodes::KEY_LEFT_CONTROL;
        case XK_Control_R:      return Keycodes::KEY_RIGHT_CONTROL;
        case XK_Alt_L:          return Keycodes::KEY_LEFT_ALT;
        case XK_Alt_R:          return Keycodes::KEY_RIGHT_ALT;
        case XK_Super_L:        return Keycodes::KEY_LEFT_SUPER;
        case XK_Super_R:        return Keycodes::KEY_RIGHT_SUPER;
        case XK_Menu:           return Keycodes::KEY_MENU;
        case XK_Num_Lock:       return Keycodes::KEY_NUM_LOCK;
        case XK_Caps_Lock:      return Keycodes::KEY_CAPS_LOCK;
        case XK_Print:          return Keycodes::KEY_PRINT_SCREEN;
        case XK_Scroll_Lock:    return Keycodes::KEY_SCROLL_LOCK;
        case XK_Pause:          return Keycodes::KEY_PAUSE;
        case XK_Delete:         return Keycodes::KEY_DELETE;
        case XK_BackSpace:      return Keycodes::KEY_BACKSPACE;
        case XK_Return:         return Keycodes::KEY_ENTER;
        case XK_Home:           return Keycodes::KEY_HOME;
        case XK_End:            return Keycodes::KEY_END;
        case XK_Page_Up:        return Keycodes::KEY_PAGE_UP;
        case XK_Page_Down:      return Keycodes::KEY_PAGE_DOWN;
        case XK_Insert:         return Keycodes::KEY_INSERT;
        case XK_Left:           return Keycodes::KEY_LEFT;
        case XK_Right:          return Keycodes::KEY_RIGHT;
        case XK_Down:           return Keycodes::KEY_DOWN;
        case XK_Up:             return Keycodes::KEY_UP;
        case XK_F1:             return Keycodes::KEY_F1;
        case XK_F2:             return Keycodes::KEY_F2;
        case XK_F3:             return Keycodes::KEY_F3;
        case XK_F4:             return Keycodes::KEY_F4;
        case XK_F5:             return Keycodes::KEY_F5;
        case XK_F6:             return Keycodes::KEY_F6;
        case XK_F7:             return Keycodes::KEY_F7;
        case XK_F8:             return Keycodes::KEY_F8;
        case XK_F9:             return Keycodes::KEY_F9;
        case XK_F10:            return Keycodes::KEY_F10;
        case XK_F11:            return Keycodes::KEY_F11;
        case XK_F12:            return Keycodes::KEY_F12;
        case XK_F13:            return Keycodes::KEY_F13;
        case XK_F14:            return Keycodes::KEY_F14;
        case XK_F15:            return Keycodes::KEY_F15;
        case XK_F16:            return Keycodes::KEY_F16;
        case XK_F17:            return Keycodes::KEY_F17;
        case XK_F18:            return Keycodes::KEY_F18;
        case XK_F19:            return Keycodes::KEY_F19;
        case XK_F20:            return Keycodes::KEY_F20;
        case XK_F21:            return Keycodes::KEY_F21;
        case XK_F22:            return Keycodes::KEY_F22;
        case XK_F23:            return Keycodes::KEY_F23;
        case XK_F24:            return Keycodes::KEY_F24;
        case XK_F25:            return Keycodes::KEY_F25; // im gonna press the f25 key

        // Numeric keypad
        case XK_KP_Divide:      return Keycodes::KEY_KP_DIVIDE;
        case XK_KP_Multiply:    return Keycodes::KEY_KP_MULTIPLY;
        case XK_KP_Subtract:    return Keycodes::KEY_KP_SUBTRACT;
        case XK_KP_Add:         return Keycodes::KEY_KP_ADD;

        // These should have been detected in secondary keysym test above!
        case XK_KP_Insert:      return Keycodes::KEY_KP_0;
        case XK_KP_End:         return Keycodes::KEY_KP_1;
        case XK_KP_Down:        return Keycodes::KEY_KP_2;
        case XK_KP_Page_Down:   return Keycodes::KEY_KP_3;
        case XK_KP_Left:        return Keycodes::KEY_KP_4;
        case XK_KP_Right:       return Keycodes::KEY_KP_6;
        case XK_KP_Home:        return Keycodes::KEY_KP_7;
        case XK_KP_Up:          return Keycodes::KEY_KP_8;
        case XK_KP_Page_Up:     return Keycodes::KEY_KP_9;
        case XK_KP_Delete:      return Keycodes::KEY_KP_DECIMAL;
        case XK_KP_Equal:       return Keycodes::KEY_KP_EQUAL;
        case XK_KP_Enter:       return Keycodes::KEY_KP_ENTER;
        
        case XK_a:              return Keycodes::KEY_A;
        case XK_b:              return Keycodes::KEY_B;
        case XK_c:              return Keycodes::KEY_C;
        case XK_d:              return Keycodes::KEY_D;
        case XK_e:              return Keycodes::KEY_E;
        case XK_f:              return Keycodes::KEY_F;
        case XK_g:              return Keycodes::KEY_G;
        case XK_h:              return Keycodes::KEY_H;
        case XK_i:              return Keycodes::KEY_I;
        case XK_j:              return Keycodes::KEY_J;
        case XK_k:              return Keycodes::KEY_K;
        case XK_l:              return Keycodes::KEY_L;
        case XK_m:              return Keycodes::KEY_M;
        case XK_n:              return Keycodes::KEY_N;
        case XK_o:              return Keycodes::KEY_O;
        case XK_p:              return Keycodes::KEY_P;
        case XK_q:              return Keycodes::KEY_Q;
        case XK_r:              return Keycodes::KEY_R;
        case XK_s:              return Keycodes::KEY_S;
        case XK_t:              return Keycodes::KEY_T;
        case XK_u:              return Keycodes::KEY_U;
        case XK_v:              return Keycodes::KEY_V;
        case XK_w:              return Keycodes::KEY_W;
        case XK_x:              return Keycodes::KEY_X;
        case XK_y:              return Keycodes::KEY_Y;
        case XK_z:              return Keycodes::KEY_Z;
        case XK_1:              return Keycodes::KEY_1;
        case XK_2:              return Keycodes::KEY_2;
        case XK_3:              return Keycodes::KEY_3;
        case XK_4:              return Keycodes::KEY_4;
        case XK_5:              return Keycodes::KEY_5;
        case XK_6:              return Keycodes::KEY_6;
        case XK_7:              return Keycodes::KEY_7;
        case XK_8:              return Keycodes::KEY_8;
        case XK_9:              return Keycodes::KEY_9;
        case XK_0:              return Keycodes::KEY_0;
        case XK_space:          return Keycodes::KEY_SPACE;
        case XK_minus:          return Keycodes::KEY_MINUS;
        case XK_equal:          return Keycodes::KEY_EQUAL;
        case XK_bracketleft:    return Keycodes::KEY_LEFT_BRACKET;
        case XK_bracketright:   return Keycodes::KEY_RIGHT_BRACKET;
        case XK_backslash:      return Keycodes::KEY_BACKSLASH;
        case XK_semicolon:      return Keycodes::KEY_SEMICOLON;
        case XK_apostrophe:     return Keycodes::KEY_APOSTROPHE;
        case XK_grave:          return Keycodes::KEY_GRAVE_ACCENT;
        case XK_comma:          return Keycodes::KEY_COMMA;
        case XK_period:         return Keycodes::KEY_PERIOD;
        case XK_slash:          return Keycodes::KEY_SLASH;
        case XK_less:           return Keycodes::KEY_WORLD_1;
        default:                break;
    }
    
    return Keycodes::KEY_SPACE;
}

void Application::CreateKeyLUT()
{
    memset(storage->keycodes, -1, sizeof(storage->keycodes));

    XkbDescPtr desc = XkbGetMap(storage->display, 0, XkbUseCoreKbd);
    XkbGetNames(storage->display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

    int scanMin = desc->min_key_code;
    int scanMax = desc->max_key_code;

    int width;
    KeySym* keysyms = XGetKeyboardMapping(
        storage->display,
        scanMin,
        scanMax - scanMin + 1,
        &width
    );

    for (int i = scanMin; i <= scanMax; i++)
    {
        if (storage->keycodes[i] < 0)
        {
            const size_t base = (i - scanMin) * width;
            storage->keycodes[i] = 
                (int16_t)TranslateScancode(&keysyms[base], width);
        }
    }    

    XkbFreeNames(desc, XkbKeyNamesMask, true);
    XkbFreeKeyboard(desc, 0, true);
}

void Application::FreeLibraries()
{
    FreeLibrary(storage->xrr.handle);
    FreeLibrary(storage->xi.handle);
}

#endif //__linux__