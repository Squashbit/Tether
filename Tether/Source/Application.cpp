#include <Tether/Application.hpp>
#include <Tether/Native.hpp>
#include <stdexcept>

namespace Tether
{
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

	Tether::Storage::AppVarStorage* Tether::Application::GetStorage() const
	{
		return storage.get();
	}

	const int16_t* const Tether::Application::GetKeycodes() const
	{
		return keycodes;
	}

	const int16_t* const Tether::Application::GetScancodes() const
	{
		return scancodes;
	}

	Application& Application::Get()
	{
		if (!internal.get())
			internal = std::unique_ptr<Application>(new Application());

		return *internal;
	}
}
