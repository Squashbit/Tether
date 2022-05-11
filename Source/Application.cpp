#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

using namespace Tether;

Application Application::internal = Application();

bool Application::Init()
{
    if (initialized)
        return false;
    storage = new Storage::AppVarStorage();

    if (!OnInit())
        return false;

    InitVulkan();

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
    FreeVulkan();
    OnAppDispose();
    
    delete storage;
}