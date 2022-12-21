#ifdef _WIN32

#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

#include <string.h>

using namespace Tether;

bool Application::OnInit()
{
    return true;
}

void Application::OnAppDispose()
{
    
}

void Application::LoadLibraries() {}
void Application::LoadFunctions() {}
void Application::FreeLibraries() {}
void Application::CreateKeyLUT() {}

#endif //_WIN32