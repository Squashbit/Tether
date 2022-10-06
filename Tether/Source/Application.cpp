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

    // Initialize all modules
    for (size_t i = 0; i < modules.size(); i++)
    {
        if (!modules[i]->OnInit())
        {
            std::cerr << "Tether > Module > " << modules[i]->GetModuleName()
                << ": Initialization failed" << std::endl;

            // If the module init failed, just skip it and move on.
            continue;
        }
    }

    initialized = true;
    return true;
}

void Application::RegisterModule(Module* pModule)
{
    modules.push_back(pModule);
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