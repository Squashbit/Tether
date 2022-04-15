#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

#include <dlfcn.h>

using namespace Tether;

Application Application::internal = Application();

bool Application::Init()
{
    if (initialized)
        return false;
    storage = new Storage::AppVarStorage();

    if (!OnInit())
        return false;

    initialized = true;
    return true;
}

Storage::AppVarStorage* Application::GetStorage()
{
    return storage;
}

Application& Application::Get()
{
    return internal;
}

void Application::OnDispose()
{
    OnAppDispose();
    
    delete storage;
}