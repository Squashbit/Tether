#ifdef _WIN32

#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

#include <string.h>

using namespace Tether;

#undef LoadLibrary

static void* LoadLib(const char* path)
{
    return LoadLibraryA(path);
}

void* Application::LoadLibrary(const char* path)
{
    return LoadLib(path);
}

void* Application::LoadFunction(void* handle, const char* funcName)
{
    return GetProcAddress((HMODULE)handle, funcName);
}

static void FreeLib(void* library)
{
    FreeLibrary((HMODULE)library);
}

void Application::FreeLibrary(void* library)
{
    if (library)
        FreeLib(library);
}

bool Application::OnInit()
{
    return true;
}

void Application::OnAppDispose()
{
    FreeLibraries();
}

void Application::LoadLibraries()
{
    
}

void Application::LoadFunctions()
{
    
}

void Application::CreateKeyLUT()
{

}

void Application::FreeLibraries()
{
    
}

#endif //_WIN32