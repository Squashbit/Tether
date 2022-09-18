#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

using namespace Tether;

Application Application::internal = Application();

bool Application::Init()
{
    if (initialized)
        return false;

    storage = new(std::nothrow) Storage::AppVarStorage();
    if (!storage)
        return false;

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
    DisposeVulkan();
    OnAppDispose();
    
    delete storage;
}