#include <Tether/Application.hpp>
#include <Tether/Native.hpp>

using namespace Tether;

Application Application::internal = Application();

Application::Application()
{
	storage = new(std::nothrow) Storage::AppVarStorage();
	if (!storage)
		return;

	if (!OnInit())
		return;

	initialized = true;
}

void Application::RegisterModule(Module* pModule)
{
	modules.push_back(pModule);
}

Storage::AppVarStorage* Application::GetStorage()
{
	return storage;
}

std::vector<Module*>* Application::GetModules()
{
	return &modules;
}

Application& Application::Get()
{
	return internal;
}

void Application::OnDispose()
{
	// Dispose all modules
	for (size_t i = 0; i < modules.size(); i++)
		modules[i]->OnDispose();

	OnAppDispose();
	delete storage;
}