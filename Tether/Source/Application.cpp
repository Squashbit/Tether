#include <Tether/Application.hpp>
#include <Tether/Native.hpp>
#include <stdexcept>

using namespace Tether;

Application::Application()
{
	storage = std::make_unique<Storage::AppVarStorage>();
	
	if (!OnInit())
		throw std::runtime_error("Failed to initialize Application");
}

Application::~Application()
{
	OnAppDispose();
}

Storage::AppVarStorage* Application::GetStorage()
{
	return storage.get();
}

int16_t*const Application::GetKeycodes()
{
	return keycodes;
}

int16_t*const Application::GetScancodes()
{
	return scancodes;
}

Application& Application::Get()
{
	if (!internal.get())
		internal = std::unique_ptr<Application>(new Application());

	return *internal;
}

void Application::DisposeApplication()
{
	internal.release();
}
